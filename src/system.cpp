#include <sys/time.h>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <thread>
#include <vector>

#include "system.h"

tsn_system::tsn_system(user& cu)
{
   current_user = cu;
   std::vector<user> on_vector;
   online_users = on_vector;

   std::vector<user> all_vector;
   all_users = all_vector;
}

void tsn_system::user_publisher()
{
  while(true)
  {
    publishUserInfo();
    sleep(30);
  }
}
void tsn_system::request_listener()
{
  TSN::requestSeq requestList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager request_mgr;

  request_mgr.createParticipant("TSN");
  TSN::requestTypeSupport_var reqts = new TSN::requestTypeSupport();
  request_mgr.registerType(reqts.in());

  char req_topic[] = "request";

  request_mgr.createTopic(req_topic);
  request_mgr.createSubscriber();
  request_mgr.createReader();

  DDS::DataReader_var req_data = request_mgr.getReader();
  TSN::requestDataReader_var requestReader = TSN::requestDataReader::_narrow(req_data.in());
  checkHandle(requestReader.in(), "requestDataReader::_narrow");

  ReturnCode_t request_status = -1;
  
  while(true)
  {
    request_status = requestReader->take(requestList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(request_status, "requestInformationDataReader::take");

    if(requestList.length() > 0)
    {
      for (DDS::ULong j = 0; j < requestList.length(); j++)
      {
        for(DDS::ULong i = 0; i < requestList[j].user_requests.length(); i++)
          if(strcmp(requestList[j].user_requests[i].fulfiller_uuid, current_user.uuid) == 0)
          {
            publishResponse(requestList[j]);
          }
      }  
    } 
    request_status = requestReader->return_loan(requestList, infoSeq);
    checkStatus(request_status, "requestDataReader::return_loan");
    sleep(1);
  }

  request_mgr.deleteReader();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();
  request_mgr.deleteSubscriber();
}

void tsn_system::response_listener()
{
  TSN::responseSeq responseList;
  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager response_mgr;

  response_mgr.createParticipant("TSN");
  TSN::responseTypeSupport_var respts = new TSN::responseTypeSupport();
  response_mgr.registerType(respts.in());

  char resp_topic[] = "response";

  response_mgr.createTopic(resp_topic);
  response_mgr.createSubscriber();
  response_mgr.createReader();

  DDS::DataReader_var resp_data =  response_mgr.getReader();
  TSN::responseDataReader_var responseReader = TSN::responseDataReader::_narrow(resp_data.in());
  checkHandle(responseReader.in(), "responseDataReader::_narrow");

  ReturnCode_t response_status = -1;
  
  while(true)
  {
    response_status = responseReader->take(responseList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(response_status, "responseDataReader::take");
    
    for (DDS::ULong j = 0; j < responseList.length(); j++)
     {
       if((strcmp(responseList[j].uuid, current_user.uuid) != 0) && responseList[j].post_id > 0)
       {
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
    response_status = responseReader->return_loan(responseList, infoSeq);
    checkStatus(response_status, "response_informationDataReader::return_loan");
    sleep(1);
  }
    response_mgr.deleteReader();
    response_mgr.deleteTopic();
    response_mgr.deleteParticipant();
    response_mgr.deleteSubscriber();
}

void tsn_system::user_listener()
{
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
         
         //if user is already known, delete old record in vector
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
         std::string path = home + "/.tsnusers";
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
  userinfo_mgr.deleteReader();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();
  userinfo_mgr.deleteSubscriber();
}


long tsn_system::publishRequest()
{
  //initializing data manager for user information
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
  //getting individual node requests
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
    post_seq_length = static_cast<int> (requested_p.size()); //declare within loop after debugging
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

    //adding all individual node requests into one request
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

  request_mgr.deleteWriter();
  request_mgr.deletePublisher();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();

  sleep(n);
  return date;
}

void tsn_system::publishUserInfo()
{

  //initializing data manager for user information
  DDSEntityManager userinfo_mgr;
  userinfo_mgr.createParticipant("TSN");

  TSN::user_informationTypeSupport_var mt = new TSN::user_informationTypeSupport();
  userinfo_mgr.registerType(mt.in());

  char userinfo_topic[] = "user_information";
  userinfo_mgr.createTopic(userinfo_topic);

  userinfo_mgr.createPublisher();
  userinfo_mgr.createWriter(false);


  DDS::DataWriter_var dw = userinfo_mgr.getWriter();
  TSN::user_informationDataWriter_var userinfoWriter = TSN::user_informationDataWriter::_narrow(dw.in());
  TSN::user_information userinfoInstance;

  
  userinfoInstance.first_name = DDS::string_dup((current_user.first_name).c_str());
  userinfoInstance.last_name = DDS::string_dup((current_user.last_name).c_str());
  strcpy(userinfoInstance.uuid, current_user.uuid);
  userinfoInstance.number_of_highest_post = current_user.get_highest_pnum();
  userinfoInstance.date_of_birth = current_user.date_of_birth;

  //get length of vector
  int n = 0;
  std::vector<std::string>::iterator it;
  for(it = current_user.interests.begin(); it != current_user.interests.end(); it++)
  {
    n++;
  }
  userinfoInstance.interests.length(n);

  //store interests in sequence
  n = 0;
  for(it = current_user.interests.begin(); it != current_user.interests.end(); it++, n++)
  {
    userinfoInstance.interests[n] = DDS::string_dup(it->c_str());
  }

  ReturnCode_t status = userinfoWriter->write(userinfoInstance, DDS::HANDLE_NIL);
  checkStatus(status, "user_informationDataWriter::write");

  userinfo_mgr.deleteWriter();
  userinfo_mgr.deletePublisher();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();
}

void tsn_system::publishResponse(TSN::request r)
{

  //initializing data manager for user information
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
    if(strcmp(r.user_requests[j].fulfiller_uuid, current_user.uuid) == 0)
      my_node_req = r.user_requests[j];
  
    for(DDS::ULong j = 0; j < my_node_req.requested_posts.length(); j++)
    {
      int n = 0;
      TSN::response responseInstance;

      std::string body;
      TSN::serial_number serial_num;
      long doc;
      std::vector<post>::iterator it;
      bool post_found = false;
      for(it = current_user.posts.begin(); it != current_user.posts.end(); it++, n++)
      {
        if(my_node_req.requested_posts[j] == it->get_sn())
        {
          body = it->get_body();
          serial_num = it->get_sn();
          doc = it->get_doc();
          post_found = true;
          break;
        }
      }

      if(post_found)
      {
        strcpy(responseInstance.uuid, current_user.uuid);
        responseInstance.post_id = serial_num;
        responseInstance.date_of_creation = doc;
        responseInstance.post_body = DDS::string_dup(body.c_str());

        ReturnCode_t status = responseWriter->write(responseInstance, DDS::HANDLE_NIL);
        checkStatus(status, "user_informationDataWriter::write");
        sleep(2);
      }
    }
  }
  response_mgr.deleteWriter();
  response_mgr.deletePublisher();
  response_mgr.deleteTopic();
  response_mgr.deleteParticipant();
}

void tsn_system::load_user_data()
{
  std::string home = getenv("HOME"); //.tsn is always stored in home directory
  std::string path = home + "/.tsnusers";
  std::ifstream in(path);

  std::string first_name;
  std::string last_name;
  char myuuid[TSN::UUID_SIZE] = {};
  std::vector<post> posts;
  std::vector<std::string> interests;

  std::string temp; 
   //loading info about known nodes
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
  in.close();
  
  //now attempting to load info of the current user/node from .tsn
  path = home + "/.tsn";
  in.open(path);

  //the .tsn file does not exist so obtain data directly from user
  if(!in)
  {
    current_user = create_new_user(path);
    return;
  }

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
  
  while(getline(in, temp))
  {
    if(temp == "END INTERESTS")
      break;
				
    interests.push_back(temp);    
  }

  long doc;
  TSN::serial_number sn;
  std::string body;

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

    posts.push_back(post(sn, body, doc));
  }
  in.close();

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
    sleep(300);
  }
}

void tsn_system::request_all_posts(user requested_user)
{
    //initializing data manager for user information
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

  TSN::node_request nodeReqInstance;
  strcpy(nodeReqInstance.fulfiller_uuid, requested_user.uuid);

  nodeReqInstance.requested_posts.length(requested_user.get_highest_pnum());
  std::vector<TSN::serial_number> requested_p;
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
  std::string path = home + "/.tsnusers";
  std::remove(path.c_str());

  online_users.clear();
  all_users.clear();

  std::cout << "\nData about all other known and online nodes have been wiped." << std::endl;
}

void tsn_system::create_post()
{
  TSN::serial_number sn = (TSN::serial_number) current_user.get_highest_pnum()+1;

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

  std::string home = getenv("HOME"); //.tsn is always stored in home directory
  std::string path = home + "/.tsn";
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
    current_user.interests.clear();
    string interest;
    std::cout << "Enter your new interests, entering a newline after each interest (type 0 to stop): " << std::endl;
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
}