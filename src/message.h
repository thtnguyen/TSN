#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
#include "ccpp_tsn.h"

class message
{
    private:
        char sender_uuid[TSN::UUID_SIZE];
        char receiver_uuid[TSN::UUID_SIZE];
        std::string meesage_body;
        long creation_date;

    public:
        
        //this is a default constructor
        message();
        message(char *sender_uuid, char *receiver_uuid, std::string meesage_body, std::long creation_date);

        char* get_sender_uuid();
        char* get_reciever_uuid();
        string get_body();
        long get_doc();
};

#endif