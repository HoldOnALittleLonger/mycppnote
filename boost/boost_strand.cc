#include <iostream>
#include <functional>
#include <thread>
#include <cstddef>

#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>

constexpr std::size_t MAX_COUNTER(8);

namespace basio = boost::asio;

class ticker {
public:

  void tick1_second()
  {
    if (_counter++ < MAX_COUNTER) {
      std::cout << _counter << " ticked - Ticker1." << std::endl;
      _t1.expires_at(_t1.expiry() + basio::chrono::seconds(1));
      _t1.async_wait(basio::bind_executor(_strand,
                                          std::bind(&ticker::tick1_second, this)
                                          )
                     );
    }
  }

  void tick2_second()
  {
    if (_counter++ < MAX_COUNTER) {
      std::cout << _counter << " ticked - Ticker2." << std::endl;
      _t2.expires_at(_t1.expiry() + basio::chrono::seconds(1));
      _t2.async_wait(basio::bind_executor(_strand,
                                          std::bind(&ticker::tick2_second, this)
                                          )
                     );
    }
  }

  ticker(basio::io_context &io)
    : _strand(basio::make_strand(io)),
      _t1(io, basio::chrono::seconds(1)),
      _t2(io, basio::chrono::seconds(1))
  {
    _counter = 0;
    _t1.async_wait(basio::bind_executor(_strand,
                                        std::bind(&ticker::tick1_second, this)
                                        )
                   );
    _t2.async_wait(basio::bind_executor(_strand,
                                        std::bind(&ticker::tick2_second, this)
                                        )
                   );
  }

private:
  basio::strand<basio::io_context::executor_type> _strand;
  basio::steady_timer _t1;
  basio::steady_timer _t2;
  std::size_t _counter;
};

void threadFunc1(basio::io_context *io)
{
  std::cout << __FUNCTION__ << " started." << std::endl;
  io->run();
}

void threadFunc2(basio::io_context *io)
{
  io->run();
  std::cout << __FUNCTION__ << " accomplished." << std::endl;
}

int main(void)
{
  basio::io_context io;
  ticker t(io);

  std::thread thread1(threadFunc1, &io);
  std::thread thread2(threadFunc2, &io);
  thread1.join();
  thread2.join();
  
  return 0;
}
