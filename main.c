#include "yield.h"

int main(void)
{
  // Init yield then, as other init tasks might require delays
  platform_yield_init();

  while (1)
  {
    platform_yield();
  }
}