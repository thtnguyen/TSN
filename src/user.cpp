#include "user.h"
#include <cstring>
#include <vector>
#include <iostream> //debug info

user::user()
{
    first_name = "fname";
    last_name = "lname";
    date_of_birth = 0;

    std::vector<std::string> i;
    interests = i;

    std::vector<post> p;
    posts = p;

    highest_pnum = 0;
}
user::user(std::string fname, std::string lname, long dob, char *id, std::vector<std::string> &i, std::vector<post> &p, unsigned long long hp)
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
    highest_pnum++;
}

unsigned long long user::get_highest_pnum()
{
    return highest_pnum;
}

user &user::operator=(const user &rhs)
{
    first_name = rhs.first_name;
    last_name = rhs.last_name;
    date_of_birth = rhs.date_of_birth;
    strcpy(uuid, rhs.uuid);
    interests = rhs.interests;
    posts = rhs.posts;
    highest_pnum = rhs.highest_pnum;

    return *this;
}