#include "../src/util/hook_lacker.hpp"
#include "../src/util/make_hook_functor_ptr.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <string>

struct state {
  int count = 0;
  std::string str;
};

void count_plus(state* st, int x) {
  st->count += x;
}

int main() {
  state st;

  auto creator = util::hook::make_hook_functor_ptr_creator<void(state*, int)>();
  util::hook::hook_type hook_plus(6, creator(count_plus));
  util::hook::hook_type hook_multiple(
      7, creator([](state* st, int x) { st->count *= x; }));
  //   util::hook::hook_type sample_hook1(
  //       5, std::make_shared<std::function<void(state*, int)>>(
  //              [](state* st, int x) { st->count *= x; }));
  //   sample_hook1.exec(&st, 4);
  //   util::hook::hook_type sample_hook2(
  //       2.5, std::make_shared<std::function<void(state*, int)>>(count_plus));
  util::hook::hook_lacker lacker(hook_plus);
  lacker.add_hook(hook_multiple);
  lacker.sort();
  lacker.each_exec(&st, 5);

  std::cout << st.count << std::endl;
  return 0;
}