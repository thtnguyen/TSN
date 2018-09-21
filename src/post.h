#ifndef POST_H
#define POST_H

#include <string>
#include "ccpp_tsn.h"

class post(string message)
{
  private:
    TSN::serial_number id;
    string post_body; //message of post
    long creation_date;
    
  public:
    string get_body();
    long get_doc(); //date of creation
    TSN::serial_number get_id();
};

#endif
