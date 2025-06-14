#include "circular_buffer.h"
#include <cassert>
#include <vector>

auto main() -> int {
  circular_buffer<int, 1> b1;
  circular_buffer<int, 3> b2({1, 2, 3});
  circular_buffer<int, 3> b3(42);

  circular_buffer b({1, 2, 3, 4});
  assert(b.size() == 4);
  b.push_back(5);
  b.push_back(6);
  b.pop_front();

  assert(b.size() == 3);
  assert(b[0] == 4);
  assert(b[1] == 5);
  assert(b[2] == 6);

  static_assert(std::is_swappable_v<circular_buffer_iterator<int, 10>>);

  circular_buffer buf({1, 2, 3});
  std::vector<int> v;
  for (auto const e : buf) {
    v.push_back(e);
  }

  *buf.begin() = 0;
  assert(buf.front() == 0);
}