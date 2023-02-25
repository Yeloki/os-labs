#include "ArgsParser.hpp"

namespace common {
  void ArgsParser::AddParserFlag(char flag,
                                 ArgsParser::ArgType type,
                                 const std::optional<std::string> &description,
                                 const std::optional<std::string> &full_name) {
    if (description.has_value()) {
      _flags_to_description[flag] = description.value();
    }

    if (full_name.has_value()) {
      _flags_to_full_name[flag] = full_name.value();
    }

    _flags[flag] = type;
  }
} // common