#ifndef POST_H
#define POST_H

#include <string>
#include <chrono>
#include "ccpp_tsn.h"

//std::string message
class post
{
  private:
    TSN::serial_number serial_num;
    std::string post_body; //message of post
    long creation_date;
    
  public:
    post(TSN::serial_number sn, std::string body, long doc);
    std::string get_body(); //return the post message
    long get_doc(); //return the date of creation
    TSN::serial_number get_sn(); //return the post's serial num
};

#endif
