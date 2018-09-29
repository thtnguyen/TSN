#include <thread>
#include <sys/time.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "io.h"

int state;

void printMenu()
{

  //user this_user = load_user_data(".tsn");

  std::cout << "\nSelect what you want to do:" << std::endl << std::endl;
  std::cout << "(1) Add A Post" << std::endl;
  std::cout << "(2) Publish A Request" << std::endl;
  std::cout << "(3) List Users" << std::endl;
  std::cout << "(4) Show User" << std::endl;
  std::cout << "(5) Edit" << std::endl;
  std::cout << "(6) Resync" << std::endl;
  std::cout << "(0) Exit" << std::endl;
}

//create and add post
void createPost(user &current_user)
{
  (current_user).highest_pnum++;
  TSN::serial_number sn = (TSN::serial_number) current_user.get_highest_pnum();

  std::string message;
  std::cout << "Enter a message for your post: " << std::endl;
  getline(cin, message);

  //getting epoch time in seconds
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long date = tp.tv_sec;

  post p = post(sn, message, date);

  current_user.add_post(p);

  //implement writing post information to .tsn file here

  state = -1;
}

void print_online(std::vector<user>& on)
{
  std::vector<user>::iterator it;
  for(it = on.begin(); it != on.end(); it++)
  {
    std::cout << "UUID  : " << it->uuid;
    std::cout << "    Name : " << it->first_name << " " << it->last_name << std::endl;
  }
  state = -1;
}

void menu(user &current_user, std::vector<user>& on)
{
  state = -1;
  while(state != 0)
  {
    printMenu();
    std::cout << "$ ";
    std::cin >> state;
    std::cout << "choice was " << state << std::endl;
    cin.ignore();

    if(state == 0)
    {
      exit(0);
    }		
    if(state == 1)
    {
      createPost(current_user);
    }
    if(state == 2)
    {
      publishRequest(current_user);
    }
    if(state == 3)
    {
      print_online(on);
    }
    
  }
} 

int main (int argc, char* argv[])
{	
  std::cout << "Welcome to The Social Network." << std::endl;

  user current_user = load_user_data(".tsn");

  
  /*
  test if load_user_data workd properly

  std::cout << current_user.uuid << std::endl;
  std::cout << current_user.first_name << std::endl;
  std::cout << current_user.last_name << std::endl;
  std::cout << current_user.date_of_birth << std::endl;
  std::cout << current_user.highest_pnum << "\n" << std::endl;
  
  std::vector<post>::iterator it;
  for(it = current_user.posts.begin(); it != current_user.posts.end(); it++)
  {
    std::cout << it->get_sn() << std::endl;
    std::cout << it->get_doc() << std::endl;
    std::cout << it->get_body() << "\n" << std::endl; 
  }

  exit(0);*/

  std::vector<user> online_users;
  std::thread UP (user_publisher, std::ref(current_user));  
  std::thread UL (user_listener, std::ref(current_user), std::ref(online_users));
  std::thread ROL (refresh_online_list, std::ref(online_users));
  std::thread ReqL(request_listener, std::ref(current_user));
  std::thread RespL (response_listener, std::ref(current_user));

  menu(current_user, online_users);

  ROL.join();
  UL.join();
  UP.join();
  ReqL.join();
  RespL.join();

  exit(0);

  return 0;
}