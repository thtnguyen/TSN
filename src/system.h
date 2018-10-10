#ifndef SYSTEM_H
#define SYSTEM_H

#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>

class tsn_system
{
  public:
    //stores information about the user on this instance of the program
    user current_user; 

     //stores information about all other known users
    std::vector<user> all_users;

    //stores information about all users currently connected to TSN
    std::vector<user> online_users; 

    //manager object to create data publishers/subscribers
    DDSEntityManager manager;

    //constructor, takes a user object as a parameter to initialize current_user
    tsn_system(user &cu);

    //publishes current user's info on the network every 30 seconds
    void user_publisher(); 

    //creates a subscriber/reader and listens for requests
    void request_listener(); 

    //creates a subscriber/reader and listens for responses
    void response_listener(); 

    //creates a subscriber/reader and listens for user information
    void user_listener(); 

    //prompts current user for info and publishes a request on the network
    long publish_request(); 

    //takes a request as a parameter and publishes a response to that request if needed
    void publish_response(TSN::request r); 

    //loads data from the .tsn and .tsnusers file into current_user and all_users
    void load_user_data(); 

    //prompts current user for info to construct a new user object and returns it
    user create_new_user(std::string path); 

    //writes user data to a file, parameters are the user whose data we want to write, the 
    //file stream to the file to write to, and a boolean indicating if we also want to
    //write all the user's post data or not
    void write_user_data(user user_to_save, std::ofstream& out, bool write_posts); 

    //clears the online_users vector every 150 seconds to remove any users who have disconnected
    void refresh_online_list();

    //generates a request for all posts from a specific user, is invoked in view::show_user()
    //parameter is the user we want to request posts from
    void request_all_posts(user requested_user);

    //clears all information currently stored in the all_users and online_users vectors
    //and deletes the current .tsnusers file
    void resync();

    //prompts the user for a post message and constructs a new post object for the current user
    void create_post();

    //prompts the user to either change their first name, last name, or interests
    void edit_user();

};

#endif