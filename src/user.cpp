#include "user.h"
#include <cstring>

user::user(std::string fname, std::string lname, long dob, char *id, std::vector<std::string>& i, std::vector<post>& p, unsigned long long hp)
{
    first_name = fname;
    last_name = lname;
    date_of_birth = dob;
    strcpy(uuid, id);
    interests = i;
    posts = p;
    highest_pnum = hp;

}

void user::add_post(post p)
{
    posts.push_back(p);
}

unsigned long long user::get_highest_pnum()
{
    return highest_pnum;
}
