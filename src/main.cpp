#include <iostream>
#include <string>
#include <fstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

int main (int argc, char* argv[])
{
  std::cout << "Welcome to The Social Network." << std::endl;
  user this_user = load_user_data(".tsn");

  //code to publish startup info goes here

  //? 
  DDSEntityManager mgr;
  mgr.createParticipant("TSN");

  mgr.createSubscriber();
  mgr.createPublisher();

  //TBC
}

user load_user_data(std::string filename)
{
  std::string home = getenv("HOME"); //.tsn is always stored in home directory 
  ifstream in(home + "/" + filename );
  if(!in)
  {
    std::cout << "No saved user data was found. Please enter your information." << std::endl;
  }

  //code to parse through the file goes here
}
