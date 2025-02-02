#include <iostream>
#include <functional>

#include <cstddef>

#include <boost/asio.hpp>

namespace basio = boost::asio;

constexpr std::size_t MAX_COUNT(8);

class my_tick_second {
public:

  explicit my_tick_second(basio::io_context &io)
    : t_(io, basio::chrono::seconds(1)), counter_(0)
  {
    t_.async_wait(std::bind(&my_tick_second::tick_second, this));
    io.run();
  }


    void tick_second()
  {
    if (counter_++ < MAX_COUNT) {
      std::cout << "Tick" << std::endl;
      t_.expires_at(t_.expiry() + basio::chrono::seconds(1));
      t_.async_wait(std::bind(&my_tick_second::tick_second, this));
    } else {
      std::cout << "Exceeded maximum count" << std::endl;
    }
  }

private:
  basio::steady_timer t_;
  std::size_t counter_;
};

int main(void)
{
  basio::io_context io;
  my_tick_second mts(io);
  return 0;
}
