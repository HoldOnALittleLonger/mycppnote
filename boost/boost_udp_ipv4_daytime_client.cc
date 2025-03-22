#include <iostream>
#include <array>

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include <cstddef>

namespace asio = boost::asio;

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage : @this-program <remote server ip>" << std::endl;
    return 1;
  }

  try {
    asio::io_context io;
    asio::ip::udp::resolver udp_resolver(io);
    asio::ip::udp::socket udp_socket(io);
    udp_socket.open(asio::ip::udp::v4());
    asio::ip::udp::endpoint remote_endpoint = *udp_resolver.resolve(asio::ip::udp::v4(), argv[1], "daytime").begin();

    // used as notification for remote server
    std::array<char, 1> send_buf{0};

    udp_socket.send_to(asio::buffer(send_buf), remote_endpoint);

    std::array<char, 256> recv_buf{0};
    asio::ip::udp::endpoint local_endpoint;

    ssize_t len = udp_socket.receive_from(asio::buffer(recv_buf), local_endpoint);

    std::cout.write(recv_buf.data(), len);

  } catch (const boost::system::system_error &ec) {
    std::cerr << ec.what() << std::endl;
    return 1;
  }

  return 0;
}
