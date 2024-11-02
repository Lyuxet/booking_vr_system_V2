#pragma once

#include <memory>
#include <string>
#include <thread>
#include <curl/curl.h>

class AsyncEmailSender : public std::enable_shared_from_this<AsyncEmailSender> {
public:
    AsyncEmailSender();
    ~AsyncEmailSender();

    void send(const std::string& subject, const std::string& body);
    void add_recipient(const std::string& email);

private:
    static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);
    struct upload_status {
        size_t lines_read = 0;
    };

    
    std::string smtp_url_ = "smtps://smtp.mail.ru:465";
    std::string email_from_ = "lyuxet@mail.ru";
    std::string smtp_user_ = "lyuxet@mail.ru";
    std::string smtp_password_ = "riErty7CndLS0pVjexaB";
    CURL* curl_;
    curl_slist* recipients_;
};
