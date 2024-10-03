#include <httplib.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "Connect_to_DB.h"
#include <boost/json.hpp>


namespace beast = boost::beast;  
namespace http = beast::http;  
namespace json = boost::json;
namespace vr{
    void ArenaBookingInsert(const json::value& jsonData, http::response<http::string_body>& res, ConnectionPool& pool);
    void Availability(const http::request<http::string_body>& req, http::response<http::string_body>& res, ConnectionPool& pool);
    void CubesBookingInsert(const json::value& jsonData, http::response<http::string_body>& res, ConnectionPool& pool);


}