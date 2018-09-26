#ifndef POST_H
#define POST_H

#include <string>
#include "ccpp_tsn.h"

class post(string message)
{
  private:
    TSN::serial_number id;
    std::string post_body; //message of post
    long creation_date;
    
  public:
    std::string get_body(); //return the post message
    long get_doc(); //return the date of creation
    TSN::serial_number get_id(); //return the post's serial num
};

#endif
