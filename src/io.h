#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>

void background(user &current_user);
void publishRequest(user &current_user);
void publishUserInfo(user &current_user);
void publishResponse(user &current_user, TSN::request r);