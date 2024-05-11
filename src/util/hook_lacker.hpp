#pragma once

#include "hook_type.hpp"
#include <iostream>

namespace util::hook {
template <typename Func_T>
struct hook_lacker {
public:
  // this class needs at least 1 hook.
  hook_lacker(hook_type<Func_T>& hook) {
    this->add_hook(hook);
  }

  void sort() {
    std::sort(this->hook_vec.begin(), this->hook_vec.end());
  }

  void add_hook(hook_type<Func_T>& hook) {
    hook_vec.push_back(hook);
  }

  template <class... Args>
  void each_exec(Args... args) {
    for (auto& hook : hook_vec) {
      hook.exec(args...);
    }
  }

private:
  std::vector<hook_type<Func_T>> hook_vec;
};
} // namespace util::hook