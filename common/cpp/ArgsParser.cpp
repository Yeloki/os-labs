#include "ArgsParser.hpp"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace common {

void ArgsParser::AddParserFlag(char arg_name, ArgsParser::ArgType type,
                               bool required,
                               const std::optional<std::string> &description,
                               const std::optional<std::string> &full_name,
                               const std::optional<std::any> &default_val) {
  Arg arg;

  arg.is_required = required;
  arg.description = description;
  arg.full_name = full_name;
  arg.default_value = default_val;
  arg.type = type;
  args_[arg_name] = arg;
}

void ArgsParser::AddHelpText(std::string &text) { help_ = std::move(text); }

void ArgsParser::Parse(int argc, char **argv) {
  common::Args args;

  for (int i(1); i < argc; ++i) {
    const auto &key = std::string(argv[i]);

    if (key.size() < 2) {
      throw common::ParseError("Cant parse arg, length less than 2");
    }

    char arg = (key.size() > 2) ? FindArg(key) : key[1];
    if (args_.find(arg) == args_.end()) {
      throw common::ParseError(boost::lexical_cast<std::string>(
          boost::format("Unknown arg %1%") % arg));
    }

    switch (args_[arg].type) {
    case ArgType::flag:
      args.flags[arg] = true;
      break;
    case ArgType::int_value:
      break;
    case ArgType::string_value:
      break;
    }
  }
}

char ArgsParser::FindArg(const std::string &key) {
  for (const auto &[k, v] : args_) {
    if (v.full_name.has_value() && v.full_name == key) {
      return k;
    }
  }
  throw common::ParseError(boost::lexical_cast<std::string>(
      boost::format("Cant parse arg %1%") % key));
}

} // namespace common