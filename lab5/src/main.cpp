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
  Sieve = (int64_t(*)(int64_t, int64_t))dlsym(handle2, "_Z5Sievell");
  Native = (int64_t(*)(int64_t, int64_t))dlsym(handle2, "_Z6Nativell");
  RectArea = (double (*)(double, double))dlsym(handle1, "_Z8RectAreadd");
  RightTriangleArea =
      (double (*)(double, double))dlsym(handle1, "_Z17RightTriangleAreadd");

  int64_t a, b;
  std::cin >> a >> b;
  std::cout << "Sieve " << Sieve(a, b) << std::endl;
  std::cout << "Native " << Native(a, b) << std::endl;
  double c, d;
  std::cin >> c >> d;
  std::cout << "RectArea " << RectArea(c, d) << std::endl;
  std::cout << "RightTriangleArea " << RightTriangleArea(c, d) << std::endl;
}
