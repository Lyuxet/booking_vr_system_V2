#pragma once

#include <memory>
#include <string>
#include <thread>
#include <curl/curl.h>

class AsyncEmailSender : public std::enable_shared_from_this<AsyncEmailSender> {
public:
    AsyncEmailSender();
    ~AsyncEmailSender();
    void initEmail();

    void send(const std::string& subject, const std::string& body);
    void add_recipient(const std::string& email);

private:
    void readConfigEmailFile();
    static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);
    struct upload_status {
        size_t lines_read = 0;
    };

    std::string configFilePath = "email_config.conf";
    std::string smtp_url_;
    std::string email_from_;
    std::string smtp_user_;
    std::string smtp_password_;
    CURL* curl_;
    curl_slist* recipients_;
};
