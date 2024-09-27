#include <httplib.h>
#include "Connect_to_DB.h"

namespace vr{
    void ArenaBookingInsert(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool);
    void Availability(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool);
    void CubesBookingInsert(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool);


}