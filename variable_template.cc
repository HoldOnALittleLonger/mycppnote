#include<iostream>
#include<vector>
#include<cstddef>

template<class _CType>
std::vector<_CType> vec;

int main(void)
{
  std::vector<int> *pVec1 = &vec<int>;
  std::vector<int> *pVec2 = &vec<int>;

  std::cout<<static_cast<const void *>(pVec1)<<std::endl
	   <<static_cast<const void *>(pVec2)<<std::endl;

  return 0;
}
