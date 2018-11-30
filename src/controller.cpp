#include <sys/time.h>
#include <thread>
#include "controller.h"

void controller::execute_cmd()
{
    string state = " ";
    std::cout << ">> ";
    std::cin >> state;
    cin.ignore();

    if(state == "exit") //exit
    {
      exit(0);
    }		
    if(state == "post") //create post
    {
      sys.create_post(); 
    }
    if(state == "request") //publish a request
    {
      //get the current epoch time
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long current_time = tp.tv_sec;
      long ret_value = 0;

      //check if any requests were made in the past 60 seconds
      if(last_request_time == 0 || current_time - last_request_time > 1)
      {
        ret_value = sys.publish_request();
        assert(ret_value > 0 || ret_value == -1);
        
        if(ret_value > 0)
          last_request_time = ret_value;
      }
      else
        std::cout << "\nYou can only publish a request once every 60 seconds." << std::endl;
    }
    if(state == "list") //list users
    {
      viewer.print_online();
    }
    if(state == "show") //show user
    {
      viewer.show_user();
    }
    if(state == "edit") //edit user info
    {
      viewer.print_edit_menu();
      sys.edit_user();
    }
    if(state == "resync") //resync
    {
      sys.resync();
    }
    if(state == "stats") //statistics
    {
      viewer.show_stats();
    }
    if(state == "message")
    {
      sys.publish_message();
    }
    if(state == "inbox")
    {
      viewer.print_messages();
    }
    if(state == "reply")
    {
      sys.create_reply(sys.recent_post, sys.recent_uuid);
    }
    if(state == "match")
    {
      std::string::iterator str_it;
      std::string match;
      std::cout << "Enter which interest you'd like to match posts to: ";
      getline(cin, match);

      for(str_it = match.begin(); str_it != match.end(); str_it++)
        *str_it = tolower(*str_it);
      
      sys.choice = match;
    }
    if(state == "help")
    {
      viewer.print_help_menu();
    }
}
void controller::background()
{ 
  //all listener methods, user publisher, and online list refresher runs in the background
  std::thread UP (&tsn_system::user_publisher, &sys);  
  std::thread UL (&tsn_system::user_listener, &sys);
  std::thread ROL (&tsn_system::refresh_online_list, &sys);
  std::thread ReqL(&tsn_system::request_listener, &sys);
  std::thread RespL (&tsn_system::response_listener, &sys);
  std::thread MessL(&tsn_system::message_listener, &sys);

  ROL.join();
  UL.join();
  UP.join();
  ReqL.join();
  RespL.join();
  MessL.join();
}