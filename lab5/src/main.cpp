#include "libs/areas.hpp"
#include "libs/primes.hpp"

#include <bits/stdc++.h>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
int main() {

  void *handle1;
  void *handle2;
  handle1 = dlopen("./libareas_sh.so", RTLD_NOW);
  handle2 = dlopen("./libprimes_sh.so", RTLD_NOW);
  int64_t (*Sieve)(int64_t, int64_t);
  int64_t (*Native)(int64_t, int64_t);

  double (*RectArea)(double, double);
  double (*RightTriangleArea)(double, double);
  Sieve = (int64_t(*)(int64_t, int64_t))dlsym(handle2, "Sieve");
  Native = (int64_t(*)(int64_t, int64_t))dlsym(handle2, "Native");
  RectArea = (double (*)(double, double))dlsym(handle1, "RectArea");
  RightTriangleArea =
      (double (*)(double, double))dlsym(handle1, "RightTriangleArea");
  bool real(1);
  std::optional<double> a = {};
  std::optional<double> b = {};

  while (true) {
    double tmp;
    if (!a.has_value()) {
      std::cin >> tmp;
      a = tmp;
    } else {
      std::cin >> tmp;
      b = tmp;
    }

    if (a.has_value() && a.value() == 0) {
      a = {};
      real = !real;
    } else if (b.has_value() && b.value() == 0) {
      b = {};
      real = !real;
    }

    if (real) {
      if (a.has_value() && b.has_value()) {
        std::cout << "Sieve " << Sieve(a.value(), b.value()) << std::endl;
        std::cout << "RectArea " << RectArea(a.value(), b.value()) << std::endl;
        a = {};
        b = {};
      }
    } else {
      
      if (a.has_value() && b.has_value()) {
        std::cout << "Native " << Native(a.value(), b.value()) << std::endl;
        std::cout << "RightTriangleArea "
                  << RightTriangleArea(a.value(), b.value()) << std::endl;
        a = {};
        b = {};
      }
    }
  }
}