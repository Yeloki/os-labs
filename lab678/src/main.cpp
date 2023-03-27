#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <zmq.hpp>
#include <boost/random.hpp>
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

int TopologyFinder(
    int id, int lid,
    const std::unordered_map<int, std::unordered_set<int>> &top) {
  for (const auto &[k, v] : top) {
    if (k == id || v.contains(id))
      return -2; // bad id
  }
  for (const auto &[k, v] : top) {
    if (k == lid || v.contains(lid) || v.contains(id)) {
      return k;
    }
  }
  if (lid == -1) {
    return id;
  }
  return -1; // bad lid
}

void CreateNode(int _id, int _lid, zmq::context_t *ctx) {
  auto pid = fork();
  if (pid == 0) {
    const auto kid = std::to_string(_id);
    boost::mt19937 rng(time(0));
    zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
    zmq::socket_t publisher(*ctx, zmq::socket_type::pub);

    publisher.bind("inproc://#1");
    const std::string last_endpoint =
        publisher.get(zmq::sockopt::last_endpoint);
    std::cout << "Connecting to " << last_endpoint << std::endl;
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
              CreateNode(id, rng(), ctx);
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
              publisher.send(zmq::message_t(std::to_string(_lid)), zmq::send_flags::sndmore);
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
              publisher.send(zmq::message_t(std::to_string(_lid)), zmq::send_flags::sndmore);
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
                publisher.send(zmq::message_t(next_id),
                               zmq::send_flags::sndmore);
                publisher.send(msg);
              }
              exit(0);
            }
          }
        }
      }
    }
  }
  std::cout << "[OK ON CREATE NEW NODE, PID -> " << pid << "]" << std::endl;
}

int main() {
  std::unordered_map<int, std::unordered_set<int>> topology;
  std::unordered_map<int, int> inproc_uids;
  zmq::context_t ctx;
  zmq::socket_t publisher(ctx, zmq::socket_type::pub);
  zmq::socket_t subscriber(ctx, zmq::socket_type::sub);
  boost::mt19937 rng(time(0));
  publisher.bind("inproc://#1");
  const std::string last_endpoint = publisher.get(zmq::sockopt::last_endpoint);
  std::cout << "Connecting to " << last_endpoint << std::endl;
  subscriber.connect(last_endpoint);
//  subscriber.set(zmq::sockopt::subscribe, "-1");
  std::string command;

  while (std::cin >> command) {

    if (command == "create") {
      int id, lid;
      std::cin >> id >> lid;
      if (lid != -1) {
        const auto nid = TopologyFinder(id, lid, topology);

        if (nid == -2) {
          std::cout << "Bad id" << std::endl;
          continue;
        } else if (nid == -1) {
          std::cout << "Bad lid" << std::endl;
          continue;
        }
        inproc_uids[id] = rng();
        subscriber.set(zmq::sockopt::subscribe,
                       std::to_string(inproc_uids[id]));
        zmq::message_t msg(BuildSendData(2, id, inproc_uids[id], {}));
        publisher.send(zmq::message_t(std::to_string(nid)),
                       zmq::send_flags::sndmore);
        publisher.send(msg);

        bool flag = true;
        auto start = time(nullptr);
        while (time(nullptr) - start < 1) {
          // Receive all parts of the message
          std::vector<zmq::message_t> recv_msgs;
          zmq::recv_result_t result =
              zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));

          if (result && *result == 2) {
            flag = false;
            std::cout << recv_msgs[1].to_string() << std::endl;
          }
        }
        if (flag) {
          std::cout << "[FAIL ON CREATE NODE]";
        }

      } else {
        const auto nid = TopologyFinder(id, lid, topology);
        if (nid == -2) {
          std::cout << "Bad id" << std::endl;
          continue;
        }
        inproc_uids[id] = rng();
        CreateNode(id, inproc_uids[id], &ctx);
      }

    } else if (command == "ping") {
      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -2) {
        std::cout << "Bad id" << std::endl;
        continue;
      }
      subscriber.set(zmq::sockopt::subscribe, std::to_string(inproc_uids[id]));
      zmq::message_t msg(BuildSendData(1, id, {}, {}));
      publisher.send(zmq::message_t(std::to_string(nid)),
                     zmq::send_flags::sndmore);
      publisher.send(msg);

      bool flag = true;
      auto start = time(nullptr);
      while (time(nullptr) - start < 1) {
        // Receive all parts of the message
        std::vector<zmq::message_t> recv_msgs;
        zmq::recv_result_t result =
            zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));

        if (result && *result == 2) {
          flag = false;
          std::cout << recv_msgs[1].to_string() << std::endl;
        }
      }
      if (flag) {
        std::cout << "[FAIL ON PING NODE]" << std::endl;
      }

    } else if (command == "exec") {
      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -2) {
        std::cout << "Bad id" << std::endl;
        continue;
      }
      int n;
      std::cin >> n;
      std::vector<int> data(n);
      for (auto &v : data)
        std::cin >> v;
      subscriber.set(zmq::sockopt::subscribe, std::to_string(inproc_uids[id]));
      zmq::message_t msg(BuildSendData(3, id, {}, data));
      publisher.send(zmq::message_t(std::to_string(nid)),
                     zmq::send_flags::sndmore);
      publisher.send(msg);
      bool flag = true;
      auto start = time(nullptr);
      while (time(nullptr) - start < 1) {
        // Receive all parts of the message
        std::vector<zmq::message_t> recv_msgs;
        zmq::recv_result_t result =
            zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));

        if (result && *result == 2) {
          flag = false;
          std::cout << recv_msgs[1].to_string() << std::endl;
        }
      }
      if (flag) {
        std::cout << "[FAIL ON EXEC NODE]" << std::endl;
      }
    } else if (command == "remove") {

      int id;
      std::cin >> id;
      const auto nid = TopologyFinder(id, -1, topology);

      if (nid == -2) {
        std::cout << "Bad id" << std::endl;
        continue;
      }

      zmq::message_t msg(BuildSendData(4, id, {}, {}));
      publisher.send(zmq::message_t(std::to_string(nid)),
                     zmq::send_flags::sndmore);
      publisher.send(msg);

      if (topology.contains(nid)) {
        topology.erase(nid);
      }
    }
  }
}