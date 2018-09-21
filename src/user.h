#ifndef USER_H
#define USER_H

#include <string>
#include "ccpp_tsn.h"
#include "post.h"

class user(string fname, string lname, sequence<string> i) 
{
  public:
    string first_name;
    string last_name;
    long date_of_birth;
    char uuid[17];
    sequence<string> interests;
    vector<post> posts;

    void load_user_data(string file_name); //loads all user info at program initialization
    TSN::request create_request(char *uuid, sequence<node_request> user_reqs); 
    TSN::response create_response(char *uuid, post requested_post);

};

#endif
