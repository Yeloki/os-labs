#include <iostream>

enum class CommandType {
  create_new_node,
  create_new_node_group,
  ping,
  remove_node,

  exec
};

struct Command {
  CommandType type;

};


Command GetCommand() {
  std::string command;
  std::cin >> command;

}

int main() {
  // create ping remove exec

  return 0;
}