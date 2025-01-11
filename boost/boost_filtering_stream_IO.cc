#include <unistd.h>
#include <fcntl.h>

#include <ctype.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/concepts.hpp>

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
           !isalpha(c));
    return (c == EOF) ? c : c + 1;
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
    return boost::iostreams::put(sink, static_cast<char_type>(toupper(c)));
  }
};

#define SOURCE_FILE  "boost_filtering_istream_source"
#define SINK_FILE  "boost_filtering_ostream_sink"

int main(void)
{
  //  boost filtering istream and boost filtering ostream
  boost::iostreams::filtering_istream in;
  boost::iostreams::filtering_ostream out;

  //  Linux file descriptor
  int fdIn = -1, fdOut = - 1;

  //  POSIX open
  fdIn = open(SOURCE_FILE, O_RDONLY);
  fdOut = open(SINK_FILE, O_WRONLY);

  if (fdIn < 0)
    return -1;

  if (fdOut < 0)
    return -1;

  //  source and sink
  boost::iostreams::file_descriptor_source inFile;
  boost::iostreams::file_descriptor_sink outFile;
  //  use deprecated method open() on handle
  inFile.open(fdIn, boost::iostreams::never_close_handle);
  outFile.open(fdOut, boost::iostreams::never_close_handle);

  //  C-Language string version for pathname is also provided
  //  by boost.

  //  reserve order to pushed
  in.push(input_filter_increase());
  in.push(inFile);

  //  in order they pushed
  out.push(output_filter_toupper());
  out.push(outFile);

  char c('\0');
  while (in >> c && c != EOF) {
    //    output_filter_toupper().put(outFile, c);  //  manually create filter
        out << c;
    /**
     * <fixed>
     * use @out the object is type of boost::iostreams::filtering_ostream would
     * leave @sink unchanged,that is,no data is written into it.
     * filter function put() exactly called,and call to boost::iostreams::put()
     * is succeed,too.
     * but strang,the file have no data in it.
     *
     * c++ iostream flush buffer at the time when basic_iostream::tie() was called,
     * or the object is destructing.
     * boost iostreams is similar as such.
     * but the local object is destructed when function main exit,and the code
     * close Linux file descriptor before than occurence of boost iostreams flush.
     * thus,all data in the buffer will be dropped.
     */
  }
  out.flush();

  close(fdIn);
  close(fdOut);

  return 0;
}
