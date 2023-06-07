#include<coroutine>
#include<iostream>
#include<cstdbool>

/*struct coroutine {
  struct promise_type {
    coroutine get_return_object() { return {._chpt = std::coroutine_handle<promise_type>::from_promise(*this)}; }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {};
    void unhandled_exception() {}
  };

  std::coroutine_handle<promise_type> _chpt;
  operator std::coroutine_handle<promise_type>() const { return _chpt; }
  operator std::coroutine_handle<>() const { return _chpt; }
};
*/

/*
struct promise;
struct coroutine : std::coroutine_handle<promise> {
  using promise_type = ::promise;
};

struct promise {
  coroutine get_return_object()
  {
    return {coroutine::from_promise(*this)};
  }
  std::suspend_always initial_suspend() { std::cout<<"invoke initial_suspend()."<<std::endl; return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  void return_void() {};
  void unhandled_exception() {};
};




struct awaiter {
  bool await_ready() { return false; }
  void await_suspend(std::coroutine_handle<> h) {}
  void await_resume() { std::cout<<"await_resume() executed."<<std::endl; }
};

coroutine function(int i)
{
  std::cout<<"resume initial_suspend()."<<std::endl;
  awaiter Awaiter;
  std::cout<<"employ awaiter,ready enter cycle."<<std::endl;
  for (int a = i; true; ++a) {
    std::cout<<a<<std::endl;
    co_await Awaiter;
  }
}

//  coroutine external_handle = function(0);
//  external_handle();
//  external_handle.resume();

*/

struct custom_awaiter {
  bool await_ready() { std::cout<<"custom_awaiter ready."<<std::endl; return false; }
  bool await_suspend(std::coroutine_handle<> h) { std::cout<<"custom_awaiter suspend."<<std::endl; return false; }
  void await_resume() noexcept { std::cout<<"custom_awaiter resume."<<std::endl; }
};

struct coroutine {
  struct promise_type {
    coroutine get_return_object() { return {}; }
    custom_awaiter initial_suspend() { std::cout<<"initial_suspend() executed."<<std::endl; return {}; }
    std::suspend_never final_suspend() noexcept { return {}; };
    void return_void() {}
    void unhandled_exception() {}
  };
};

struct awaiter {
  std::coroutine_handle<> *_ch;
  bool await_ready() { return false; }
  void await_suspend(std::coroutine_handle<> h) { *_ch = h; }
  void await_resume() {}
};

coroutine function(std::coroutine_handle<> *p)
{
  awaiter a{p};
  std::cout<<"ready enter cycle."<<std::endl;
  for (int i = 0; ; ++i) {
    std::cout<<"i = "<<i<<std::endl;
    co_await a;
  }
}


struct coroutine_second {
  struct promise_type {
    int _v;
    coroutine_second get_return_object() { return {std::coroutine_handle<promise_type>::from_promise(*this)}; }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
    std::suspend_never yield_value(int i) { _v = i; return {}; }
  };

  std::coroutine_handle<promise_type> _cschpt;
  operator std::coroutine_handle<>() const { return _cschpt; }
  operator std::coroutine_handle<promise_type>() const { return _cschpt; }
};

struct awaiter_second {
  coroutine_second::promise_type *_pP;
  bool await_ready() { return false; }
  bool await_suspend(std::coroutine_handle<coroutine_second::promise_type> h)
  {
    _pP = &h.promise();
    return false;
  }
  coroutine_second::promise_type *await_resume() { return _pP;}
};

coroutine_second function2(int i)
{
  coroutine_second::promise_type *internal_promise = co_await awaiter_second{};
  std::cout<<"ready enter cycle."<<std::endl;
  for (int c(i); ; ++c) {
    std::cout<<"in cycle. "<<std::endl;
    internal_promise->_v = c;  //  same as next statement
    co_yield c;
    std::cout<<"updated value."<<std::endl;
    co_await std::suspend_always{};
  }
}

int main(void)
{
  std::coroutine_handle<> h;
  function(&h);
  h.resume();
  h();

  std::coroutine_handle<coroutine_second::promise_type> h2 = function2(16);
  coroutine_second::promise_type &rh2 = h2.promise();
  for (int i = 3; i >= 0; --i) {
    std::cout<<"value = "<<rh2._v<<std::endl;
    h2();
  }

  h.destroy();
  h2.destroy();

  return 0;
}
