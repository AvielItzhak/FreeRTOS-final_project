#include "hello.h"
#include "init.h"

int main(void) {

  // Always first initialise the main application.
  init_main();

  while (1) {
    hello();
  }

  return 0;
}
