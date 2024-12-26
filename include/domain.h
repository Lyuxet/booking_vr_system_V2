#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdexcept> 
#include <unordered_set>



int getZellerDayOfWeek(int year, int month, int day) {
    if (month <= 2) {
        month += 12;
        year--;
    }
    
    int k = year % 100;
    int j = year / 100;
    
    int h = (day + 13*(month + 1)/5 + k + k/4 + j/4 - 2*j) % 7;
    
    return ((h + 5) % 7 + 7) % 7;
}

std::string convertDate(std::string_view date) {
    std::unordered_map<int, std::string> months = {
        {1, "января"}, {2, "февраля"}, {3, "марта"}, {4, "апреля"}, 
        {5, "мая"}, {6, "июня"}, {7, "июля"}, {8, "августа"}, 
        {9, "сентября"}, {10, "октября"}, {11, "ноября"}, {12, "декабря"}
    };
    std::vector<int> dateParts;
    std::string date_str(date); 

    std::istringstream iss(date_str);
    std::string part;
    char delimiter = date_str.find('.') != std::string::npos ? '.' : '-';
    
    while (std::getline(iss, part, delimiter)) {
        dateParts.push_back(std::stoi(part));
    }

    if (dateParts.size() != 3) {
        throw std::invalid_argument("Invalid date format");
    }

    int year = dateParts[0];
    int month = dateParts[1];
    int day = dateParts[2];

    std::ostringstream oss;
    oss << day << " " << months[month] << " " << year;

    return oss.str();
}
