#include <sys/time.h>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <thread>
#include <vector>
#include "user.h"
#include <string>
#include <iostream>
#include "system.h"

tsn_system::tsn_system(user& cu)
{
   current_user = cu;
   std::vector<user> on_vector;
   online_users = on_vector;
   std::vector<message> pm_vector;
   private_messages = pm_vector;

   manager.createParticipant("TSN");

   std::vector<user> all_vector;
   all_users = all_vector;
}

void tsn_system::user_publisher()
{
  //initializing data publisher and writer

  TSN::user_informationTypeSupport_var mt = new TSN::user_informationTypeSupport();
  manager.registerType(mt.in());

  char userinfo_topic[] = "user_information";
  manager.createTopic(userinfo_topic);

  manager.createPublisher();
  manager.createWriter(false);


  DDS::DataWriter_var dw = manager.getWriter();
  TSN::user_informationDataWriter_var userinfoWriter = TSN::user_informationDataWriter::_narrow(dw.in());
  
 while(true)
  {
    TSN::user_information userinfoInstance;


    //initialize a user_information instance with info from current_user
    userinfoInstance.first_name = DDS::string_dup((current_user.first_name).c_str());
    userinfoInstance.last_name = DDS::string_dup((current_user.last_name).c_str());
    strcpy(userinfoInstance.uuid, current_user.uuid);
    userinfoInstance.number_of_highest_post = current_user.get_highest_pnum();
    userinfoInstance.date_of_birth = current_user.date_of_birth;

    //get length of the interests vector so we can set the sequence length of user_information
    int n = 0;
    std::vector<std::string>::iterator it;
    for(it = current_user.interests.begin(); it != current_user.interests.end(); it++)
    {
      n++;
    }
    userinfoInstance.interests.length(n);

    //store current user's interests in the sequence
    n = 0;
    for(it = current_user.interests.begin(); it != current_user.interests.end(); it++, n++)
    {
      userinfoInstance.interests[n] = DDS::string_dup(it->c_str());
    }

    ReturnCode_t status = userinfoWriter->write(userinfoInstance, DDS::HANDLE_NIL);
    checkStatus(status, "user_informationDataWriter::write");

    sleep(30);
  }

}
void tsn_system::request_listener()
{
  //initializing the data readers and subscribers
  TSN::requestSeq requestList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager req_mgr;
  req_mgr.createParticipant("TSN");

  TSN::requestTypeSupport_var reqts = new TSN::requestTypeSupport();
  req_mgr.registerType(reqts.in());

  char req_topic[] = "request";
  req_mgr.createTopic(req_topic);

  req_mgr.createSubscriber();
  req_mgr.createReader();

  DDS::DataReader_var req_data = req_mgr.getReader();
  TSN::requestDataReader_var requestReader = TSN::requestDataReader::_narrow(req_data.in());
  checkHandle(requestReader.in(), "requestDataReader::_narrow");

  ReturnCode_t request_status = -1;
  
  //while loop constantly listens for any requests sent over the network
  while(true)
  {
    request_status = requestReader->take(requestList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(request_status, "requestInformationDataReader::take");

    if(requestList.length() > 0)
    {
      for (DDS::ULong j = 0; j < requestList.length(); j++)
      {
        for(DDS::ULong i = 0; i < requestList[j].user_requests.length(); i++)
          //if one of the node requests is meant for the current user, then publish a response to it
          if(strcmp(requestList[j].user_requests[i].fulfiller_uuid, current_user.uuid) == 0)
          {
            publish_response(requestList[j]);
          }
      }  
    } 
    request_status = requestReader->return_loan(requestList, infoSeq);
    checkStatus(request_status, "requestDataReader::return_loan");
    sleep(1);
  }

  req_mgr.deleteReader();
  req_mgr.deleteSubscriber();
  req_mgr.deleteTopic();
  req_mgr.deleteParticipant();
}

void tsn_system::response_listener()
{
  //initializing data readers and subscribers
  TSN::responseSeq responseList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager resp_mgr;
  resp_mgr.createParticipant("TSN");

  TSN::responseTypeSupport_var respts = new TSN::responseTypeSupport();
  resp_mgr.registerType(respts.in());

  char resp_topic[] = "response";

  resp_mgr.createTopic(resp_topic);
  resp_mgr.createSubscriber();
  resp_mgr.createReader();

  DDS::DataReader_var resp_data =  resp_mgr.getReader();
  TSN::responseDataReader_var responseReader = TSN::responseDataReader::_narrow(resp_data.in());
  checkHandle(responseReader.in(), "responseDataReader::_narrow");

  ReturnCode_t response_status = -1;

  //this is to be used to see if a post is posted by user interest
  int printed_post = 0;
  
  //while loop constantly listens for any responses sent over the network
  while(true)
  {
    response_status = responseReader->take(responseList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(response_status, "responseDataReader::take");
    
    for (DDS::ULong j = 0; j < responseList.length(); j++)
     {
       if(choice == "yes")
       {
         //ignore the response if it's sent from the current user
          if((strcmp(responseList[j].uuid, current_user.uuid) != 0) && responseList[j].post_id != 0)
          {
            //retrieving the corresponding name to the responder's uuid; name is initialized in case the
            //online list was refreshed and the responding user's info hasn't been re-published yet
            std::vector<user>::iterator it;
            std::string name = "unable to retrieve name";
            for(it = online_users.begin(); it != online_users.end(); it++)
            {
              if(strcmp(it->uuid, responseList[j].uuid) == 0)
              {
                name = it->first_name + " " + it->last_name;
                break;
              }
            }
         //this will get us our interest post
         string curr_interests = "nothing";
         string curr_post = DDS::string_dup((responseList[j].post_body));
         int interest_exists = 0;
         unsigned k;
         for(k = 0; k < current_user.interests.size(); k++)
         {
           curr_interests = current_user.interests[k];
           interest_exists = curr_post.find(curr_interests);
           if(interest_exists > 0)
            {
              std::cout << "\n    Name  : " << name << std::endl;
              std::cout << "    Post ID : " << responseList[j].post_id << std::endl;
              std::cout << "    Date of Creation: " << responseList[j].date_of_creation << std::endl;
              std::cout << "    Post Body: " << responseList[j].post_body << std::endl;
              std::cout << "    Sorted by your interest of: " << curr_interests << std::endl;
              printed_post++;
            }
          }
        }
       }else{
          //ignore the response if it's sent from the current user
          if((strcmp(responseList[j].uuid, current_user.uuid) != 0) && responseList[j].post_id != 0)
          {
            //retrieving the corresponding name to the responder's uuid; name is initialized in case the
            //online list was refreshed and the responding user's info hasn't been re-published yet
            std::vector<user>::iterator it;
            std::string name = "unable to retrieve name";
            for(it = online_users.begin(); it != online_users.end(); it++)
            {
              if(strcmp(it->uuid, responseList[j].uuid) == 0)
              {
                name = it->first_name + " " + it->last_name;
                break;
              }
            }
              std::cout << "\n    Name  : " << name << std::endl;
              std::cout << "    Post ID : " << responseList[j].post_id << std::endl;
              std::cout << "    Date of Creation: " << responseList[j].date_of_creation << std::endl;
              std::cout << "    Post Body: " << responseList[j].post_body << std::endl;
          }
       }
		 }
    response_status = responseReader->return_loan(responseList, infoSeq);
    checkStatus(response_status, "response_informationDataReader::return_loan");
    sleep(1);
  }

  resp_mgr.deleteReader();
  resp_mgr.deleteTopic();
  resp_mgr.deleteParticipant();
  resp_mgr.deleteSubscriber();

}

void tsn_system::user_listener()
{
  //initializing the data readers and subscribers
  TSN::user_informationSeq userinfoList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager userinfo_mgr;
  userinfo_mgr.createParticipant("TSN");

  TSN::user_informationTypeSupport_var uits = new TSN::user_informationTypeSupport();
  userinfo_mgr.registerType(uits.in());

  char user_topic[] = "user_information";
	
  userinfo_mgr.createTopic(user_topic);
  userinfo_mgr.createSubscriber();
  userinfo_mgr.createReader();

  DDS::DataReader_var user_data = userinfo_mgr.getReader();
  TSN::user_informationDataReader_var userReader = TSN::user_informationDataReader::_narrow(user_data.in());
  checkHandle(userReader.in(), "user_informationDataReader::_narrow");

  ReturnCode_t userinfo_status = -1;
  
  //while loop constantly listens for any user information sent over the network
  while(1)
  {
    userinfo_status = userReader->take(userinfoList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(userinfo_status, "user_informationDataReader::take");
    
     for (DDS::ULong j = 0; j < userinfoList.length(); j++)
     {
       if(strcmp(userinfoList[j].uuid, current_user.uuid) != 0)
       {
         char uuid[TSN::UUID_SIZE];
         strcpy(uuid, userinfoList[j].uuid);

         std::vector<std::string> interests;
         std::string interest;
         for(unsigned int i = 0; i < userinfoList[j].interests.length(); i++)
         {
           interest = DDS::string_dup(userinfoList[j].interests[i]);
           interests.push_back(interest);
         }
         std::vector<post> posts;

         string fname = DDS::string_dup(userinfoList[j].first_name);
         string lname = DDS::string_dup(userinfoList[j].last_name);
         long date = userinfoList[j].date_of_birth;
         unsigned long long hp = userinfoList[j].number_of_highest_post;

         user new_user = user(fname, lname, date, uuid, interests, posts, hp);
         
         //if user is already known, delete the old record in vector and add the new one
         std::vector<user>::iterator it;
         for(it = online_users.begin(); it != online_users.end(); it++)
         {
           if(strcmp(it->uuid, new_user.uuid) == 0)
           {
             online_users.erase(it);
             break;
           }
         }
         online_users.push_back(new_user);

         for(it = all_users.begin(); it != all_users.end(); it++)
         {
           if(strcmp(it->uuid, new_user.uuid) == 0)
           {
             all_users.erase(it);
             break;
           }
         }
         all_users.push_back(new_user);
         std::string home = getenv("HOME"); //.tsn is always stored in home directory
         std::string path = home + "/.tsnusers1";
         std::ofstream out (path);
         for(it = all_users.begin(); it != all_users.end(); it++)
         {
           write_user_data(*it, out, false);
         }
         out.close();

       }
		}
    userinfo_status = userReader->return_loan(userinfoList, infoSeq);
    checkStatus(userinfo_status, "user_informationDataReader::return_loan");
    sleep(1);	
  }
  //clean up
  userinfo_mgr.deleteReader();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();
  userinfo_mgr.deleteSubscriber();
}


long tsn_system::publish_request()
{
  //We are going to get a choice from the user to see if they want to see interest
  std::cout << "Do you want to sort by interests (yes or no)" << std::endl;
  getline(cin, choice);

  //initializing data publisher and writer
  DDSEntityManager request_mgr;

  request_mgr.createParticipant("TSN");
  TSN::requestTypeSupport_var mt = new TSN::requestTypeSupport();
  request_mgr.registerType(mt.in());

  char request_topic[] = "request";
  request_mgr.createTopic(request_topic);

  request_mgr.createPublisher();
  request_mgr.createWriter(false);

  DDS::DataWriter_var dw = request_mgr.getWriter();
  TSN::requestDataWriter_var requestWriter = TSN::requestDataWriter::_narrow(dw.in());
  TSN::request requestInstance;
  std::vector<TSN::node_request> requests;

  int post_seq_length = 0;
  int n;

  //getting information for the individual node requests from the user
  while(true)
  {
    TSN::node_request nodeReqInstance;
    std::vector<TSN::serial_number> requested_p;
    
    char myuuid[TSN::UUID_SIZE] = {};

    
    n = 0;
    std::cout << std::endl;
    
    std::vector<user>::iterator user_it;
    TSN::serial_number requested_hpnum;
    int on_list_size = static_cast<int> (online_users.size());

    //prints out a list of online users that can be sent a request to
    if(on_list_size > 0)
    {
      std::cout << "==========ONLINE USERS==========" << std::endl;
      for(user_it = online_users.begin(); user_it != online_users.end(); user_it++, n++)
      {
        std::cout << "(" << n << ") " << user_it->first_name << " " << user_it->last_name << std::endl;
      }
      std::cout << "\nChoose which user to request from: " << std::endl;

      n = 0;
      int choice;
      std::cin >> choice;

      //retrieving the UUID and highest post number of chosen user
      for(user_it = online_users.begin(); n < choice+1 ; user_it++, n++)
      {
        if(n == choice)
          {
            strcpy(myuuid, user_it->uuid);
            requested_hpnum = user_it->get_highest_pnum();
          }
      }
    }
    else
    {
      std::cout << "\nThere are no users online to publish a request to." << std::endl;
      return -1;
    }

    if(requested_hpnum == 0)
    {
      std::cout << "This user has no posts to request for." << std::endl;
      return -1;
    }

    //getting serial numbers of desired posts from that user
    std::cout << "Enter the serial numbers of the posts you want from that user on a separate line, enter 0 to stop:" << std::endl;
    TSN::serial_number get_input;
    while(true)
    {
      std::cin >> get_input;
      if(get_input == 0)
      {
        break;
      }
      else
      {
        if(get_input > requested_hpnum)
        {
          std::cout << "The user doesn't have a post with that serial number. Please re-enter." << std::endl;
        }
        else
          requested_p.push_back(get_input);
      }
    }
    
    strcpy(nodeReqInstance.fulfiller_uuid, myuuid);
    post_seq_length = static_cast<int> (requested_p.size());
    nodeReqInstance.requested_posts.length(post_seq_length);

    n = 0;
    std::vector<TSN::serial_number>::iterator sn_it;
    for(sn_it = requested_p.begin(); sn_it != requested_p.end(); sn_it++, n++)
    {
      nodeReqInstance.requested_posts[n] = *sn_it;
    }
    requests.push_back(nodeReqInstance);

    int choice;
    std::cout << "Would you like to request from another user? Enter 1 for Yes or 0 for No: ";
    std::cin >> choice;

    //adding all the individual node requests into one request instance
    if(choice == 0)
    {
      int req_seq_length = static_cast<int> (requests.size());
      requestInstance.user_requests.length(req_seq_length);

      int n = 0;
      std::vector<TSN::node_request>::iterator it;
      for(it = requests.begin(); it != requests.end(); it++, n++)
        requestInstance.user_requests[n] = *it;

      break;
    }
  }  

  strcpy(requestInstance.uuid, current_user.uuid);
  std::cout << "\n=== [Publisher] publishing your request on the TSN network :";
 
  ReturnCode_t status = requestWriter->write(requestInstance, DDS::HANDLE_NIL);
  checkStatus(status, "requestDataWriter::write");
  std::cout << " success" << std::endl;

  struct timeval tp;
  gettimeofday(&tp, NULL);
  long date = tp.tv_sec;

  //clean up
  request_mgr.deleteWriter();
  request_mgr.deletePublisher();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();

  sleep(n);

  //we return the time the request is published to adhere to the 1 request per minute rule
  return date; 
}

void tsn_system::publish_response(TSN::request r)
{

  //initializing data publisher and writer
  DDSEntityManager response_mgr;
  response_mgr.createParticipant("TSN");

  TSN::responseTypeSupport_var mt = new TSN::responseTypeSupport();
  response_mgr.registerType(mt.in());

  char response_topic[] = "response";
  response_mgr.createTopic(response_topic);

  response_mgr.createPublisher();
  response_mgr.createWriter(false);

  DDS::DataWriter_var dw = response_mgr.getWriter();
  TSN::responseDataWriter_var responseWriter = TSN::responseDataWriter::_narrow(dw.in());

  TSN::node_request my_node_req;

  for (DDS::ULong j = 0; j < r.user_requests.length(); j++)
  {
    //finding the node request that is meant for the current user
    if(strcmp(r.user_requests[j].fulfiller_uuid, current_user.uuid) == 0)
      my_node_req = r.user_requests[j];
  
    //creating a response for each post serial number within the node request
    for(DDS::ULong j = 0; j < my_node_req.requested_posts.length(); j++)
    {
      TSN::response responseInstance;

      std::string body = "**no post for this serial number was found**";
      TSN::serial_number serial_num = my_node_req.requested_posts[j];
      long doc = 0;

      std::vector<post>::iterator it;
      for(it = current_user.posts.begin(); it != current_user.posts.end(); it++)
      {
        if(my_node_req.requested_posts[j] == it->get_sn())
        {
          body = it->get_body();
          serial_num = it->get_sn();
          doc = it->get_doc();
          break;
        }
      }
      strcpy(responseInstance.uuid, current_user.uuid);
      responseInstance.post_id = serial_num;
      responseInstance.date_of_creation = doc;
      responseInstance.post_body = DDS::string_dup(body.c_str());

      ReturnCode_t status = responseWriter->write(responseInstance, DDS::HANDLE_NIL);
      checkStatus(status, "responseDataWriter::write");
      sleep(2);

    }
  }
  response_mgr.deleteWriter();
  response_mgr.deletePublisher();
  response_mgr.deleteTopic();
  response_mgr.deleteParticipant();
}

void tsn_system::load_user_data()
{
  std::string home = getenv("HOME"); //.tsn and .tsnusers is always stored in home directory
  std::string path = home + "/.tsnusers1";
  std::ifstream in(path);

  std::string first_name;
  std::string last_name;
  char myuuid[TSN::UUID_SIZE] = {};
  std::vector<post> posts;
  std::vector<std::string> interests;

  std::string temp; 

  //loading info about known nodes from .tsnusers if it exsits
  if(in)
  {
    while(getline(in, temp)) 
    {
      strcpy(myuuid, temp.c_str());
      in >> first_name;
      in >> last_name;

      long date;
      in >> temp;
      stringstream ss (temp);
      ss >> date;

      unsigned long long highest_pnum;
      in >> temp;
      ss.str(temp);
      ss.clear();
      ss >> highest_pnum;
      
      while(getline(in, temp))
      {
        if(temp == "END INTERESTS")
          break;
            
        interests.push_back(temp);    
      }

      all_users.push_back(user(first_name, last_name, date, myuuid, interests, posts, highest_pnum));
    }
  }
  in.close();
  
  //now attempting to load info of the current user/node from .tsn
  path = home + "/.tsn1";
  in.open(path);

  //the .tsn file does not exist so obtain data directly from user
  if(!in)
  {
    current_user = create_new_user(path);
    return;
  }

  //the file exists so parse through the file and get the info
  interests.clear();
  std::string uuidstring;
  in >> uuidstring;
  strcpy(myuuid, uuidstring.c_str());

  in >> first_name;
  in >> last_name;

  long date;
  in >> temp;
  stringstream ss (temp);
  ss >> date;

  unsigned long long highest_pnum;
  in >> temp;
  ss.str(temp);
  ss.clear();
  ss >> highest_pnum;
  
  //retrieving current user's interests
  while(getline(in, temp))
  {
    if(temp == "END INTERESTS")
      break;
				
    interests.push_back(temp);    
  }

  long doc;
  TSN::serial_number sn;
  std::string body;

  //retrieving current user's posts
  while(getline(in, temp))
  {
    if(temp == "END POSTS")
      break;
    
    //read serial number
    ss.str(temp);
    ss.clear();
    ss >> sn;

    //read post body
    getline(in, temp);
    body = temp;

    //read date of creation
    getline(in, temp);
    ss.str(temp);
    ss.clear();
    ss >> doc;

    char id[TSN::UUID_SIZE] = "000000000000000000000000000000000000";
    posts.push_back(post(sn, body, doc, false, -1, id));
  }
  in.close();

  //initializing current_user with a user object with all the data from .tsn
  current_user = user(first_name, last_name, date, myuuid, interests, posts, highest_pnum);
}

user tsn_system::create_new_user(std::string path)
{
  std::string first_name;
  std::string last_name;
  std::vector<post> posts;
  std::vector<std::string> interests;

  std::ofstream out(path);

  //generating a boost UUID
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  string uuidstring = boost::uuids::to_string(uuid);
  char myuuid[TSN::UUID_SIZE] = {};
  strcpy(myuuid, uuidstring.c_str());
  out << uuidstring << std::endl;

  std::cout << "No saved user data was found. Please enter your information." << std::endl;

  std::cout << "Enter your first name: ";
  std::cin >> first_name;
  out << first_name << std::endl;

  std::cout << "Enter your last name: ";
  std::cin >> last_name;
  out << last_name << std::endl;

  struct timeval tp;
  gettimeofday(&tp, NULL);
  long date = tp.tv_sec;
  out << date << std::endl;

  out << "0"; //highest post number is 0
  string interest;
  std::cout << "Enter your interests, entering a newline after each interest (type 0 to stop): " << std::endl;
  while(true)
  {
    getline(cin, interest);
    if(interest == "0")
    {
      break;
    }
    out << interest << std::endl;
    interests.push_back(interest);
  }
  out << "END INTERESTS" << std::endl;
  out.close();
  return user(first_name, last_name, date, myuuid, interests, posts, 0);
}

void tsn_system::refresh_online_list()
{
 while(true)
  {
    online_users.clear();
    sleep(150);
  }
}

void tsn_system::request_all_posts(user requested_user)
{
  //initializing data publisher and writer
  DDSEntityManager request_mgr;

  request_mgr.createParticipant("TSN");
  TSN::requestTypeSupport_var mt = new TSN::requestTypeSupport();
  request_mgr.registerType(mt.in());

  char request_topic[] = "request";
  request_mgr.createTopic(request_topic);

  request_mgr.createPublisher();
  request_mgr.createWriter(false);

  DDS::DataWriter_var dw = request_mgr.getWriter();
  TSN::requestDataWriter_var requestWriter = TSN::requestDataWriter::_narrow(dw.in());

  //creating a request for the specified user
  TSN::request requestInstance;
  TSN::node_request nodeReqInstance;
  strcpy(nodeReqInstance.fulfiller_uuid, requested_user.uuid);

  nodeReqInstance.requested_posts.length(requested_user.get_highest_pnum());
  std::vector<TSN::serial_number> requested_p;

  //requesting for post serial numbers 1 to highest post number
  for(unsigned long long n = 0; n < requested_user.get_highest_pnum(); n++)
  {
    nodeReqInstance.requested_posts[n] = n+1;
  }
  strcpy(requestInstance.uuid, current_user.uuid);
  requestInstance.user_requests.length(1);
  requestInstance.user_requests[0] = nodeReqInstance;

  ReturnCode_t status = requestWriter->write(requestInstance, DDS::HANDLE_NIL);
  checkStatus(status, "requestDataWriter::write");

  request_mgr.deleteWriter();
  request_mgr.deletePublisher();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();
}

void tsn_system::write_user_data(user user_to_save, std::ofstream& out, bool write_posts)
{
  //writing personal information of the specified user to the output file stream
  out << user_to_save.uuid << std::endl;
  out << user_to_save.first_name << std::endl;
  out << user_to_save.last_name << std::endl;
  out << user_to_save.date_of_birth << std::endl;
  out << user_to_save.get_highest_pnum();

  std::vector<std::string>::iterator it;
  for(it = user_to_save.interests.begin(); it != user_to_save.interests.end(); it++)
  {
    out << *it << std::endl;
  }
  out << "END INTERESTS" << std::endl;

  //if we also want the user's posts to be written then do so
  if(write_posts)
  {
    std::vector<post>::iterator posts_it;
    for(posts_it = user_to_save.posts.begin(); posts_it != user_to_save.posts.end(); posts_it++)
    {
      out << posts_it->get_sn() << std::endl;
      out << posts_it->get_body() << std::endl;
      out << posts_it->get_doc() << std::endl;
    }
    out << "END POSTS" << std::endl;
  }
}

void tsn_system::resync()
{
  std::string home = getenv("HOME");
  std::string path = home + "/.tsnusers1";
  std::remove(path.c_str());

  online_users.clear();
  all_users.clear();

  std::cout << "\nData about all other known and online nodes have been wiped." << std::endl;
}

void tsn_system::create_post()
{
  //calculating the new post serial number 
  TSN::serial_number sn = (TSN::serial_number) current_user.get_highest_pnum()+1;

  std::string message;
  std::cout << "Enter a message for your post: " << std::endl;
  getline(cin, message);

  //getting epoch time in seconds
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long date = tp.tv_sec;

  //saving the post in the current_user object
  char id[TSN::UUID_SIZE] = "000000000000000000000000000000000000";
  post p = post(sn, message, date, false, -1, id);
  current_user.add_post(p);

  //writing the new post to .tsn file
  std::string home = getenv("HOME");
  std::string path = home + "/.tsn1";
  std::ofstream out (path);

  write_user_data(current_user, out, true);
  out.close();

}

void tsn_system::edit_user()
{
  int choice = 0;
  cin >> choice;
  
  if(choice == 1)
  {
    std::string name;
    std::cout << "Enter your new first name: ";
    cin >> name;
    current_user.first_name = name;
  }
  if(choice == 2)
  {
    std::string name;
    std::cout << "Enter your new last name: ";
    cin >> name;
    current_user.last_name = name;
  }
  if(choice == 3)
  {
    //existing interests are wiped and replaced with new interests
    current_user.interests.clear();
    string interest;
    std::cout << "Enter your new interests to replace your existing interests, entering a newline after each interest (type 0 to stop): " << std::endl;
    while(true)
    {
      getline(cin, interest);
      if(interest == "0")
      {
        break;
      }
      current_user.interests.push_back(interest);
    }
  }

  //writing edited information to .tsn file
  std::string home = getenv("HOME");
  std::string path = home + "/.tsn1";
  std::ofstream out (path);

  write_user_data(current_user, out, true);
  out.close();
}

void tsn_system::message_listener(){
  //initializing data readers and subscribers
  TSN::private_messageSeq messageList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager msg_mgr;
  msg_mgr.createParticipant("TSN");

  TSN::private_messageTypeSupport_var msgts = new TSN::private_messageTypeSupport();
  msg_mgr.registerType(msgts.in());

  char msg_topic[] = "message";

  msg_mgr.createTopic(msg_topic);
  msg_mgr.createSubscriber();
  msg_mgr.createReader();

  DDS::DataReader_var msg_data =  msg_mgr.getReader();
  TSN::private_messageDataReader_var messageReader = TSN::private_messageDataReader::_narrow(msg_data.in());
  checkHandle(messageReader.in(), "private_messageDataReader::_narrow");

  ReturnCode_t message_status = -1;

  //waiting for messages
  while(true)
  {
    message_status = messageReader->take(messageList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(message_status, "private_messageDataReader::take");
    
    for (DDS::ULong j = 0; j < messageList.length(); j++)
     {
       //ignore the message if it's sent from the current user, save message if it's meant for current user
       if((strcmp(messageList[j].sender_uuid, current_user.uuid) != 0) 
       && strcmp(messageList[j].receiver_uuid, current_user.uuid)==0 )
       {
         //variables to construct new message object
         char sender_uuid[TSN::UUID_SIZE];
         strcpy(sender_uuid, messageList[j].sender_uuid);
         std::string msg = DDS::string_dup(messageList[j].message_body);
         std::cout << "in message_listener, msg: " << msg << std::endl;
         long doc = messageList[j].date_of_creation;

         std::cout << "You have received a new private message." << std::endl;
         private_messages.push_back(message(sender_uuid, current_user.uuid, msg, doc));
       }
		 }
    message_status = messageReader->return_loan(messageList, infoSeq);
    checkStatus(message_status, "private_message_informationDataReader::return_loan");
    sleep(1);
  }
  msg_mgr.deleteReader();
  msg_mgr.deleteTopic();
  msg_mgr.deleteParticipant();
  msg_mgr.deleteSubscriber();

}

void tsn_system::publish_message()
{
  //initializing data publisher and writer
  DDSEntityManager msg_mgr;

  msg_mgr.createParticipant("TSN");
  TSN::private_messageTypeSupport_var mt = new TSN::private_messageTypeSupport();
  msg_mgr.registerType(mt.in());

  char message_topic[] = "message";
  msg_mgr.createTopic(message_topic);

  msg_mgr.createPublisher();
  msg_mgr.createWriter(false);

  DDS::DataWriter_var dw = msg_mgr.getWriter();
  TSN::private_messageDataWriter_var messageWriter = TSN::private_messageDataWriter::_narrow(dw.in());
  TSN::private_message pm;

  char receiver_uuid[TSN::UUID_SIZE];

  int on_list_size = static_cast<int> (online_users.size());
  std::vector<user>::iterator user_it;
  int n = 0;
  //prints out a list of online users that can be sent a request to
  if(on_list_size > 0)
  {
    std::cout << "==========ONLINE USERS==========" << std::endl;
    for(user_it = online_users.begin(); user_it != online_users.end(); user_it++, n++)
    {
      std::cout << "(" << n << ") " << user_it->first_name << " " << user_it->last_name << std::endl;
    }
    std::cout << "\nChoose which user to request from: " << std::endl;

    n = 0;
    int choice;
    std::cin >> choice;

    //retrieving the UUID and highest post number of chosen user
    for(user_it = online_users.begin(); n < choice+1 ; user_it++, n++)
    {
      if(n == choice)
        {
          strcpy(receiver_uuid, user_it->uuid);
        }
    }
  }
  else
  {
    std::cout << "\nThere are no users online to send a message to." << std::endl;
    return;
  }

  std::string msg_body;
  std::cout << "Enter the message to send: " << std::endl;
  cin.ignore();
  getline(cin, msg_body);

  strcpy(pm.sender_uuid, current_user.uuid);
  strcpy(pm.receiver_uuid, receiver_uuid);
  pm.message_body = DDS::string_dup(msg_body.c_str());

  std::cout << "publish_message, pm.message_body: " << pm.message_body<< std::endl;

  struct timeval tp;
  gettimeofday(&tp, NULL);
  long date = tp.tv_sec;
  pm.date_of_creation = date;


  ReturnCode_t status = messageWriter->write(pm, DDS::HANDLE_NIL);
  checkStatus(status, "private_messageDataWriter::write");

  msg_mgr.deleteWriter();
  msg_mgr.deletePublisher();
  msg_mgr.deleteTopic();
  msg_mgr.deleteParticipant();
}