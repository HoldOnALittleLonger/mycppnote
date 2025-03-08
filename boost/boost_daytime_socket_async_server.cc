#include <ctime>
#include <cstddef>
#include <memory>
#include <functional>
#include <string>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace basio = boost::asio;
namespace bai = basio::ip;

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:

  typedef std::shared_ptr<tcp_connection> pointer;

  static pointer makeup(basio::io_context &io)
  {
    return pointer(new tcp_connection(io));
  }

  bai::tcp::socket &socket(void)
  {
    return socket_;
  }

  void start_process(void)
  {
    time_t now(time(0));
    std::string_view time_str(ctime(&now));
    basio::async_write(socket_, basio::buffer(time_str),
                       std::bind(&tcp_connection::handle_write, 
                                 shared_from_this(),
                                 basio::placeholders::error,
                                 basio::placeholders::bytes_transferred
                                 )
                       );

  }

private:

  tcp_connection(basio::io_context &io)
    : socket_(io)
  {
  }


  void handle_write(const boost::system::error_code &, std::size_t)
  {
  }

  bai::tcp::socket socket_;
};

class tcp_async_server {
public:
  tcp_async_server(basio::io_context &io)
    : io_context_(io), 
      acceptor_(io, bai::tcp::endpoint(bai::tcp::v4(), 13))
  {
    start_accept();
  }

private:

  void start_accept()
  {
    tcp_connection::pointer t(tcp_connection::makeup(io_context_));
    acceptor_.async_accept(t->socket(),
                           std::bind(&tcp_async_server::handle_accept,
                                     this,
                                     basio::placeholders::error,
                                     t));

  }

  void handle_accept(const boost::system::error_code &ec, tcp_connection::pointer connection)
  {
    if (!ec)
      connection->start_process();
    start_accept();
  }

  basio::io_context &io_context_;
  bai::tcp::acceptor acceptor_;
};

int main(void)
{
  basio::io_context io_context;
  tcp_async_server server(io_context);
  io_context.run();

  return 0;
}
