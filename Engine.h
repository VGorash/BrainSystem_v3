#ifndef ENGINE_H
#define ENGINE_H

#include "Hal.h"
#include "App.h"

class Engine
{
public:
  Engine(Hal* hal, App* (*initialApp)(), App* (*menuApp)()): m_hal(hal)
  {
    if (menuApp)
    {
      m_menuApp = menuApp();
    }
    else
    {
      m_menuApp = nullptr;
    }

    if(initialApp)
    {
      m_app = initialApp();
    }
    else
    {
      m_app = m_menuApp;
    }
    
  };

  void tick()
  {
    m_hal->tick();
    
    m_app->tick(m_hal);

    switch(m_app->appChangeNeeded())
    {
      case AppChangeType::CUSTOM:
      {
        App* temp = m_app;
        m_app = m_app->getCustomApp();
        if(temp != m_menuApp)
        {
          delete temp;
        }
        break;
      }
      case AppChangeType::MENU:
      {
        if(!m_menuApp)
        {
          break;
        }
        App* temp = m_app;
        m_app = m_menuApp;
        if(temp != m_menuApp)
        {
          delete temp;
        }
        break;
      }
    }
  };

private:
  Hal* m_hal;
  App* m_app;
  App* m_menuApp;
};

#endif