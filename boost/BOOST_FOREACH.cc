#include <boost/foreach.hpp>
#include <iostream>
#include <vector>

int main(void)
{
  std::vector<int> nums{1, 2, 3, 4, 5, 6};

  for (auto i : nums) {
    std::cout << i << " ";
  }
  std::cout << "\n";

  BOOST_FOREACH(int index, nums) {
    std::cout << index * 2 << "\n";
  }

  return 0;
}
