#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace common {

  struct Args {
    std::unordered_map<char, bool> flags;
    std::unordered_map<char, int> int_values;
    std::unordered_map<char, std::string> string_values;
  };

  class ArgsParser {
  public:
    enum class ArgType {
      flag,
      int_value,
      string_value
    };

    void Parse();

    void AddParserFlag(char flag,
                       ArgType type,
                       const std::optional<std::string> &description = {},
                       const std::optional<std::string> &full_name = {});
    void AddHelpText(std::string& text);
  private:
    std::string _help;
    std::unordered_map<char, ArgType> _flags;
    std::unordered_map<char, std::string> _flags_to_full_name;
    std::unordered_map<char, std::string> _flags_to_description;
  };

} // common

