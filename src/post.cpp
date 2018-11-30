#include "post.h"

post::post()
{
    serial_num = 0;
    post_body = " ";
    creation_date = 0;

    char id[TSN::UUID_SIZE] = "000000000000000000000000000000000000";
    strcpy(this->parent_uuid, id);
    this->parent_post_id = 0;
}
post::post(TSN::serial_number sn, std::string body, long doc, bool thread, TSN::serial_number parent_post_id, char* parent_uuid)
{
    serial_num = sn;
    post_body = body;
    creation_date = doc;
    if(thread)
    {
        strcpy(this->parent_uuid, parent_uuid);
        this->parent_post_id = parent_post_id;
    }
    else
    {
        this->parent_post_id = 0;
    }
}

std::string post::get_body()
{
    return post_body;
}
long post::get_doc()
{
    return creation_date;
}
TSN::serial_number post::get_sn()
{
    return serial_num;
}
TSN::serial_number post::get_parent_sn()
{
    return parent_post_id;
}
char* post::get_parent_uuid()
{
    return parent_uuid;
}
void post::set_child_uuid(char* uuid)
{
    strcpy(this->child_uuid, uuid);
}
void post::set_child_post(TSN::serial_number post_id)
{
    this->child_post_id = post_id;
}
