#ifndef APP_H
#define APP_H

#include "Hal.h"

enum AppChangeType
{
  NONE,
  MENU,
  CUSTOM
};

class App
{
public:
  virtual void tick(Hal* hal) = 0;

  virtual AppChangeType appChangeNeeded() = 0;
  virtual App* getCustomApp() = 0;
};

#endif