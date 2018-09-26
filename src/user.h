#ifndef USER_H
#define USER_H

#include <string>
#include "ccpp_tsn.h"
#include "post.h"

class user(std::string fname, std::string lname, sequence<std::string> i) 
{
  public:
    std::string first_name;
    std::string last_name;
    long date_of_birth;
    char uuid[TSN::UUID_SIZE];
    sequence<string> interests;
    std::vector<post> posts;

    TSN::request create_request();
     //method will get who we want posts from and the serial number of those posts as input from user to create
     //a node request, which will then make and return a user_request 
    

    TSN::response create_response(char *uuid, post requested_post);
   //method will look through post information stored on local file to find the request post serial number and 
  //create and return a response

};

#endif
