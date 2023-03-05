#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <set>
#include <stdexcept>

#include <atomic>
#include <memory>

// number of primes between 2 and x:
//             10 =>           4
//            100 =>          25
//          1,000 =>         168
//         10,000 =>       1,229
//        100,000 =>       9,592
//      1,000,000 =>      78,498
//     10,000,000 =>     664,579
//    100,000,000 =>   5,761,455
//  1,000,000,000 =>  50,847,534
// 10,000,000,000 => 455,052,511

struct Settings {
  int threads_count = 4;
  int up_border = 1e5;
};

Settings ParseSettings(int argc, char **argv) {
  Settings settings;
  if (argc == 1) {
    return settings;
  }

  if (argc >= 3) {
    if (argv[1][1] == 't') {
      settings.threads_count = boost::lexical_cast<int>(argv[2]);
    } else if (argv[1][1] == 'n') {
      settings.up_border = boost::lexical_cast<int>(argv[2]) + 1;
    } else {
      throw std::runtime_error(
          "Wrong argument was provided, type -h for more info");
    }
  }

  if (argc == 5) {
    if (argv[3][1] == 't') {
      settings.threads_count = boost::lexical_cast<int>(argv[4]);
    } else if (argv[3][1] == 'n') {
      settings.up_border = boost::lexical_cast<int>(argv[4]) + 1;
    } else {
      throw std::runtime_error(
          "Wrong argument was provided, type -h for more info");
    }
  }
  return settings;
}

struct WorkerArgs {
  int64_t prime;
  size_t lower_bound;
  size_t upper_bound;
  uint8_t *bitmask;
  std::function<size_t(size_t)> to_index;
  std::function<void(uint8_t *, size_t)> modify;
};

void *Worker(WorkerArgs *args) {
  for (size_t i(args->lower_bound); i < args->upper_bound; ++i) {
    if (i % args->prime == 0) {
      args->modify(args->bitmask, args->to_index(i));
    }
  }
  delete args;
  pthread_exit(nullptr);
}

size_t ToIndex(size_t index) { return index - 2; }

void Modifier(uint8_t *arr, size_t index) { arr[index] = 0; }

int main(int argc, char **argv) {

  std::cout << "Args count: " << argc << std::endl;
  std::cout << "Args:" << std::endl;
  for (int i(0); i < argc; ++i) {
    std::cout << argv[i] << std::endl;
  }
  std::cout << "EndArgs" << std::endl;

  if (argc == 2 && argv[1][1] == 'h') {
    std::cout << "Usage: main -t [] -n []\n"
                 "t - count of threads\n"
                 "n - up border of prime search\n";
    return 0;
  }

  if (argc != 1 && argc != 3 && argc != 5) {
    throw std::runtime_error("Wrong args count");
  }
  const auto settings = ParseSettings(argc, argv);

  // from two to up_border (two extra values)

  auto *bitmask = new uint8_t[settings.up_border];
  for (int i(0); i < settings.up_border; ++i) {
    bitmask[i] = 1;
  }
  std::ofstream fout("primes.txt");

  int primes_count = 0;
  std::vector<std::pair<pthread_t, pthread_attr_t *>> workers;

  for (int prime(2); prime <= settings.up_border; ++prime) {

    if (bitmask[ToIndex(prime)]) {
      workers.resize(settings.threads_count);
      fout << prime << std::endl;
      primes_count++;
      size_t cluster_size =
          (settings.up_border - prime) / settings.threads_count;

      for (int j(0); j < settings.threads_count; ++j) {

        const auto lower_bound =
            settings.up_border - (settings.threads_count - j) * cluster_size;
        const auto upper_bound =
            settings.up_border -
            (settings.threads_count - j - 1) * cluster_size;

        auto *args = new WorkerArgs(prime, lower_bound, upper_bound, bitmask,
                                    ToIndex, Modifier);

        auto *pthreadAttr = new pthread_attr_t;
        workers[j].second = pthreadAttr;
        pthread_attr_init(pthreadAttr);

        // With correct args mutex are extra, therefore I was not using it
        pthread_create(&workers[j].first, pthreadAttr,
                       reinterpret_cast<void *(*)(void *)>(Worker), args);
      }
      for (auto &[pid, pattrs] : workers) {
        pthread_join(pid, nullptr);
        delete pattrs;
      }
      workers.clear();
    }
  }
  std::cout << primes_count;
  return 0;
}
