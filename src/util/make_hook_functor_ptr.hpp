#pragma once
#include <iostream>
#include <functional>
#include <memory>
namespace util::hook {
template <typename functor_type>
std::function<std::shared_ptr<std::function<functor_type>>(functor_type)>
make_hook_functor_ptr_creator() {
  std::function<std::shared_ptr<std::function<functor_type>>(functor_type func)>
      f([](functor_type func) {
        return std::make_shared<std::function<functor_type>>(func);
      });
  return f;
}
} // namespace util::hook