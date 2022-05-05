#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdio>
#include <cstdlib>
//#include <cstring>
#include <string>

using namespace std;

#define HEADER_LEN 4

/* Class defines the response structure */
class response_
{
  public:
    enum { header_length = HEADER_LEN };
    enum { max_body_length = 1024 };

    response_(string value)
      : body_length_(0)
    {
      encode_message(value);
    }

    const char* data() const
    {
      return data_;
    }

    char* data()
    {
      return data_;
    }

    int length() const
    {
      return header_length + body_length_;
    }

    const char* body() const
    {
      return data_ + header_length;
    }

    char* body()
    {
      return data_ + header_length;
    }

    int body_length() const
    {
      return body_length_;
    }

    void set_body_length(int new_length)
    {
      body_length_ = new_length;
      if (body_length_ > max_body_length)
        body_length_ = max_body_length;
    }

    void encode_message(string value)
    {
      set_body_length(value.size());
      int len  = 0;
      if (body_length_ > 0) {
        len = int (log10(body_length_)) + 1;
      }
      // Prepare header
      memcpy (data_, "0000", header_length - len);
      if (value.size() > 0) {
        memcpy(data_ + (header_length - len),
          to_string(body_length_).c_str(), len);
        // copy body
        memcpy(data_ + header_length, value.c_str(), value.size());
      }

      // // Another way to write header length
      // using namespace std; // For sprintf and memcpy.
      // char header[header_length + 1] = "";
      // sprintf(header, "%4d", body_length_);
      // memcpy(data_, header, header_length);
    }

  private:
    /* First "header_length" byte contatin size of the message and remaining bytes is body */
    char data_[header_length + max_body_length];
    int body_length_;
};

typedef response_ response_t;

#endif // UTILS_HPP
