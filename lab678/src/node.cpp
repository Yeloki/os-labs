#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <zmq.hpp>

#include <boost/lexical_cast.hpp>
#include <csignal>
#include <zmq_addon.hpp>

std::string BuildSendData(int command, std::optional<int> id,
                          std::optional<int> lid,
                          std::optional<std::vector<int>> data) {
  if (command == 1) {

    return "ping " + std::to_string(id.value());
  } else if (command == 2) {

    return "create " + std::to_string(id.value()) + " " +
           std::to_string(lid.value());
  } else if (command == 3) {
    std::stringstream ss;

    ss << "exec " << id.value() << " ";
    ss << data.value().size() << " ";
    for (size_t i(0); i < data.value().size(); ++i) {
      ss << data.value()[i];
      if (i != data->size() - 1)
        ss << " ";
    }

    return ss.str();
  }
  return "remove " + std::to_string(id.value());
}

void CreateNode(int id, int lid) {
  auto pid = fork();
  if (pid == 0) {
    execl("./lab-678-worker", "lab-678-worker", std::to_string(id).c_str(),
          NULL);
  }
  std::cout << "[OK ON CREATE NEW NODE, PID -> " << pid << "]" << std::endl;
}

int main(int argc, char **argv) {
  const auto kid = argv[1];
  zmq::context_t ctx;
  zmq::socket_t subscriber(ctx, zmq::socket_type::sub);
  zmq::socket_t publisher(ctx, zmq::socket_type::pub);

  publisher.bind("inproc://#1");
  const std::string last_endpoint =
      publisher.get(zmq::sockopt::last_endpoint);
  std::cout << "Connecting to "
            << last_endpoint << std::endl;
  subscriber.connect(last_endpoint);
//  subscriber.connect("tcp://127.0.0.1:*");
  std::string next_id;
  std::cout << kid << std::endl;
  subscriber.set(zmq::sockopt::subscribe, kid);

  while (true) {
    // Receive all parts of the message
    std::vector<zmq::message_t> recv_msgs;
    zmq::recv_result_t result =
        zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
    if (result && *result == 2) {
      std::stringstream si(recv_msgs[1].to_string());
      std::cin.rdbuf(si.rdbuf());
      ////////////////////////////////////////////////////////
      std::string command;

      while (std::cin >> command) {
        if (command == "create") {
          int id, lid;
          std::cin >> id >> lid;
          if (std::to_string(id) != kid) {
            zmq::message_t msg(BuildSendData(2, id, lid, {}));
            publisher.send(zmq::message_t(next_id), zmq::send_flags::sndmore);
            publisher.send(msg);
          } else {
            CreateNode(id, lid);
          }
        } else if (command == "ping") {
          int id;
          std::cin >> id;
          if (std::to_string(id) != kid) {
            zmq::message_t msg(BuildSendData(1, id, {}, {}));
            publisher.send(zmq::message_t(next_id), zmq::send_flags::sndmore);
            publisher.send(msg);
          } else {
            zmq::message_t msg("[OK ON PING " + std::string(kid) + "]");
            publisher.send(zmq::message_t("-1"), zmq::send_flags::sndmore);
            publisher.send(msg);
          }
        } else if (command == "exec") {
          int id;
          std::cin >> id;
          int n;
          std::cin >> n;
          std::vector<int> data(n);
          for (auto &v : data)
            std::cin >> v;
          if (std::to_string(id) != kid) {
            zmq::message_t msg(BuildSendData(3, id, {}, data));
            publisher.send(zmq::message_t(next_id), zmq::send_flags::sndmore);
            publisher.send(msg);
          } else {
            int res(0);
            for (const auto &v : data)
              res += v;
            zmq::message_t msg("[OK ON EXEC NODE ID " + std::string(kid) +
                               ", RESULT " + std::to_string(res) + " ]");
            publisher.send(zmq::message_t("-1"), zmq::send_flags::sndmore);
            publisher.send(msg);
          }
        } else if (command == "remove") {
          int id;
          std::cin >> id;
          if (std::to_string(id) != kid) {
            zmq::message_t msg(BuildSendData(4, id, {}, {}));
            publisher.send(zmq::message_t(next_id), zmq::send_flags::sndmore);
            publisher.send(msg);
          } else {
            if (!next_id.empty()) {
              zmq::message_t msg(BuildSendData(4, id, {}, {}));
              publisher.send(zmq::message_t(next_id), zmq::send_flags::sndmore);
              publisher.send(msg);
            }
            return 0;
          }
        }
      }
    }
  }
}