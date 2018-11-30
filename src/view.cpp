#include <iomanip>
#include "view.h"
#include "message.h"


void view::print_start_menu()
{
  std::cout << "\nWelcome to The Social Network! \nEnter \"help\" to see all possible commands." << std::endl;
}
void view::print_help_menu()
{
  std::cout << "\nCommands:" << std::endl << std::endl;
  std::cout << "post - publish a post to The Social Network" << std::endl;
  std::cout << "request - request a post from a user" << std::endl;
  std::cout << "list - list all users currently online" << std::endl;
  std::cout << "show - show information of a specific online user" << std::endl;
  std::cout << "edit - edit your personal information" << std::endl;
  std::cout << "resync - wipe all data and statistics" << std::endl;
  std::cout << "stats - shows how many other users are known and /% of posts on TSN that you own" << std::endl;
  std::cout << "message - send a private message to another online user" << std::endl;
  std::cout << "inbox - view your private message inbox" << std::endl;
  std::cout << "exit - exit the network" << std::endl;
}

void view::print_edit_menu()
{
  std::cout << "\n(1) First Name" << std::endl;
  std::cout << "(2) Last Name" << std::endl;
  std::cout << "(3) Interests" << std::endl;
  std::cout << "\nChoose what you'd like to change: " << std::endl;
}

void view::print_online()
{
  if(sys.online_users.size() == 0)
  {
    std::cout << "\nNo other users currently connected to TSN." << std::endl;
    return;
  }
  std::vector<user>::iterator it;
  std::cout << "==========ONLINE USERS==========" << std::endl;
  for(it = sys.online_users.begin(); it != sys.online_users.end(); it++)
  {
    std::cout << "Name : " << it->first_name << " " << it->last_name << std::endl;
    std::cout << "Interests: ";

    std::vector<std::string>::iterator interests_it;
    for(interests_it = it->interests.begin(); interests_it != it->interests.end(); interests_it++)
    {
      std::cout << "  -- " <<*interests_it << std::endl;
    }
    std::cout << std::endl;
  }
}

void view::show_user()
{
  std::vector<user>::iterator it;
  int n = 0;
  std::cout << std::endl;
  
  //listing all online users whose info can be retrieved and shown
  int on_list_size = static_cast<int> (sys.online_users.size());
  if(on_list_size > 0)
  {
    std::cout << "==========ONLINE USERS==========" << std::endl;
    for(it = sys.online_users.begin(); it != sys.online_users.end(); it++, n++)
    {
      std::cout << "(" << n << ") " << it->first_name << " " << it->last_name << std::endl;
    }
    std::cout << "\nChoose which user to show: " << std::endl;

    n = 0;
    int choice;
    std::cin >> choice;
    for(it = sys.online_users.begin(); n < choice+1 ; it++, n++)
    {
      //retrieving and printing out the chosen user's information
      if(n == choice)
        {
          std::cout << "Name: " << it->first_name << " " << it->last_name << std::endl;
          std::cout << "Interests: ";

          std::vector<std::string>::iterator interests_it;
          for(interests_it = it->interests.begin(); interests_it != it->interests.end(); interests_it++)
          {
            std::cout << "  -- " <<*interests_it << std::endl;
          }
          std::cout << "\nPosts: " << std::endl;
          if(it->get_highest_pnum() == 0)
            std::cout << "There are no posts to show." << std::endl;
          else
          {
            sys.request_all_posts(*it);
            sleep(it->get_highest_pnum());
          }
          break;
        }
    }
  }
  else
    std::cout << "There are no users online to show." << std::endl; 
}

void view::show_stats()
{
  std::vector<user>::iterator it;
  TSN::serial_number total_posts = 0;
  int total_num_users = 0; //number of known users

  //calculating number of all known posts available and all known users
  for(it = sys.all_users.begin(); it != sys.all_users.end(); it++)
  {
    total_posts += it->get_highest_pnum();
    total_num_users++;
  }

  //calculating the percentage of posts available on this node
  double content_percent = 0;
  if(sys.current_user.get_highest_pnum() > 0)
  {
    total_posts += sys.current_user.get_highest_pnum();
    content_percent = (double) sys.current_user.get_highest_pnum() / total_posts * 100;
  }
  std::cout << "\nThere are " << total_num_users << " other nodes known." << std::endl;
  std::cout << "This node contains " << fixed << std::setprecision(2) << content_percent << "\% of posts available on the TSN network." << std::endl;

}

void view::print_messages()
{
  std::vector<message>::iterator msg_it = sys.private_messages.begin();
  std::vector<user>::iterator user_it;

  std::string name = "unable to retrieve name";
  char sender_uuid[TSN::UUID_SIZE];

  for(;msg_it != sys.private_messages.end(); msg_it++)
  {
    strcpy(sender_uuid, msg_it->get_sender_uuid());
    for(user_it = sys.online_users.begin(); user_it != sys.online_users.end(); user_it++)
    {
      if(strcmp(user_it->uuid, sender_uuid) == 0)
      {
        name = user_it->first_name + " " + user_it->last_name;
        break;
      }
    }
    std::cout << "\n  === From    : " << name << " ===" << std::endl;
    std::cout << "    Message : " << msg_it->get_body() << std::endl;

  }
}