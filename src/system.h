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
    user current_user;
    std::vector<user> all_users;
    std::vector<user> online_users;

    tsn_system(user &cu);
    void user_publisher();
    void request_listener();
    void response_listener();
    void user_listener();
    long publishRequest();
    void publishUserInfo();
    void publishResponse(TSN::request r);
    void load_user_data();
    user create_new_user(std::string path);
    void write_user_data(user user_to_save, std::ofstream& out, bool write_posts);
    void refresh_online_list();
    void request_all_posts(user requested_user);
    void resync(); //from main
    void create_post(); //main
    void edit_user();

};

#endif