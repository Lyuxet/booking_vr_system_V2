#include "booking_vr.h"
#include "Connect_to_DB.h"
#include <memory>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>

void Booking::Add_date(Client_data client, Booking_data booking){
    clients_ = client;
    booking_ = booking_;
}

void Arena::Open_arena() const {
    if (!conn_.Check_Connect()){
        throw sql::SQLException("Error connect to db");
    }

    // Начинаем транзакцию
    conn_.Check_Connect()->setAutoCommit(false); // Отключаем автокомми

    std::unique_ptr<sql::PreparedStatement> pstmt(conn_.Check_Connect()->prepareStatement("INSERT IGNORE INTO Clients(first_name, last_name, phone, email) VALUES (?,?,?,?)"));
    pstmt->setString(1, clients_.first_name);
    pstmt->setString(2, clients_.last_name);
    pstmt->setString(3, clients_.phone);
    pstmt->setString(4, clients_.email);


    conn_.Check_Connect()->commit();
    std::cout << "Transaction completed successfully." << std::endl;

    
}



