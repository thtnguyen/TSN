#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>

void user_publisher(user &current_user);
void request_listener(user &current_uesr);
void response_listener(user &current_user);
void user_listener(user &current_user, std::vector<user>& online_users);
long publishRequest(user &current_user);
void publishUserInfo(user &current_user);
void publishResponse(user &current_user, TSN::request r);
user load_user_data(std::string filename);
user create_new_user(std::string path);
void write_user_data(user user_to_save, std::string file);
void refresh_online_list(std::vector<user>& on);
void request_all_posts(user &current_user, user requested_user);