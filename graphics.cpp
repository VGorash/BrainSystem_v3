#include "graphics.h"

Element::Element(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates)
{
  m_tft = tft;
  m_fontRender = fontRender;
  m_coordinates = coordinates;
  m_hidden = false;
  m_updateCallback = nullptr;
  m_onPress = nullptr;
  m_onClick = nullptr;
  m_onHold = nullptr;
}

coordinates_t Element::getCoordinates()
{
  return m_coordinates;
}

bounds_t Element::getBounds()
{
  return {m_coordinates.x, m_coordinates.y, m_coordinates.w + m_coordinates.x, m_coordinates.h + m_coordinates.y};
}

bool Element::checkHit(uint x, uint y)
{
  bounds_t bounds = getBounds();
  return x >= bounds.left && x <= bounds.right && y >= bounds.top && y <= bounds.bottom;
}

bool Element::shouldUpdate(bool force)
{
  return !m_hidden && (force || m_dirty);
}

void Element::show(bool force, bool clearDirty)
{
  if(!shouldUpdate(force))
  {
    return;
  }

  m_tft->fillRoundRect(m_coordinates.x, m_coordinates.y, m_coordinates.w, m_coordinates.h, m_borderRadius, m_backgroundColor);
  m_tft->drawRoundRect(m_coordinates.x, m_coordinates.y, m_coordinates.w, m_coordinates.h, m_borderRadius, m_borderColor);

  if(clearDirty)
  {
    m_dirty = false;
  }
}

void Element::setUpdateCallback(void (*updateCallback)(const DisplayState&, Element*))
{
  m_updateCallback = updateCallback;
}

void Element::setOnPressCallback(void (*onPressCallback)(DisplayState&, Element*))
{
  m_onPress = onPressCallback;
}

void Element::setOnClickCallback(void (*onClickCallback)(DisplayState&, Element*))
{
  m_onClick = onClickCallback;
}

void Element::setOnHoldCallback(void (*onHoldCallback)(DisplayState&, Element*))
{
  m_onHold = onHoldCallback;
}

void Element::setBackgroundColor(uint32_t color)
{
  if(m_backgroundColor != color)
  {
    m_backgroundColor = color;
    m_dirty = true;
  }
}

void Element::setBorderColor(uint32_t color)
{
  if(m_borderColor != color)
  {
    m_borderColor = color;
    m_dirty = true;
  }
}

void Element::setBorderRadius(int32_t radius)
{
  if(m_borderRadius != radius)
  {
    m_borderRadius = radius;
    m_dirty = true;
  }
}

void Element::update(const DisplayState& state)
{
  if(!m_updateCallback)
  {
    return;
  }
  m_updateCallback(state, this);
}

void Element::setHidden(bool hidden)
{
  if(m_hidden != hidden)
  {
    m_hidden = hidden;
    m_dirty = true;
  }
}

void Element::onPress(DisplayState& state, uint x, uint y)
{
  if(!m_onPress|| m_hidden || !checkHit(x, y))
  {
    return;
  }
  m_onPress(state, this);
}

void Element::onClick(DisplayState& state, uint x, uint y)
{
  if(!m_onClick|| m_hidden || !checkHit(x, y))
  {
    return;
  }
  m_onClick(state, this);
}

void Element::onHold(DisplayState& state, uint x, uint y)
{
  if(!m_onHold|| m_hidden || !checkHit(x, y))
  {
    return;
  }
  m_onHold(state, this);
}


TextElement::TextElement(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates) : Element(tft, fontRender, coordinates){}

void TextElement::setText(const String& text)
{
  if(m_text != text)
  {
    m_text = text;
    m_dirty = true;
  }
}

void TextElement::setTextColor(uint32_t color)
{
  if(m_textColor != color)
  {
    m_textColor = color;
    m_dirty = true;
  }
}

void TextElement::setFontSize(uint size)
{
  if(m_fontSize != size)
  {
    m_fontSize = size;
    m_dirty = true;
  }
}

void TextElement::show(bool force, bool clearDirty)
{
  Element::show(force, false);

  if(!shouldUpdate(force))
  {
    return;
  }

  m_fontRender->setFontColor(m_textColor, m_backgroundColor);
  m_fontRender->setFontSize(m_fontSize);
  uint x = m_coordinates.x + (m_coordinates.w - m_fontRender->getTextWidth(m_text.c_str())) / 2;
  uint y = m_coordinates.y + (m_coordinates.h - m_fontRender->getTextHeight(m_text.c_str())) / 2;
  m_fontRender->setCursor(x, y);
  m_fontRender->printf(m_text.c_str());

  if(clearDirty)
  {
    m_dirty = false;
  }
}

BitmapElement::BitmapElement(TFT_eSPI* tft, OpenFontRender* fontRender, const coordinates_t& coordinates): Element(tft, fontRender, coordinates){}

void BitmapElement::setBitmap(const uint8_t* bitmap)
{
  if(m_bitmap != bitmap)
  {
    m_bitmap = bitmap;
    m_dirty = true;
  }
}

void BitmapElement::setBitmapColor(uint32_t color)
{
  if(m_bitmapColor != color)
  {
    m_bitmapColor = color;
    m_dirty = true;
  }
}

void BitmapElement::show(bool force, bool clearDirty)
{
  Element::show(force, false);

  if(!shouldUpdate(force) || !m_bitmap)
  {
    return;
  }

  m_tft->drawBitmap(m_coordinates.x, m_coordinates.y, m_bitmap, m_coordinates.w, m_coordinates.h, m_bitmapColor, m_backgroundColor);

  if(clearDirty)
  {
    m_dirty = false;
  }
}
