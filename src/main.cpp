#include <iostream>
#include <string>
#include <fstream>
#include <thread>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
#include "user.h"

int state;
user::user current_user;
void background(string first_name)
{
	TSN::requestSeq requestList;
  TSN::responseSeq responseList;
  TSN::user_informationSeq userinfoList;

	TSN::SampleInfoSeq infoSeq;

	TSN::DDSEntityManager userinfo_mgr;
  TSN::DDSEntityManager request_mgr;
  TSN::DDSEntityManager response_mgr;  

  requestTypeSupport_var reqts = new requestTypeSupport();
  responseTypeSupport_var respts = new reponseTypeSupport();
  user_informationTypeSupport_var uits = new user_informationTypeSupport;

	userinfo_mgr.createParticipant(first_name);
  request_mgr.createParticipant(first_name);
  response_mgr.createParticipant(first_name);

  requestTypeSupport_var reqts = new requestTypeSupport();
  responseTypeSupport_var respts = new reponseTypeSupport();
  user_informationTypeSupport_var uits = new user_informationTypeSupport;

  request_mgr.registerType(reqts.in());
  response_mgr.registerType(respts.in());
  userinfo_mgr.register(Type(uits.in());

	char user_topic[] = "user_information";
  char req_topic[] = "request";
  char resp_topic[] = "response";
	
  userinfo_mgr.createTopic(user_topic);
  request_mgr.createTopic(req_topic);
  reponse_mgr.createTopic(resp_topic);

	userinfo_mgr.createSubscriber();
	request_mgr.createSubscriber();
  response_mgr.createSubscriber();

	DataReader_var user_data = userinfo_mgr.getReader();
  DataReader_var req_data = request_mgr.getReader();
  DataReader_var resp_data =  response_mgr.getReader();

	user_informationDataReader_var userReader = user_informationDataReader::_narrow(user_data.in());
  requestDataReader_var requestReader = requestDataReader::_narrow(req_data.in());
  responseDataReader_var responseReader = responseDataReader::_narrow(resp_data.in());

	checkHandle(userReader.in(), "user_informationDataReader::_narrow");
  checkHandle(requestReader.in(), "requestDataReader::_narrow");
  checkHandle(responseReader.in(), "responseDataReader::_narrow");

  //std::cout << "=== [Subscriber] Ready ..." << std::endl;

  ReturnCode_t status =  - 1;
	
	while(1)
	{
		status = userReader->take(userinfoList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "user_informationDataReader::take");
    

    /*
		//run when in read message state
	  for (DDS::ULong j = 0; j < msgList.length(); j++)
    {
	    std::cout << "=== [Subscriber] message received :" << std::endl;
	    std::cout << "    Name  : " << userinfoList[j].first_name << " " << userinfoList[j].last_name << std::endl;
	    std::cout << "    UUID : \"" << userinfoList[j].uuid << "\"" << std::endl;
		}*/
  }			

    status = userinfoReader->return_loan(userinfoList, infoSeq);
    checkStatus(status, "user_informationDataReader::return_loan");

		sleep(1);	

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

void publishUserInfo()
{

  //initializing data manager for user information
	DDSEntityManager userinfo_mgr;
	userinfo_mgr.createParticipant("TSN");

	user_informationTypeSupport_var mt = new user_informationTypeSupport();
	userinfo_mgr.registerType(mt.in());

	char userinfo_topic[] = "user_information";
	mgr.createTopic(userinfo_topic);

	mgr.createPublisher();
	mgr.createWriter(false);

	DataWriter_var dw = userinfo_mgr.getWriter();
	user_informationDataWriter_var userinfoWriter = user_informationDataWriter::_narrow(dw.in());

	user_information userinfoInstance;
	userinfoInstance.first_name = DDS::string_dup(first_name);
	userinfoInstance.last_name = DDS::string_dup(last_name);
  //userinfoInstance.uuid = current_user.uuid;
  

  //publishing the data over the network
	std::cout << "=== [Publisher] publishing user info on network :" << std::endl;
	/*std::cout << "    Name  : " << msgInstance.name << std::endl;
	std::cout << "    Message : \"" << msgInstance.message << "\"" << std::endl;*/

	ReturnCode_t status = userinfoWriter->write(userinfoInstance, DDS::HANDLE_NIL);
	checkStatus(status, "user_informationDataWriter::write");


  //deleteing the data manager
  userinfo_mgr.deleteWriter();
  userinfo_mgr.deletePublisher();
  userinfo_mgr.deleteTopic();
}

/*void publishMessages(bool kill)
{
  DDSEntityManager mgr;
  mgr.createParticipant("TSN");

  MsgTypeSupport_var mt = new MsgTypeSupport();
  mgr.registerType(mt.in());

  char topic[] = "Topic";
  mgr.createTopic(topic);

  mgr.createPublisher();
  mgr.createWriter(false);

  DataWriter_var dw = mgr.getWriter();
  MsgDataWriter_var MsgWriter = MsgDataWriter::_narrow(dw.in());

  std::cout << "Enter msg: " << std::endl;
  char msg[255];	
  std::cin >> msg;
  std::cin.ignore();

  Msg msgInstance;
  msgInstance.name = DDS::string_dup(name);
  msgInstance.message = DDS::string_dup(msg);

  std::cout << "=== [Publisher] writing a message containing :" << std::endl;
  std::cout << "    Name  : " << msgInstance.name << std::endl;
  std::cout << "    Message : \"" << msgInstance.message << "\"" << std::endl;

  ReturnCode_t status = MsgWriter->write(msgInstance, DDS::HANDLE_NIL);
  checkStatus(status, "MsgDataWriter::write");

  state = -1;
  printMenu();
}*/

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
			publishMessages(false);
		}
		state = -1;
	}

	publishMessages(true);
	std::cout << "EXITING: STATE = " << state << std::endl;
}

int main (int argc, char* argv[])
{	
  current_user.uuid = boost::uuids::random_generator()();
  std::cout << "Welcome to The Social Network." << std::endl;
  std::cout << "Your UUID is: " << current_user.uuid << std::endl;

	std::cout << "Enter your first name: " << std::endl;
	string first_name<70>;
	std::cin >> first_name;
	std::cin.ignore();

  std::cout << "Enter your last name: " << std::endl;
	string last_name<70>;
	std::cin >> last_name;
	std::cin.ignore();

  //get user info
  publishUserInfo();

	std::thread BG (background, first_name);

	//enter menu state
	menu();

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
