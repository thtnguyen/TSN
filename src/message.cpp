#include "message.h"

message::message()
{
    this->message_body = "";
    creation_date = 0;
}

message::message(char *sender_uuid, char *receiver_uuid, std::string meesage_body, long creation_date)
{
    strcpy(this->sender_uuid, sender_uuid);
    strcpy(this->receiver_uuid, receiver_uuid);
    this->message_body = message_body;
    this->creation_date = creation_date;
}

char* message::get_sender_uuid()
{
    return sender_uuid;
}

std::string message::get_body()
{
    return message_body;
}

long message::get_doc()
{
    return creation_date;
}