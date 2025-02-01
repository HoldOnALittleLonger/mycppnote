#include <iostream>
#include <functional>

#include <cstddef>

#include <boost/asio.hpp>

namespace basio = boost::asio;

constexpr std::size_t max_count(8);

void tick_second(const boost::system::error_code &ec,
                 basio::steady_timer *t, std::size_t *counter)
{
  if (*counter < max_count) {
    std::cout << "Tick" << std::endl;
    ++*counter;
    //  adjust timer expiry,all works will be done before next time tick comes
    t->expires_at(t->expiry() + basio::chrono::seconds(1));
    //  initiate a new asynchronous wait operation
    t->async_wait(std::bind(tick_second, basio::placeholders::error,
                            t, counter));
  } else {
    std::cout << "Exceeded max tick count - " << max_count << std::endl;
  }
}

int main(void)
{
  basio::io_context ioc;
  basio::steady_timer t(ioc, basio::chrono::seconds(1));
  std::size_t counter(0);
  t.async_wait(std::bind(tick_second, basio::placeholders::error,
                         &t, &counter));
  ioc.run();
  return 0;
}
