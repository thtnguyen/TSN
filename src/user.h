#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "ccpp_tsn.h"
#include "post.h"

class user
{
  private:
    unsigned long long highest_pnum;
    
  public:
    std::string first_name;
    std::string last_name;
    long date_of_birth;
    char uuid[TSN::UUID_SIZE];
    std::vector<std::string> interests;
    std::vector<post> posts;
    
    user(); //default constructor
    user(std::string fname, std::string lname, long dob, char *id, std::vector<std::string>& i,
        std::vector<post>& p, unsigned long long hp);

    unsigned long long get_highest_pnum();
    void add_post(post p);
    user& operator= (const user &rhs);
};

#endif
