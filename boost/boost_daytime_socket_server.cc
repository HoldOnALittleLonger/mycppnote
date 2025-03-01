#include <ctime>
#include <cstddef>
#include <string>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

namespace basio = boost::asio;

int main(void)
{
  basio::io_context io;
  basio::ip::tcp::acceptor acceptor(io,
                                    basio::ip::tcp::endpoint(basio::ip::tcp::v4(), 13));  
  for (;;) {
    basio::ip::tcp::socket socket(io);
    acceptor.accept(socket);
    time_t now(time(0));
    std::string time_str(ctime(&now));
    boost::system::error_code ignored_ec;
    basio::write(socket, basio::buffer(time_str), ignored_ec);
  }

  return 0;
}


