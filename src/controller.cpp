#include <sys/time.h>
#include <thread>
#include "controller.h"

void controller::execute_cmd()
{
    viewer.print_main_menu();
    int state = -1;
    std::cout << "$ ";
    std::cin >> state;
    cin.ignore();

    if(state == 0) //exit
    {
      exit(0);
    }		
    if(state == 1) //create post
    {
      sys.create_post(); 
    }
    if(state == 2) //publish a request
    {
      //get the current epoch time
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long current_time = tp.tv_sec;
      long ret_value = 0;

      //check if any requests were made in the past 60 seconds
      if(last_request_time == 0 || current_time - last_request_time > 60)
      {
        ret_value = sys.publish_request();
        if(ret_value > 0)
          last_request_time = ret_value;
      }
      else
        std::cout << "\nYou can only publish a request once every 60 seconds." << std::endl;
    }
    if(state == 3) //list users
    {
      viewer.print_online();
    }
    if(state == 4) //show user
    {
      viewer.show_user();
    }
    if(state == 5) //edit user info
    {
      viewer.print_edit_menu();
      sys.edit_user();
    }
    if(state == 6) //resync
    {
      sys.resync();
    }
    if(state == 7) //statistics
    {
      viewer.show_stats();
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

  ROL.join();
  UL.join();
  UP.join();
  ReqL.join();
  RespL.join();
}