#include <stack>
#include <cstdio>

int main() {
  std::stack <void*> stk;
  #define GOSUB(target) do { __label__ n; stk.push(&&n); goto target; n:; } while(0)
  #define RETURN()      do { void *r = stk.top(); stk.pop(); goto *r;     } while(0)

  std::printf("Main function\n");
  GOSUB(test1);

  std::printf("Still main function\n");
  GOSUB(test2);

  return 0;

  test1:
    std::printf("GOSUB test 1\n");
  RETURN();

  test2:
    std::printf("GOSUB test 2\n");
  RETURN();
}
