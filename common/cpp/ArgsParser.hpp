#pragma once

#include <any>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace common {

class ParseError : public std::runtime_error {
  using runtime_error::runtime_error;
};

struct Args {
  std::unordered_map<char, std::any> flags;
  std::unordered_map<char, int> int_values;
  std::unordered_map<char, std::string> string_values;
};

class ArgsParser {
public:
  enum class ArgType { flag, int_value, string_value };

  void Parse(int argc, char **argv);

  void AddParserFlag(char arg_name, ArgType type, bool required = false,
                     const std::optional<std::string> &description = {},
                     const std::optional<std::string> &full_name = {},
                     const std::optional<std::any> &default_val = {});

  void AddHelpText(std::string &text);

private:
  char FindArg(const std::string &key);
  struct Arg {
    bool is_required;
    ArgType type;
    std::optional<std::string> description;
    std::optional<std::string> full_name;
    std::any default_value;
  };

  std::string help_;
  std::unordered_map<char, Arg> args_;
  std::unordered_map<char, bool> is_required_;
};

} // namespace common
