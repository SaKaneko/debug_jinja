#pragma once
#include <iostream>

namespace util::hook {
template <typename Func_T>
struct hook_type {
public:
  hook_type(float _priority, std::shared_ptr<Func_T> _func)
      : priority(_priority), func(_func) {}

  template <class... Args>
  void exec(Args... args) {
    (*func)(args...);
  }

  // for sort
  bool operator<(const hook_type& another) const {
    return priority < another.priority;
  }

private:
  float priority;
  std::shared_ptr<Func_T> func;
  std::string hook_name;
};
} // namespace util::hook