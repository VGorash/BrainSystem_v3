#ifndef TIMER_H
#define TIMER_H

#include "Core/Hal.h"

namespace vgs
{

class Timer
{
public:
  Timer(unsigned long time = 1000, bool periodMode = false) : m_started(false), m_periodMode(periodMode), m_time(time), m_startTime(0){}
  
  void setTime(unsigned long time)
  {
    m_time = time;
  }

  void setPeriodMode(bool mode)
  {
    m_periodMode = mode;
  }

  void start(Hal* hal)
  {
    m_started = true;
    m_startTime = hal->getTimeMillis();
  }

  void stop()
  {
    m_started = false;
  }

  bool isStarted()
  {
    return m_started;
  }

  bool tick(Hal* hal)
  {
    if(!m_started)
    {
      return false;
    }

    unsigned long currentTime = hal->getTimeMillis();

    if(currentTime - m_startTime >= m_time)
    {
      if(m_periodMode)
      {
        m_startTime = currentTime;
      }
      else
      {
        m_started = false;
      }
      return true;
    }
    return false;
  }


private:
  bool m_started;
  bool m_periodMode;
  unsigned long m_startTime;
  unsigned long m_time;
};

} // namespace vgs

#endif
