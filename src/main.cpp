#include <iostream>
#include <string>
#include <fstream>
#include <thread>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "DDSEntityManager.h"
#include "ccpp_tsn.h"
//#include "user.h"


void background(string first_name)
{
	TSN::requestSeq requestList;
  TSN::responseSeq responseList;
  TSN::user_informationSeq userinfoList;

	TSN::SampleInfoSeq infoSeq;

	TSN::DDSEntityManager userinfo_mgr;
  TSN::DDSEntityManager request_mgr;
  TSN::DDSEntityManager response_mgr;  

	userinfo_mgr.createParticipant(first_name);

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

  //CONTINUE IMPLEMENTING HERE
	mgr.createSubscriber();
	mgr.createReader();
	DataReader_var dr = mgr.getReader();

	MsgDataReader_var MsgReader = MsgDataReader::_narrow(dr.in());
	checkHandle(MsgReader.in(), "MsgDataReader::_narrow");

  std::cout << "=== [Subscriber] Ready ..." << std::endl;

  ReturnCode_t status =  - 1;
	
	while(1)
	{
		status = MsgReader->take(msgList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "msgDataReader::take");
    for (DDS::ULong j = 0; j < msgList.length(); j++)
    {
			if(strcmp(msgList[j].name, name) != 0)
			{
      std::cout << "=== [Subscriber] message received :" << std::endl;
      std::cout << "    Name  : " << msgList[j].name << std::endl;
      std::cout << "    Message : \"" << msgList[j].message << "\"" << std::endl;
			}
    }
    status = MsgReader->return_loan(msgList, infoSeq);
    checkStatus(status, "MsgDataReader::return_loan");
		sleep(1);
	}

  mgr.deleteReader();
  mgr.deleteWriter();
  mgr.deletePublisher();
  mgr.deleteTopic();
  mgr.deleteParticipant();
}

int main (int argc, char* argv[])
{

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::cout << "Welcome to The Social Network." << std::endl;
  std::cout << "Your UUID is: " << uuid << std::endl;

	std::cout << "Enter your first name: " << std::endl;
	string first_name<70>;
	std::cin >> first_name;
	std::cin.ignore();

  std::cout << "Enter your last name: " << std::endl;
	string last_name<70>;
	std::cin >> last_name;
	std::cin.ignore();

	std::thread BG (background, first_name);
	//user this_user = load_user_data(".tsn");

	//code to publish startup info goes here

	TSN::DDSEntityManager userinfo_mgr;
  TSN::DDSEntityManager request_mgr;
  TSN::DDSEntityManager response_mgr;

	mgr.createParticipant(first_name);

  //CHANGE IMPLMENTATION HERE!!!
	MsgTypeSupport_var mt = new MsgTypeSupport();
	mgr.registerType(mt.in());

	char topic[] = "Topic";
	mgr.createTopic(topic);

	mgr.createPublisher();

	mgr.createWriter(false);

	DataWriter_var dw = mgr.getWriter();

	MsgDataWriter_var MsgWriter = MsgDataWriter::_narrow(dw.in());
	int choice;

	while(1)
	{
		std::cin >> choice;
		if(choice == 1)
		{
			std::cout << "Enter msg: " << std::endl;
			char msg[255];
			std::cin.ignore();
			std::cin >> msg;

			Msg msgInstance;
			msgInstance.name = DDS::string_dup(name);
			msgInstance.message = DDS::string_dup(msg);

			std::cout << "=== [Publisher] writing a message containing :" << std::endl;
			std::cout << "    Name  : " << msgInstance.name << std::endl;
			std::cout << "    Message : \"" << msgInstance.message << "\"" << std::endl;

			ReturnCode_t status = MsgWriter->write(msgInstance, DDS::HANDLE_NIL);
			checkStatus(status, "MsgDataWriter::write");
			sleep(1);
		}
		else
		{
			exit(0);
		}
	}
	
  BG.join();
	
  mgr.deleteReader();
  mgr.deleteWriter();
  mgr.deletePublisher();
  mgr.deleteTopic();
  mgr.deleteParticipant();

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
