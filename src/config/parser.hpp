#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <map>
#include <exception>

namespace jinja::config {

const static std::string default_config_file_path =
    "C:/Users/ryngo/working/debug_jinja/etc/config.toml";

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

  template <typename T>
  void _read_value(const std::string& key, std::vector<T>& vec) {
    try {
      if (!lines.count(key))
        throw std::invalid_argument("No such thing");
      _read_elements(lines[key], vec);
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
  }

  void _read_elements(std::string str, std::vector<int>& vec) {
    char delimiter = ',';
    auto _str      = remove_space(str);
    _str           = _str.substr(1, _str.size() - 2);
    std::stringstream sstream{_str};
    std::string buff;
    while (getline(sstream, buff, delimiter)) {
      vec.push_back(stoi(buff));
    }
  }
  void _read_elements(std::string str, std::vector<double>& vec) {
    char delimiter = ',';
    auto _str      = remove_space(str);
    _str           = _str.substr(1, _str.size() - 2);
    std::stringstream sstream{_str};
    std::string buff;
    while (getline(sstream, buff, delimiter)) {
      vec.push_back(stod(buff));
    }
  }
  void _read_elements(std::string str, std::vector<long long>& vec) {
    char delimiter = ',';
    auto _str      = remove_space(str);
    _str           = _str.substr(1, _str.size() - 2);
    std::stringstream sstream{_str};
    std::string buff;
    while (getline(sstream, buff, delimiter)) {
      vec.push_back(stoll(buff));
    }
  }

  void _read_elements(std::string str, std::vector<std::string>& vec) {
    char delimiter = ',';
    auto _str      = remove_space(str);
    _str           = _str.substr(1, _str.size() - 2);
    std::stringstream sstream{_str};
    std::string buff;
    while (getline(sstream, buff, delimiter)) {
      if (buff[0] == '"' && buff.back() == '"' && buff.size() >= 2) {
        buff = buff.substr(1, buff.size() - 2);
        vec.push_back(buff);
      }
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

  std::vector<std::string> get_key_list() {
    std::vector<std::string> ret;
    for (auto& x : lines) {
      ret.push_back(x.first);
    }
    return ret;
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

enum class LineType {
  Section,
  Line,
  Blank,
  Comment,
};

struct ConfigBase {
private:
  std::filesystem::path file_path;
  std::map<std::string, Section> body;

  void make_sections() {
    auto sections = parse_conf_file();
    for (auto& section : sections) {
      body[section.first] = Section(section.second);
    }
  }

  std::vector<std::pair<std::string, std::vector<std::string>>>
  parse_conf_file() {
    std::vector<std::pair<std::string, std::vector<std::string>>> ret;
    std::ifstream ifs(file_path);
    if (!ifs)
      throw(std::invalid_argument("File open failed"));

    std::string buff;
    std::string section_name = "";
    std::vector<std::string> section_lines;

    while (getline(ifs, buff)) {
      auto type = judge_line_type(buff);
      if (type == LineType::Section) {
        if (!section_name.empty()) {
          ret.push_back({section_name, section_lines});
        }
        section_name = remove_space(buff);
        section_name = section_name.substr(1, section_name.size() - 2);
      }
      else if (type == LineType::Line) {
        section_lines.push_back(buff);
      }
    }
    if (!section_name.empty()) {
      ret.push_back({section_name, section_lines});
    }

    ifs.close();
    return ret;
  }

  enum LineType judge_line_type(std::string& str) {
    auto _str = remove_space(str);
    if (_str[0] == '[' && _str.back() == ']') {
      return LineType::Section;
    }
    else if (_str.size() == 0) {
      return LineType::Blank;
    }
    else {
      return LineType::Line;
    }
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

public:
  ConfigBase(std::string& _file_path) : file_path(_file_path) {
    make_sections();
  }
  ConfigBase(std::filesystem::path& _file_path) : file_path(_file_path) {
    make_sections();
  }
  ConfigBase() : file_path(default_config_file_path) {
    make_sections();
  }

  std::vector<std::string> get_section_list() {
    std::vector<std::string> list;
    for (auto& x : body)
      list.push_back(x.first);
    return list;
  }

  const Section& operator[](std::string _str) const& {
    return body.at(_str);
  }
  Section& operator[](std::string _str) & {
    return body.at(_str);
  }
  Section operator[](std::string _str) const&& {
    return body.at(_str);
  }
};
} // namespace jinja::config