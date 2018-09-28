#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <cstring>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

int state;
void background(string fname)
{
  char first_name[70] = {};
  strcpy(first_name, fname.c_str());
  TSN::requestSeq requestList;
  TSN::responseSeq responseList;
  TSN::user_informationSeq userinfoList;

  DDS::SampleInfoSeq infoSeq;

  DDSEntityManager userinfo_mgr;
  DDSEntityManager request_mgr;
  DDSEntityManager response_mgr;  

  userinfo_mgr.createParticipant(first_name);
  request_mgr.createParticipant(first_name);
  response_mgr.createParticipant(first_name);

  TSN::requestTypeSupport_var reqts = new TSN::requestTypeSupport();
  TSN::responseTypeSupport_var respts = new TSN::responseTypeSupport();
  TSN::user_informationTypeSupport_var uits = new TSN::user_informationTypeSupport();


  request_mgr.registerType(reqts.in());
  response_mgr.registerType(respts.in());
  userinfo_mgr.registerType(uits.in());

  char user_topic[] = "user_information";
  char req_topic[] = "request";
  char resp_topic[] = "response";
	
  userinfo_mgr.createTopic(user_topic);
  request_mgr.createTopic(req_topic);
  response_mgr.createTopic(resp_topic);

  userinfo_mgr.createSubscriber();
  request_mgr.createSubscriber();
  response_mgr.createSubscriber();

  userinfo_mgr.createReader();
  request_mgr.createReader();
  response_mgr.createReader();

  DDS::DataReader_var user_data = userinfo_mgr.getReader();
  DDS::DataReader_var req_data = request_mgr.getReader();
  DDS::DataReader_var resp_data =  response_mgr.getReader();

  TSN::user_informationDataReader_var userReader = TSN::user_informationDataReader::_narrow(user_data.in());
  TSN::requestDataReader_var requestReader = TSN::requestDataReader::_narrow(req_data.in());
  TSN::responseDataReader_var responseReader = TSN::responseDataReader::_narrow(resp_data.in());

  checkHandle(userReader.in(), "user_informationDataReader::_narrow");
  checkHandle(requestReader.in(), "requestDataReader::_narrow");
  checkHandle(responseReader.in(), "responseDataReader::_narrow");

  std::cout << "=== [Subscriber] Ready ..." << std::endl;

  ReturnCode_t status =  - 1;

  while(1)
  {
    status = userReader->take(userinfoList, infoSeq, LENGTH_UNLIMITED,
     ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "user_informationDataReader::take");
    
     for (DDS::ULong j = 0; j < userinfoList.length(); j++)
     {
       std::cout << "=== [Subscriber] message received :" << std::endl;
       std::cout << "    UUID  : " << userinfoList[j].uuid  << std::endl;
       std::cout << "    Name : " << userinfoList[j].first_name << " " << userinfoList[j].last_name << std::endl;
		}
    sleep(2);
  }			

    status = userReader->return_loan(userinfoList, infoSeq);
    checkStatus(status, "user_informationDataReader::return_loan");

  userinfo_mgr.deleteReader();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();

  request_mgr.deleteReader();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();

  response_mgr.deleteReader();
  response_mgr.deleteTopic();
  response_mgr.deleteParticipant();

}

void printMenu()
{

  //user this_user = load_user_data(".tsn");

  std::cout << "Select what you want to do:" << std::endl << std::endl;
  std::cout << "(1) Recieve Messages - TEMPORARILY DISABLED" << std::endl;
  std::cout << "(2) Publish Messages - TEMPORARILY DISABLED" << std::endl;
  std::cout << "Type any other key to Exit" << std::endl;
}

void publishUserInfo(user current_user)
{

  //initializing data manager for user information
  DDSEntityManager userinfo_mgr;
  userinfo_mgr.createParticipant((current_user.first_name).c_str());

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
  

  //publishing the data over the network
  std::cout << "=== [Publisher] publishing user info on network :" << std::endl;
  std::cout << "    UUID  : " << userinfoInstance.uuid << std::endl;
  std::cout << "    Name : " << userinfoInstance.first_name << " " << userinfoInstance.last_name << std::endl;

  ReturnCode_t status = userinfoWriter->write(userinfoInstance, DDS::HANDLE_NIL);
  checkStatus(status, "user_informationDataWriter::write");


  //deleting the data manager
  userinfo_mgr.deleteWriter();
  userinfo_mgr.deletePublisher();
  userinfo_mgr.deleteTopic();

  state = -1;
  printMenu();
}

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

void menu()
{
  printMenu();
  state = -1;
  int choice;

  while(state != 0)
  {
    std::cin >> choice;
    if(choice == 1)
    std::cin >> state;
    std::cin.ignore();
    if(state == 0)
    {
      //exit loop and return to main method
      break;
    }		
    if(state == 1)
    {
      viewMessages();
    }
    if(state == 2)
    {
      //publishUserInfo()
      //publishMessages(false);
    }

    state = -1;
    
  }

  //publishMessages(true);
  std::cout << "EXITING: STATE = " << state << std::endl;
}

int main (int argc, char* argv[])
{	
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  string uuidstring = boost::uuids::to_string(uuid);
  char myuuid[TSN::UUID_SIZE] = {};
  strcpy(myuuid, uuidstring.c_str());

  std::cout << "Welcome to The Social Network." << std::endl;
  std::cout << "Your UUID is: " << uuid << std::endl;

  std::cout << "Enter your first name: " << std::endl;
  std::string first_name;
  std::cin >> first_name;
  std::cin.ignore();

  std::cout << "Enter your last name: " << std::endl;
  std::string last_name;
  std::cin >> last_name;
  std::cin.ignore();

  std::vector<std::string> string_v;
  std::vector<post> post_v;
  user current_user = user(first_name, last_name, 100, myuuid, string_v, post_v );

  //get user info
  publishUserInfo(current_user);

  std::thread BG (background, first_name);

  //enter menu state
  menu();

  std::cout << "end menu" << std::endl;
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
