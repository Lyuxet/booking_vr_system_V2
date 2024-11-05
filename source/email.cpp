#include "email.h"
#include "booking_vr.h"
#include "logger.h"

#include <fstream>
#include <string>
#include <sstream>


AsyncEmailSender::AsyncEmailSender() {
    initEmail();
}

void AsyncEmailSender::initEmail(){
    try
    {
        readConfigEmailFile();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();

        if (curl_) {
            curl_easy_setopt(curl_, CURLOPT_USERNAME, smtp_user_.c_str());
            curl_easy_setopt(curl_, CURLOPT_PASSWORD, smtp_password_.c_str());
            curl_easy_setopt(curl_, CURLOPT_URL, smtp_url_.c_str());
            curl_easy_setopt(curl_, CURLOPT_USE_SSL, CURLUSESSL_ALL);
            curl_easy_setopt(curl_, CURLOPT_MAIL_FROM, email_from_.c_str());

            recipients_ = curl_slist_append(nullptr, email_from_.c_str());
            if (!recipients_) {
                throw std::runtime_error("Не удалось инициализировать список получателей");
                
            }
            curl_easy_setopt(curl_, CURLOPT_MAIL_RCPT, recipients_);
        } else {
            throw std::runtime_error("Не удалось ининицалихировать email соединения");
        }       

    }
    catch(const std::exception& e)
    {
        Logger::getInstance().log("Ошибка инициализации подключения к smtp серверу " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_send_email.log");
    }
    
}

AsyncEmailSender::~AsyncEmailSender() {
    if (curl_) {
        curl_slist_free_all(recipients_);
        curl_easy_cleanup(curl_);
        curl_global_cleanup();
    }
}

void AsyncEmailSender::send(const std::string& subject, const std::string& body) {
    try
    {
        auto self = shared_from_this();
            std::thread([self, subject, body]() {
            struct upload_status upload_ctx;
            upload_ctx.lines_read = 0;
            std::string email_data = "Subject: " + subject + "\r\nContent-Type: text/html\r\n\r\n" + body;

            curl_easy_setopt(self->curl_, CURLOPT_READFUNCTION, payload_source);
            curl_easy_setopt(self->curl_, CURLOPT_READDATA, &email_data);
            curl_easy_setopt(self->curl_, CURLOPT_UPLOAD, 1L);

            CURLcode res = curl_easy_perform(self->curl_);
            if (res != CURLE_OK) {
                std::stringstream errorStream; errorStream << "Ошибка отправки email: " << curl_easy_strerror(res); 
                std::string errorMessage = errorStream.str();
                throw std::runtime_error(errorMessage);
            } 
        }).detach();
    }
    catch(const std::exception& e)
    {
        Logger::getInstance().log("Ошибка инициализации подключения к smtp серверу " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_send_email.log");
    }
    
    
}

size_t AsyncEmailSender::payload_source(void* ptr, size_t size, size_t nmemb, void* userp) {
    std::string* data = static_cast<std::string*>(userp);
    size_t total_size = size * nmemb;

    if (data->empty())
        return 0;

    size_t copy_size = std::min(total_size, data->size());
    std::memcpy(ptr, data->c_str(), copy_size);
    data->erase(0, copy_size);

    return copy_size;
}

void AsyncEmailSender::add_recipient(const std::string& email) {
    recipients_ = curl_slist_append(recipients_, email.c_str());
    curl_easy_setopt(curl_, CURLOPT_MAIL_RCPT, recipients_);
}


void AsyncEmailSender::readConfigEmailFile(){
    std::fstream file(configFilePath);
    if(!file.is_open()){
        throw std::runtime_error("Файл конфигурации почты не найден");
    }

    std::string line;
    while (std::getline(file, line))
    {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos){
            std::string key = line.substr(0,delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            if (key == "smtp_url"){
                smtp_url_ = value;
            }
            else if(key == "email_from"){
                email_from_ = value;
            }
            else if (key == "smtp_user"){
                smtp_user_ = value;
            }
            else if(key == "smtp_password"){
                smtp_password_ = value;
            }

        }
    }

    file.close();
    
}
