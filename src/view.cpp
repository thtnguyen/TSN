#include <iomanip>
#include "view.h"

void view::print_main_menu()
{
  std::cout << "\nSelect what you want to do:" << std::endl << std::endl;
  std::cout << "(1) Add A Post" << std::endl;
  std::cout << "(2) Publish A Request" << std::endl;
  std::cout << "(3) List Users" << std::endl;
  std::cout << "(4) Show User" << std::endl;
  std::cout << "(5) Edit" << std::endl;
  std::cout << "(6) Resync" << std::endl;
  std::cout << "(7) Statistics" << std::endl;
  std::cout << "(0) Exit" << std::endl;
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
  for(it = sys.online_users.begin(); it != sys.online_users.end(); it++)
  {
    std::cout << "UUID  : " << it->uuid;
    std::cout << "    Name : " << it->first_name << " " << it->last_name << std::endl;
  }
}

void view::show_user()
{
  std::vector<user>::iterator it;
  int n = 0;
  std::cout << std::endl;
  
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
  for(it = sys.all_users.begin(); it != sys.all_users.end(); it++)
  {
    total_posts += it->get_highest_pnum();
    total_num_users++;
  }
  double content_percent = 0;
  if(sys.current_user.get_highest_pnum() > 0)
  {
    total_posts += sys.current_user.get_highest_pnum();
    content_percent = (double) sys.current_user.get_highest_pnum() / total_posts * 100;
  }
  std::cout << "\nThere are " << total_num_users << " other nodes known." << std::endl;
  std::cout << "This node contains " << fixed << std::setprecision(2) << content_percent << "\% of posts available on the TSN network." << std::endl;

}