#pragma once
#include <../defines.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace yl {

void CreateThread(const std::string &filename) {
  const auto id_ = fork();

  if (id_ == 0) {
    auto exec_path = std::string("./");
    exec_path += filename;
    execl(exec_path.c_str(), filename.c_str(), NULL);
    perror("execl");
  }
}

} // namespace yl