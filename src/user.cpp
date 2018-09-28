#include "user.h"
#include <cstring>

user::user(std::string fname, std::string lname, long dob, char *id, std::vector<std::string>& i, std::vector<post>& p)
{
    first_name = fname;
    last_name = lname;
    date_of_birth = dob;
    strcpy(uuid, id);
    interests = i;
    posts = p;

}
