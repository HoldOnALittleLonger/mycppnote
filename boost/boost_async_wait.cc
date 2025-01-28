#include <iostream>
#include <thread>

#include <cstddef>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace basio = boost::asio;

static int completion(0);

void completion_handler(boost::system::error_code ec)
{
  std::cout << "completion handler was called." << std::endl;
  completion = 1;
}

void thread_tell_completion(basio::io_context *io)
{
  std::this_thread::sleep_for(std::chrono::seconds(5));

  //  the pointer can not be const,if it is,then no such 
  //  run() with const @this to be matched.
  io->run();
}

int main(void)
{
  basio::io_context asioc;
  basio::steady_timer t(asioc, basio::chrono::seconds(5));

  t.async_wait(completion_handler);
  
  //  std::thread requires the argument of the callback function
  //  must be rvalue or it can be converted to rvalue.
  //  just convert an object from lvalue to rvalue through
  //  std::move() is not suitable at there,thus pass it by-pointer.
  std::thread t1(thread_tell_completion, &asioc);
  do {
    std::cout << "Waiting condition completes." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (!completion);
  
  t1.join();

  return 0;
}
