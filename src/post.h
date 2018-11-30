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

    char child_uuid[TSN::UUID_SIZE];
    TSN::serial_number child_post_id;
    
  public:
    post();
    post(TSN::serial_number sn, std::string body, long doc, bool thread, TSN::serial_number parent_post_id, char* parent_uuid); //constructor
    std::string get_body(); //return the post message
    long get_doc(); //return the date of creation
    TSN::serial_number get_sn(); //return the post's serial num

    TSN::serial_number get_parent_sn();
    char* get_parent_uuid();
    void set_child_uuid(char* uuid);
    void set_child_post(TSN::serial_number post_id);
    
};

#endif
