#include <iostream>

int main(void)
{
  unsigned int c(3);
  auto lambda_func = [c](this auto& f) -> void {
    std::cout << "Entered function " << __FUNCTION__ << std::endl;
    if (c) {
      std::cout << "c is " << c << std::endl;
      --c;
      f();
    }
  };

  lambda_func();

  return 0;
}
