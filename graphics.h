#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <TFT_eSPI.h>
#include <OpenFontRender.h>

#include "State.h"

#define COMMON_BACKGROUND_COLOR 0x0006
#define COMMON_CONTENT_COLOR 0xFFFF

typedef struct bounds
{
  uint left;
  uint top;
  uint right;
  uint bottom;
} bounds_t;

typedef struct coordinates
{
  uint x;
  uint y;
  uint w;
  uint h;
} coordinates_t;

class Element
{
public:
  Element(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates);

  coordinates_t getCoordinates();
  bounds_t getBounds();

  virtual void show(bool force) = 0;

  void setHidden(bool hidden);

  void setBackgroundColor(uint32_t color);
  void setBorderColor(uint32_t color);
  void setBorderRadius(int32_t radius);

  void setUpdateCallback(void (*updateCallback)(const State&, Element*));
  void setOnPressCallback(void (*onPressCallback)(State&, Element*));
  void setOnClickCallback(void (*onClickCallback)(State&, Element*));
  void setOnHoldCallback(void (*onHoldCallback)(State&, Element*));

  void update(const State& state);
  void onPress(State& state, uint x, uint y);
  void onClick(State& state, uint x, uint y);
  void onHold(State& state, uint x, uint y);

protected:
  bool checkHit(uint x, uint y);

protected:
  coordinates_t m_coordinates;

  uint32_t m_backgroundColor = COMMON_BACKGROUND_COLOR;
  uint32_t m_borderColor = COMMON_BACKGROUND_COLOR;
  int32_t m_borderRadius = 0;

  bool m_hidden;
  bool m_dirty;
  
  TFT_eSPI* m_tft;
  OpenFontRender* m_fontRender;

  void (*m_updateCallback)(const State&, Element*);
  void (*m_onPress)(State&, Element*);
  void (*m_onClick)(State&, Element*);
  void (*m_onHold)(State&, Element*);
};

class TextElement : public Element
{
public:
  TextElement(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates);

  void setText(const String& text);
  void setTextColor(uint32_t color);
  void setFontSize(uint size);

  void show(bool force) override;

protected:
  uint32_t m_textColor = COMMON_CONTENT_COLOR;
  uint m_fontSize = 14;
  String m_text;
};

#endif