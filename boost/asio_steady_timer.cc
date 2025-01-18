#include <iostream>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace basio = boost::asio;

int main(void)
{
  basio::io_context iocontext;

  basio::steady_timer stimer(iocontext, basio::chrono::seconds(3));

  std::cout << "Ready to wait 3s." << std::endl;
  try {
    stimer.wait();
  } catch (boost::system::error_code &ec) {
    std::cerr << "Failed to wait.Detected error throwing - " << std::endl;
    char message_buffer[256] = {0};
    ec.message(message_buffer, 256);
    std::cerr << message_buffer << std::endl;
  }

  std::cout << "Time expired." << std::endl;

  return 0;
}
