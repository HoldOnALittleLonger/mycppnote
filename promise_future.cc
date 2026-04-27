#include <future>
#include <print>
#include <cstddef>
#include <thread>

/**
 * we will let this function runs in another thread.
 * because std::promise forbids copying,thus we move it.
 * std::thread requires the arguments to the thread must
 * be invocable after converted to rvalues.
 */
void return_a_add_b(std::promise<int>&& p, int a, int b)
{
  p.set_value(a + b);
}

int main(void)
{
  std::promise<int> p;
  auto the_future = p.get_future();

  std::thread task(return_a_add_b, std::move(p), 2, 2);

  std::println("wait on future.");
  the_future.wait();
  std::println("future ready,get it.");
  std::println("result is {}", the_future.get());

  task.join();
  
  return 0;
}
