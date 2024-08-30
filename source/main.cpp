#include "Connect_to_DB.h"
#include <iostream>

int main(void){

    Connection conn{};

    try
    {
        conn.Connect_database();
        std::cout << "Connect succsessful" << std::endl;
        conn.Disconnect_database();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    

}