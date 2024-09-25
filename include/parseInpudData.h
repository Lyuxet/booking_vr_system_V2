#include <unordered_map>
#include <string>
#include <vector>


namespace vr{
    std::vector<std::string> split_string(const std::string& str, char delimiter);
    std::vector<int> parse_int_list(const std::string& str);
    std::unordered_map<std::string, std::string> parse_form_data(const std::string& data);
}