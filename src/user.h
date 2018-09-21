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
    char uuid[17];
    sequence<string> interests;
    std::vector<post> posts;

    TSN::request create_request(char *uuid, sequence<node_request> user_reqs); 
    TSN::response create_response(char *uuid, post requested_post);

};

#endif
