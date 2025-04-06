#ifndef SETTINGS_H
#define SETTINGS_H

namespace vgs
{

class SettingsItem
{
friend class Settings;
public:
  SettingsItem(const char* name, int numValues, const char* const* values) : m_numValues(numValues)
  {
    m_name = new char[strlenImpl(name) + 1];
    strcpyImpl(m_name, name);

    m_values = new char*[numValues];

    for(int i=0; i<numValues; i++)
    {
      m_values[i] = new char[strlenImpl(values[i]) + 1];
      strcpyImpl(m_values[i], values[i]);
    }

    m_counter = 0;
  }

  ~SettingsItem()
  {
    delete[] m_name;
    for(int i=0; i<m_numValues; i++)
    {
      delete[] m_values[i];
    }
    delete[] m_values;

  }

  const char* getName() const
  {
    return m_name;
  }

  int getCounter() const
  {
    return m_counter;
  }

  const char* getValue() const
  {
    return m_values[m_counter];
  }

  void increment()
  {
    m_counter++;
    if(m_counter >= m_numValues)
    {
      m_counter = 0;
    }
  }

  void decrement()
  {
    m_counter--;
    if(m_counter < 0)
    {
      m_counter = m_numValues - 1;
    }
  }

  bool set(int value)
  {
    if(value >= 0 && value < m_numValues)
    {
      m_counter = value;
      return true;
    }
    return false;
  }

  SettingsItem* getPrevious()
  {
    return m_previous;
  }

  SettingsItem* getNext()
  {
    return m_next;
  }

private:
  int strlenImpl(const char* str)
  {
    int counter = 0;

    while(str[counter] != '\0')
    {
      counter++;
    }

    return counter;
  }

  void strcpyImpl(char* dst, const char* src)
  {
    int counter = 0;

    while(src[counter] != '\0')
    {
      dst[counter] = src[counter];
      counter++;
    }

    dst[counter] = src[counter]; // copy '\0'
  }

private:
  char* m_name;
  int m_numValues;
  char** m_values;
  int m_counter;

  SettingsItem* m_previous = nullptr;
  SettingsItem* m_next = nullptr;
  
};

class Settings
{
public:

  ~Settings()
  {
    SettingsItem* item = m_firstItem;
    while(item)
    {
      SettingsItem* temp = item->getNext();
      delete item;
      item = temp;
    }
  }

  void addItem(const char* name, int numValues, const char* const* values)
  {
    m_numItems++;

    SettingsItem* item = new SettingsItem(name, numValues, values);
    
    if(m_numItems == 1)
    {
      m_currentItem = item;
      m_firstItem = item;
    }
    else
    {
      m_lastItem->m_next = item;
      item->m_previous = m_lastItem;
    }

    m_lastItem = item;
  }

  void next()
  {
    if(m_numItems == 0)
    {
      return;
    }
    if(m_currentItem->getNext())
    {
      m_currentItem = m_currentItem->getNext();
      m_currentItemIndex++;
    }
    else
    {
      m_currentItem = m_firstItem;
      m_currentItemIndex = 0;
    }
  }

  void previous()
  {
    if(m_numItems == 0)
    {
      return;
    }
    if(m_currentItem->getPrevious())
    {
      m_currentItem = m_currentItem->getPrevious();
      m_currentItemIndex--;
    }
    else
    {
      m_currentItem = m_lastItem;
      m_currentItemIndex = m_numItems - 1;
    }
  }

  SettingsItem* getItem(int index) const
  {
    if(index >= m_numItems)
    {
      return nullptr;
    }
    SettingsItem* item = m_firstItem;
    for(int i=0; i<index; i++)
    {
      item = item->getNext();
    }
    return item;
  }

  SettingsItem* getCurrentItem() const
  {
    return m_currentItem;
  }

  int getCurrentItemIndex() const
  {
    return m_currentItemIndex;
  }

  int size() const
  {
    return m_numItems;
  }

  void dumpData(int* data) const
  {
    SettingsItem* item = m_firstItem;
    for(int i=0; i<m_numItems; i++)
    {
      data[i] = item->getCounter();
      item = item->getNext();
    }
  }

  void loadData(const int* data)
  {
    SettingsItem* item = m_firstItem;
    for(int i=0; i<m_numItems; i++)
    {
      item->set(data[i]);
      item = item->getNext();
    }
  }

private:
  int m_numItems = 0;
  int m_currentItemIndex = 0; 

  SettingsItem* m_firstItem = nullptr;
  SettingsItem* m_lastItem = nullptr;
  SettingsItem* m_currentItem = nullptr;

};

} //namespace vgs

#endif
