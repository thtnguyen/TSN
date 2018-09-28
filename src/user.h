#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "ccpp_tsn.h"
#include "post.h"

//std::string fname, std::string lname, sequence<std::string> i
class user
{
  public:
    std::string first_name;
    std::string last_name;
    long date_of_birth;
    char uuid[TSN::UUID_SIZE];
    std::vector<std::string> interests;
    std::vector<post> posts;

    user(std::string fname, std::string lname, long dob, char *id, std::vector<std::string>& i, std::vector<post>& p);
    TSN::request create_request();
     //method will get who we want posts from and the serial number of those posts as input from user to create
     //a node request, which will then make and return a user_request 
    

    TSN::response create_response(char *uuid, post requested_post);
   //method will look through post information stored on local file to find the request post serial number and 
  //create and return a response

    //prompt user for post data and construct a post object to store in the vector
    //will also write the post data into a save file
    post create_post();

    //will deal with incoming data appropriately, we need to decide what we'll do with them
    void handle_inc_requests(TSN::request);
    void handle_inc_responses(TSN::response);
    void handle_inc_userinfo(TSN::user_information);

};

#endif
