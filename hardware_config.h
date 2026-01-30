#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// CHANGEABLE PARAMETERS
#define USE_WIRED_BUTTONS   // Comment if wired buttons aren't used
#define USE_BUTTON_LEDS     // Comment if button LEDs aren't used
#define USE_SIGNAL_LED      // Comment if signal LED isn't used
//#define USE_LED_STRIP     // Uncomment if LED strip is used
#define USE_UART_LINKS      // Comment if UART links aren't used
#define USE_WIRELESS_LINK   // Comment if wireless link isn't used
//#define USE_BATTERY       // Uncomment if battery is used

//-------------------------------------------------------------------------------

// EDIT SECTION BELOW ONLY IF YOU WANT TO CHANGE PIN NUMBERS
// TO ENABLE OR DISABLE FEATURES USE SECTION ABOVE

// Wired buttons
#ifdef USE_WIRED_BUTTONS
  #define NUM_WIRED_BUTTONS 4
  #define BUTTON_PLAYER_1_PIN 4
  #define BUTTON_PLAYER_2_PIN 5
  #define BUTTON_PLAYER_3_PIN 35
  #define BUTTON_PLAYER_4_PIN 34
  static const int wiredButtonPins[NUM_WIRED_BUTTONS] = {BUTTON_PLAYER_1_PIN, BUTTON_PLAYER_2_PIN, BUTTON_PLAYER_3_PIN, BUTTON_PLAYER_4_PIN};
#else
  #define NUM_WIRED_BUTTONS 0
#endif
// ---------------------

// Button LEDs
#ifdef USE_BUTTON_LEDS
  #define NUM_BUTTON_LEDS 4
  #define LED_PLAYER_1_PIN 12
  #define LED_PLAYER_2_PIN 19
  #define LED_PLAYER_3_PIN 22
  #define LED_PLAYER_4_PIN 23
  static const int buttonLedPins[NUM_BUTTON_LEDS] = {LED_PLAYER_1_PIN, LED_PLAYER_2_PIN, LED_PLAYER_3_PIN, LED_PLAYER_4_PIN};
#else
  #define NUM_BUTTON_LEDS 0
#endif
// ---------------------

// Signal LED
#ifdef USE_SIGNAL_LED
  #define SIGNAL_LED_PIN 21
#endif
// ---------------------

// Sound
#define BUZZER_PIN 18

// LED strip (multi-color addresable LED strip, can replace signal LED and button LEDs)
#ifdef USE_LED_STRIP
  #define LED_STRIP_PIN 21
  #define LEDS_IN_STRIP 8
#endif
// ---------------------

// UART links
#ifdef USE_UART_LINKS
  #include "HardwareSerial.h"
  #define NUM_UART_LINKS 3
  #define UART0_RX_PIN 3
  #define UART0_TX_PIN 1
  #define UART1_RX_PIN 27
  #define UART1_TX_PIN 26
  #define UART2_RX_PIN 16
  #define UART2_TX_PIN 17
  static const int uartRxPins[NUM_UART_LINKS] = {UART0_RX_PIN, UART1_RX_PIN, UART2_RX_PIN};
  static const int uartTxPins[NUM_UART_LINKS] = {UART0_TX_PIN, UART1_TX_PIN, UART2_TX_PIN};
  static HardwareSerial* uartSerials[NUM_UART_LINKS] = {&Serial, &Serial1, &Serial2};
#else
  #define NUM_UART_LINKS 0
#endif
// ---------------------

// All links
#ifdef USE_WIRELESS_LINK
  #define NUM_LINKS (NUM_UART_LINKS + 1)
#else
  #define NUM_LINKS NUM_UART_LINKS
#endif

#if NUM_LINKS > 0
  #define USE_LINKS
#endif
// ---------------------

// Battery
#ifdef USE_BATTERY
  #define BATTERY_VOLTAGE_PIN 7
#endif
// ---------------------

#endif