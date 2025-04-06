#ifndef ENGINE_H
#define ENGINE_H

#include "Hal.h"
#include "App.h"

namespace vgs
{

class Engine
{
public:
  Engine(Hal* hal, App* (*initialApp)(), App* (*menuApp)()): m_hal(hal)
  {
    if (menuApp)
    {
      m_menuApp = menuApp();
      m_menuApp->init(m_hal);
    }
    else
    {
      m_menuApp = nullptr;
    }

    if(initialApp)
    {
      m_app = initialApp();
      m_app->init(m_hal);
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
      case AppChangeType::Custom:
      {
        App* temp = m_app;
        m_app = m_app->getCustomApp();
        m_app->init(m_hal);
        if(temp != m_menuApp)
        {
          delete temp;
        }
        break;
      }
      case AppChangeType::Menu:
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

} //namespace vgs

#endif
