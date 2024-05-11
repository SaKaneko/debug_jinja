#include "../src/util/hook_lacker.hpp"
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
  count_plus(&st, 5);
  std::cout << st.count << std::endl;

  util::hook::hook_type sample_hook1(
      5, std::make_shared<std::function<void(state*, int)>>(
             [](state* st, int x) { st->count *= x; }));
  sample_hook1.exec(&st, 4);
  util::hook::hook_type sample_hook2(
      2.5, std::make_shared<std::function<void(state*, int)>>(count_plus));
  util::hook::hook_lacker lacker(sample_hook1);
  lacker.add_hook(sample_hook2);
  lacker.sort();
  lacker.each_exec(&st, 3);

  std::cout << st.count << std::endl;
  return 0;
}