#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <TFT_eSPI.h>
#include <OpenFontRender.h>

#include "DisplayState.h"

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

struct DisplayState;

class Element
{
public:
  Element(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates);

  coordinates_t getCoordinates();
  bounds_t getBounds();

  virtual void show(bool force, bool clearDirty = true);

  void setHidden(bool hidden);

  void setBackgroundColor(uint32_t color);
  void setBorderColor(uint32_t color);
  void setBorderRadius(int32_t radius);

  void setUpdateCallback(void (*updateCallback)(const DisplayState&, Element*));
  void setOnPressCallback(void (*onPressCallback)(DisplayState&, Element*));
  void setOnClickCallback(void (*onClickCallback)(DisplayState&, Element*));
  void setOnHoldCallback(void (*onHoldCallback)(DisplayState&, Element*));

  void update(const DisplayState& state);
  void onPress(DisplayState& state, uint x, uint y);
  void onClick(DisplayState& state, uint x, uint y);
  void onHold(DisplayState& state, uint x, uint y);

protected:
  bool checkHit(uint x, uint y);
  bool shouldUpdate(bool force);

protected:
  coordinates_t m_coordinates;

  uint32_t m_backgroundColor = COMMON_BACKGROUND_COLOR;
  uint32_t m_borderColor = COMMON_BACKGROUND_COLOR;
  int32_t m_borderRadius = 0;

  bool m_hidden;
  bool m_dirty;
  
  TFT_eSPI* m_tft;
  OpenFontRender* m_fontRender;

  void (*m_updateCallback)(const DisplayState&, Element*);
  void (*m_onPress)(DisplayState&, Element*);
  void (*m_onClick)(DisplayState&, Element*);
  void (*m_onHold)(DisplayState&, Element*);
};

class TextElement : public Element
{
public:
  TextElement(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates);

  void setText(const String& text);
  void setTextColor(uint32_t color);
  void setFontSize(uint size);

  void show(bool force, bool clearDirty = true) override;

protected:
  uint32_t m_textColor = COMMON_CONTENT_COLOR;
  uint m_fontSize = 14;
  String m_text;
};

class BitmapElement : public Element
{
public:
  BitmapElement(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates);

  void setBitmap(const uint8_t* bitmap);
  void setBitmapColor(uint32_t color);

  void show(bool force, bool clearDirty = true) override;

protected:
  uint32_t m_bitmapColor = COMMON_CONTENT_COLOR;
  const uint8_t* m_bitmap = nullptr;
};

#endif
