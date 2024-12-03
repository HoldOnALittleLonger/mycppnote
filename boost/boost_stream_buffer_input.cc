#include<iostream>
#include<boost/iostreams/stream_buffer.hpp>
#include<boost/iostreams/device/file.hpp>

namespace boost_io = boost::iostreams;

int main(void)
{
  boost_io::stream_buffer<boost_io::file_source> local_file("boost_stream_buffer_input_testfile");
  if (!local_file.is_open())
    return 1;

  std::istream input(&local_file);
  char c('\0');
  do {
    if (input.eof()) {
      std::cout<<std::endl<<"EOF"<<std::endl;
      break;
    }
    input>>c;
    std::cout<<c<<" ";
  } while (1);

  local_file.close();
  return 0;
}
