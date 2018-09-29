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

  std::cout << "Select what you want to do:" << std::endl << std::endl;
  std::cout << "(1) Recieve Messages - TEMPORARILY DISABLED" << std::endl;
  std::cout << "(2) Add A Post" << std::endl;
  std::cout << "(3) Publish A Request" << std::endl;
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
  printMenu();
}

//will reimplement retrieving responses with new system
void viewMessages()
{
  std::cout << "=== RECIEVE MESSAGE MODE" << endl;
  std::cout << "Type 0 to exit" << endl;

  while(state == 1)
  {
    std::cin >> state;
    std::cin.ignore();
  }

  state = -1;	
  printMenu();
}

void menu(user &current_user)
{
  printMenu();
  state = -1;

  while(state != 0)
  {
    std::cin >> state;
    std::cin.ignore();
    if(state == 0)
    {
      break;
    }		
    if(state == 1)
    {
      viewMessages();
    }
    if(state == 2)
    {
      createPost(current_user);
    }
    if(state == 3)
    {
      publishRequest(current_user);
    }
    
  }
} 

int main (int argc, char* argv[])
{	
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  string uuidstring = boost::uuids::to_string(uuid);
  char myuuid[TSN::UUID_SIZE] = {};
  strcpy(myuuid, uuidstring.c_str());

  std::cout << "Welcome to The Social Network." << std::endl;
  std::cout << "Your UUID is: " << uuid << std::endl;

  std::cout << "Enter your first name: ";
  std::string first_name;
  std::cin >> first_name;
  std::cin.ignore();

  std::cout << "Enter your last name: ";
  std::string last_name;
  std::cin >> last_name;
  std::cin.ignore();

  std::vector<std::string> string_v;
  std::vector<post> post_v;
  user current_user = user(first_name, last_name, 100, myuuid, string_v, post_v, 0);

  publishUserInfo(current_user);

  std::thread BG (background, std::ref(current_user));

  menu(current_user);

  exit(0);
  BG.join();

  return 0;
}
/*user load_user_data(std::string filename)
{
  std::string home = getenv("HOME"); //.tsn is always stored in home directory 
  ifstream in(home + "/" + filename );
  if(!in)
  {
    std::cout << "No saved user data was found. Please enter your information." << std::endl;
  }

  //code to parse through the file goes here
}*/
