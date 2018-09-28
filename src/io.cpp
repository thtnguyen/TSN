#include "io.h"

//needed for testing
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

  ReturnCode_t userinfo_status = -1;
  ReturnCode_t request_status = -1;
  while(1)
  {
    userinfo_status = userReader->take(userinfoList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(userinfo_status, "user_informationDataReader::take");
    
     for (DDS::ULong j = 0; j < userinfoList.length(); j++)
     {
       std::cout << "=== [Subscriber] user information received :" << std::endl;
       std::cout << "    UUID  : " << userinfoList[j].uuid  << std::endl;
       std::cout << "    Name : " << userinfoList[j].first_name << " " << userinfoList[j].last_name << std::endl;
		}
    userinfo_status = userReader->return_loan(userinfoList, infoSeq);
    checkStatus(userinfo_status, "user_informationDataReader::return_loan");
    sleep(1);

    //code for testing receiving requests

    request_status = requestReader->take(requestList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(request_status, "requestInformationDataReader::take");
    for (DDS::ULong j = 0; j < requestList.length(); j++)
     {
       std::cout << "=== [Subscriber] request received :" << std::endl;
       std::cout << "Requester UUID  : " << requestList[j].uuid  << std::endl;

      /* for(int n = 0; n < 2; n++)
       {
        std::cout << "Requested from user: " << requestList[j].user_requests[n].fulfiller_uuid << std::endl;
        std::cout << "Serial Nums of posts requested: ";
        for(int m = 0; m < 2; m++)
          std::cout << requestList[j].user_requests[n].requested_posts[m];
          std::cout << " ";
       }
       std::cout << std::endl;*/
    } 
    request_status = requestReader->return_loan(requestList, infoSeq);
    checkStatus(request_status, "requestDataReader::return_loan");
    sleep(1);
  }	

  userinfo_mgr.deleteReader();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();
  userinfo_mgr.deleteSubscriber();

  request_mgr.deleteReader();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();
  request_mgr.deleteSubscriber();

  response_mgr.deleteReader();
  response_mgr.deleteTopic();
  response_mgr.deleteParticipant();
  response_mgr.deleteSubscriber();
}

void publishRequest(user &current_user)
{
  //initializing data manager for user information
  DDSEntityManager request_mgr;

  request_mgr.createParticipant((current_user.first_name).c_str());
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

  //getting individual node requests
  while(true)
  {
    TSN::node_request nodeReqInstance;
    std::vector<TSN::serial_number> requested_p;

    string name;
    std::cout << "Enter name of a user to request from: -Not working yet" << std::endl;
    //getline(cin, name);
    //cin.ignore();

    //using random uuid for testing purposes until we can retrieve them through names

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    string uuidstring = boost::uuids::to_string(uuid);
    char myuuid[TSN::UUID_SIZE] = {};
    strcpy(myuuid, uuidstring.c_str());

    std::cout << "Randomly generated uuid for testing: " << myuuid << std::endl;
    strcpy(nodeReqInstance.fulfiller_uuid, myuuid);

    std::cout << "Enter the serial numbers of the posts you want from that user on a separate line, enter 0 to stop:" << std::endl;
    TSN::serial_number get_input;
    while(true)
    {
      std::cin >> get_input;
      std::cout << "input was: " << get_input << std::endl;

      if(!get_input)
      {
        strcpy(nodeReqInstance.fulfiller_uuid, myuuid);

        int post_seq_length = static_cast<int> (requested_p.size());
        nodeReqInstance.requested_posts.length(post_seq_length);

        int n = 0;
        std::vector<TSN::serial_number>::iterator it;
        for(it = requested_p.begin(); it != requested_p.end(); it++, n++)
          nodeReqInstance.requested_posts[n] = *it;

        break;
      }
      requested_p.push_back(get_input);
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
    cin.ignore();
  }  

  strcpy(requestInstance.uuid, current_user.uuid);
  std::cout << "=== [Publisher] publishing request on network :" << std::endl;
  std::cout << "  Requester UUID  : " << requestInstance.uuid << std::endl;
 
  ReturnCode_t status = requestWriter->write(requestInstance, DDS::HANDLE_NIL);
  checkStatus(status, "requestDataWriter::write");

  request_mgr.deleteWriter();
  request_mgr.deletePublisher();
  request_mgr.deleteTopic();
  request_mgr.deleteParticipant();

}

void publishUserInfo(user &current_user)
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
  

  std::cout << "=== [Publisher] publishing user info on network :" << std::endl;
  std::cout << "    UUID  : " << userinfoInstance.uuid << std::endl;
  std::cout << "    Name : " << userinfoInstance.first_name << " " << userinfoInstance.last_name << std::endl;

  ReturnCode_t status = userinfoWriter->write(userinfoInstance, DDS::HANDLE_NIL);
  checkStatus(status, "user_informationDataWriter::write");

  userinfo_mgr.deleteWriter();
  userinfo_mgr.deletePublisher();
  userinfo_mgr.deleteTopic();
  userinfo_mgr.deleteParticipant();
}