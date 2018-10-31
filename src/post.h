#ifndef POST_H
#define POST_H

#include <string>
#include <chrono>
#include "ccpp_tsn.h"
#include <vector>

class post
{
  private:
    TSN::serial_number serial_num;
    std::string post_body; //message of post
    long creation_date;
    char parent_uuid[TSN::UUID_SIZE];
    TSN::serial_number parent_post_id;

    std::vector<std::string> children_uuids;
    std::vector<TSN::serial_number> children_post_ids;
    
  public:
    post(TSN::serial_number sn, std::string body, long doc, bool thread, TSN::serial_number parent_post_id, char* parent_uuid); //constructor
    std::string get_body(); //return the post message
    long get_doc(); //return the date of creation
    TSN::serial_number get_sn(); //return the post's serial num

    
};

#endif
