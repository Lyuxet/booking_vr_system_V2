#include "email.h"
#include "booking_vr.h"

AsyncEmailSender::AsyncEmailSender() {
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
            std::cerr << "Не удалось инициализировать список получателей." << std::endl;
            return;
        }
        curl_easy_setopt(curl_, CURLOPT_MAIL_RCPT, recipients_);
    } else {
        std::cerr << "Failed to initialize email connection.\n";
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
            std::cerr << "Error sending email: " << curl_easy_strerror(res) << "\n";
        } else {
            std::cout << "Email sent successfully.\n";
        }
    }).detach();
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
