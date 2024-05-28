#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <map>
#include <exception>

namespace jinja::config {

class Section {
private:
  std::map<std::string, std::string> lines;

  std::pair<std::string, std::string> parse_line(const std::string& input) {
    std::string left, right;
    std::size_t pos = input.find('=');

    if (pos != std::string::npos) {
      left  = input.substr(0, pos);
      right = input.substr(pos + 1);
    }
    else {
      // If '=' is not found, return the input as the left part and an empty
      // string as the right part
      left  = input;
      right = "";
    }

    return std::make_pair(left, right);
  }

  std::string remove_space(const std::string& input) {
    std::string result;
    result.reserve(input.length());
    for (char c : input) {
      if (c != ' ') {
        result += c;
      }
    }
    return result;
  }

  void _read_value(const std::string& key, int& x) {
    try {
      if (!lines.count(key))
        throw std::invalid_argument("No such thing");
      x = stoi(lines[key]);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
  }

  void _read_value(const std::string& key, std::string& x) {
    try {
      if (!lines.count(key))
        throw std::invalid_argument("No such thing");
      x = lines[key];
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
  }

  void _read_value(const std::string& key, double& d) {
    try {
      if (!lines.count(key))
        throw std::invalid_argument("No such thing");
      d = stod(lines[key]);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
  }
  void _read_value(const std::string& key, long long& ll) {
    try {
      if (!lines.count(key))
        throw std::invalid_argument("No such thing");
      ll = stoll(lines[key]);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
  }

public:
  Section() {}
  Section(const std::vector<std::string>& lines_in) {
    for (auto& line : lines_in) {
      this->set_line(line);
    }
  }
  void set_line(const std::string& s) {
    auto key_value    = this->parse_line(s);
    std::string key   = this->remove_space(key_value.first);
    std::string value = this->remove_space(key_value.second);
    lines[key]        = value;
  }

  template <typename T>
  void read_value(std::string& key, T& holder) {
    try {
      this->_read_value(key, holder);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  template <typename T>
  void read_value(std::string&& key, T& holder) {
    try {
      this->_read_value(key, holder);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
};

struct ConfigBase {
  std::filesystem::path file_path;
};
} // namespace jinja::config