#include <httplib.h>
#include "Connect_to_DB.h"

namespace vr{
    void add_booking(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool);
    void get_availability(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool);

}