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

int main(void)
{
  std::coroutine_handle<> h;
  function(&h);
  h.resume();
  h();
  return 0;
}
