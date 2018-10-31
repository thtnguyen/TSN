#include "post.h"

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
        this->parent_post_id = -1;
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
