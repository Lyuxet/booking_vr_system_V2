#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include "test_delete_data.h"
#include "test_update_data.h"

#include <iostream>
#include <thread>
#include <vector>
#include <httplib.h>



int main(){

   httplib::Server server;

   server.Post("/data", [](const httplib::Request& req, httplib::Response& res){
      
   });

   server.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        
   });

   server.Options("/*", [](const httplib::Request&, httplib::Response& res) {
        
    });

    std::cout << "Starting server on port 8080..." << std::endl;
    server.listen("0.0.0.0", 8080);



}