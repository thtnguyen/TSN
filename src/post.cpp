#include "post.h"

post::post(TSN::serial_number sn, std::string body, long doc)
{
    serial_num = sn;
    post_body = body;
    creation_date = doc;
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
