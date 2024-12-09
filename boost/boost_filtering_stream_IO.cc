//  Because I did not compile boost library and install,
//  thus I can not compile this.


#include <unistd.h>
#include <fcntl.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/concepts.hpp>

#include <ctype.h>

using boost::iostreams::input_filter, boost::iostreams::output_filter;


struct input_filter_increase : input_filter {
  typedef input_filter::char_type char_type;
  typedef input_filter::category category;

  template<typename Source>
  int get(Source &src)
  {
    char_type c('\0');
    
    while ((c = boost::iostreams::get(src)) != EOF
           &&
           !isalpha(c))
      ;
    return c + 1;
  }

};

//  input filter use method get() to retrieve a character from source
//  output filter use method put() to place a character into sink

struct output_filter_toupper : output_filter {
  typedef output_filter::char_type char_type;
  typedef output_filter::category category;

  template<typename Sink>
  bool put(Sink &sink, char_type c)
  {
    return boost::iostreams::put(sink, toupper(c));
  }
};

#define SOURCE_FILE "boost_filtering_istream_source"
#define SINK_FILE "boost_filtering_ostream_sink"

int main(void)
{
  boost::iostreams::filtering_istream in;
  boost::iostreams::filtering_ostream out;

  int fdIn = -1, fdOut = - 1;

  //  POSIX open
  fdIn = open(SOURCE_FILE, O_RDONLY);
  fdOut = open(SINK_FILE, O_WRONLY);

  boost::iostreams::file_descriptor_source inFile;
  boost::iostreams::file_descriptor_sink outFile;

  if (fdIn < 0)
    return -1;

  if (fdOut < 0)
    return -1;


  //  !!  implementation of method open is depends on what
  //      the system is.
  //      so,boost library is required.
  inFile.open(fdIn, boost::iostreams::never_close_handle);
  outFile.open(fdOut, boost::iostreams::never_close_handle);

  //  C-Language string version for pathname is also provided
  //  by boost.


  in.push(input_filter_increase());
  in.push(inFile);


  out.push(output_filter_toupper());
  out.push(outFile);

  char c('\0');

  while (in >> c && c != EOF)
    out << c;

  close(fdIn);
  close(fdOut);

  return 0;
}
