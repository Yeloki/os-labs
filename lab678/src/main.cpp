#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <zmq.hpp>

#include <csignal>
#include <zmq_addon.hpp>

// void PublisherThread(zmq::context_t *ctx) {
//   //  Prepare publisher
//
//   publisher.bind("inproc://#1");
//
//   // Give the subscribers a chance to connect, so they don't lose any
//   messages std::this_thread::sleep_for(std::chrono::milliseconds(20));
//
//   while (true) {
//     //  Write three messages, each with an envelope and content
//     publisher.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
//     publisher.send(zmq::str_buffer("Message in A envelope"));
//     publisher.send(zmq::str_buffer("B"), zmq::send_flags::sndmore);
//     publisher.send(zmq::str_buffer("Message in B envelope"));
//     publisher.send(zmq::str_buffer("C"), zmq::send_flags::sndmore);
//     publisher.send(zmq::str_buffer("Message in C envelope"));
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
//   }
// }
//
// void SubscriberThread1(zmq::context_t *ctx) {
//   //  Prepare subscriber
//   zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
//   subscriber.connect("inproc://#1");
//
//   //  Thread2 opens "A" and "B" envelopes
//   subscriber.set(zmq::sockopt::subscribe, "A");
//   subscriber.set(zmq::sockopt::subscribe, "B");
//
//   while (1) {
//     // Receive all parts of the message
//     std::vector<zmq::message_t> recv_msgs;
//     zmq::recv_result_t result =
//         zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
//     assert(result && "recv failed");
//     assert(*result == 2);
//
//     std::cout << "Thread2: [" << recv_msgs[0].to_string() << "] "
//               << recv_msgs[1].to_string() << std::endl;
//   }
// }
//
// void SubscriberThread2(zmq::context_t *ctx) {
//   //  Prepare our context and subscriber
//   zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
//   subscriber.connect("inproc://#1");
//
//   //  Thread3 opens ALL envelopes
//   subscriber.set(zmq::sockopt::subscribe, "");
//
//   while (1) {
//     // Receive all parts of the message
//     std::vector<zmq::message_t> recv_msgs;
//     zmq::recv_result_t result =
//         zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
//     assert(result && "recv failed");
//     assert(*result == 2);
//
//     std::cout << "Thread3: [" << recv_msgs[0].to_string() << "] "
//               << recv_msgs[1].to_string() << std::endl;
//   }
// }

///
/// possible commands:
/// 1 - ping id
/// 2 - create id -1
/// 2 - create id root_id
/// 3 - exec id N <...numbers...>
/// 4 - remove id
///

std::string BuildSendData(int command, std::optional<int> id,
                          std::optional<int> lid,
                          std::optional<std::vector<int>> data,
                          std::optional<int> socs) {
  if (command == 1) {

    return "ping " + std::to_string(id.value());
  } else if (command == 2) {

    return "create " + std::to_string(id.value()) + " " +
           std::to_string(lid.value()) + " " + std::to_string(socs.value());
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

int TopologyFinder(
    int id, int lid,
    const std::unordered_map<int, std::unordered_set<int>> &top) {
  for (const auto &[k, v] : top) {
    if (k == id || v.contains(id))
      return -2; // bad id
  }
  for (const auto &[k, v] : top) {
    if (k == lid || v.contains(lid)) {
      return k;
    }
  }
  return -1; // bad lid
}

void CreateNode(int id, int lid, int socs) {}

int main() {
  std::unordered_map<int, std::unordered_set<int>> topology;
  std::unordered_map<int, std::string> id_to_socs;
  int socs = 4041;

  zmq::context_t ctx(0);
  zmq::socket_t publisher(ctx, zmq::socket_type::pub);

  std::string command;

  while (std::cin >> command) {

    if (command == "create") {
      int id, lid;
      std::cin >> id >> lid;
      if (lid != -1) {
        const auto nid = TopologyFinder(id, lid, topology);

        if (nid == -1) {
          std::cout << "Bad id" << std::endl;
          continue;
        } else if (nid == -2) {
          std::cout << "Bad lid" << std::endl;
          continue;
        }
        zmq::message_t msg(BuildSendData(2, id, lid, {}, socs++));
        publisher.bind("tcp://*:" + id_to_socs[nid]);
        publisher.send(msg);
      } else {
        const auto nid = TopologyFinder(id, lid, topology);
        if (nid == -1) {
          std::cout << "Bad id" << std::endl;
          continue;
        }
        id_to_socs[id] = std::to_string(socs);
        CreateNode(id, lid, socs++);
      }

    } else if (command == "ping") {
      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -1) {
        std::cout << "Bad id" << std::endl;
        continue;
      }

    } else if (command == "exec") {
      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -1) {
        std::cout << "Bad id" << std::endl;
        continue;
      }
      int n;
      std::cin >> n;
      std::vector<int> data(n);
      for (auto &v : data)
        std::cin >> v;
      zmq::message_t msg(BuildSendData(3, id, {}, data, {}));
      publisher.bind("tcp://*:" + id_to_socs[nid]);
      publisher.send(msg);

    } else if (command == "remove") {
      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -1) {
        std::cout << "Bad id" << std::endl;
        continue;
      }


      if (topology.contains(nid)) {
        topology.erase(nid);
      }
    }
  }

  zmq::socket_t subscriber(ctx, zmq::socket_type::sub);
}