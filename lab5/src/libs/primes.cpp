#include "primes.hpp"

#include <vector>

namespace {

bool IsPrime(int64_t num) {
  for (int i(2); i * i <= num; ++i) {
    if (num % i == 0) {
      return false;
    }
  }
  return true;
}

} // namespace

extern "C" int64_t Sieve(int64_t a, int64_t b) {
  std::vector<uint8_t> v(b + 1, 1);
  int64_t res(0);
  for (int p(2); p <= b; ++p) {
    if (v[p]) {
      res += (a <= p) && (p <= b);
      for (int i(p + 1); i < v.size(); ++i) {
        if (i % p == 0) {
          v[i] = 0;
        }
      }
    }
  }
  return res;
}

extern "C" int64_t Native(int64_t a, int64_t b) {
  int64_t res(0);
  for (auto num(a); num <= b; ++num) {
    res += IsPrime(num);
  }
  return res;
}
