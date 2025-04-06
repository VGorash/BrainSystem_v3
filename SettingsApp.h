#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include "src/Framework/Core/App.h"
#include "src/Framework/Core/Settings.h"

namespace vgs
{

class SettingsApp : public App
{
public:
  SettingsApp(bool launchGame);

  void init(Hal* hal) override;
  void tick(Hal* hal) override;

  AppChangeType appChangeNeeded() override;
  App* getCustomApp() override;

private:

  void processIdle(Hal* hal);
  void processEdit(Hal* hal);

  void exit(Hal* hal);

private:
  bool m_launchGame;
  
  bool m_displayDirty = true;
  bool m_settingsDirty = false;
  bool m_shouldClose = false;
  bool m_editMode = false;

  Settings m_settings;

};

} // namespace vgs

#endif
