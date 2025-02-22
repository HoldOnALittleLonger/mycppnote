#include <iostream>
#include <array>
#include <cstddef>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace basio = boost::asio;

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "No host is given." << std::endl;
    return 1;
  }

  basio::io_context io;

  basio::ip::tcp::resolver r(io);
  basio::ip::tcp::resolver::results_type endpoint{r.resolve(argv[1], "daytime")};

  basio::ip::tcp::socket s(io);

  try {
    basio::connect(s, endpoint);
  } catch (boost::system::error_code &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  for (;;) {
    std::array<char, 128> buffer;
    boost::system::error_code ec;
    std::size_t readed = s.read_some(basio::buffer(buffer), ec);
    if (ec == boost::asio::error::eof)
      break;
    else if (ec)
      throw boost::system::system_error(ec);

    std::cout.write(buffer.data(), readed);
  }

  try {
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
