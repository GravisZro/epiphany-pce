#include <global.h>
#include "6280core.h"

int main(void)
{
  static HuC6280::Core core;
  core.Execute();
//  HuC6280::Core::Instance().Execute();


  return 0;
}
