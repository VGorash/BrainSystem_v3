#ifndef APP_H
#define APP_H

#include "Hal.h"

namespace vgs
{

enum class AppChangeType
{
  None,
  Menu,
  Custom
};

class App
{
public:
  virtual void init(Hal* hal) = 0;
  virtual void tick(Hal* hal) = 0;

  virtual AppChangeType appChangeNeeded() = 0;
  virtual App* getCustomApp() = 0;
};

} // namespace vgs

#endif
