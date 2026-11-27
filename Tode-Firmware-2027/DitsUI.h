///@file    DitsUI.h
///@brief   TFT & Keypad user interface (UI) for RF-DITS protocol on Tode.
#pragma once

#include "Config.h"
#include <Arduino.h>
#include "DITImplement.h"
#include <Adafruit_ST7735.h>
#include <OneSlot9pt7b.h>
#include <TGTEbyte.h>
#include <TGTWidgets.h>     // Setup Base Class
#include "UINodeScreen.h"
#include "UITodeSetup.h"
#include "UIDevices.h"

/* Defined in TGTWidgets Library
// WNumVal DataTypes
#define TBYTE     0b00000000  // Byte             0bxxx0 0xxx
#define TUINT     0b00001000  // Unsigned integer 0bxxx0 1xxx
#define TSINT     0b00010000  // Signed integer   0bxxx1 0xxx
#define THEX      0b00100000  // Hexadecimal      0bxx1x xxxx
#define TDEC      0b00000000  // Decimal          0bxx0x xxxx
#define TNOSET    0b00000000  // Not settable     0b00xx xxxx
#define TSET      0b01000000  // Settable         0b01xx xxxx
#define TSETPD    0b11000000  // Settable, per digit 0b11xx xxxx
// Defined in SETGS.h through DITImplement through LDevices
#define BNONE     0xFF    ///< Byte NONE/UNSET
#define INONE     0xFFFF  ///< Integer NONE/UNSET
#define SINONE    0x8000  ///< Signed Integer NONE/UNSET
*/

// Multi-Use Widget Handles

// 0x[E]- Event Drivers ... Key Presses Reserved in UIBase/Config.h
#define NAVGETVALS    0xCC
#define NAVGETSETVAL  0xCB    // Only Used to verify a SETVAL

//-------------------------------------------------------------------------
// Defined in TGTWidgetsConfig.h
//-------------------------------------------------------------------------
#ifndef NSNONE
  #define NSNONE      0xFFFF  // Should be defined in TGTWidgetsConfig.h
#endif
#ifndef WHNONE
  #define WHNONE      0xFFFF  // Should be defined in TGTWidgetsConfig.h
#endif
//#define WHNONE          0xFFFF  // Reserved. Used in UIBase.h
//#define NAVSTAGEVAL   0xEF  // Used in UIBase.h
//#define NAVSETVAL     0xEE  // Used in UIBase.h
//#define NSNONE        0xFFFF  // Used in UIBase.h
//-------------------------------------------------------------------------
class DitsUI
{
public:
  DitsUI(Adafruit_GFX& _GFX, DITImplement& _Dits, TGTEbyte& _Ebyte)
  : GFX(_GFX), Dits(_Dits), Ebyte(_Ebyte) {  }
  DitsUI::~DitsUI() {   }
  

  TGTScreen* OnScreen = nullptr;

  void Begin();
  uint8_t ScreenNavigate(uint8_t _Button);
  bool gotValue(uint16_t NScrIdx, uint16_t WHandleUID, uint16_t _Value);
  void DitUpdate(uint8_t nodeIdx);

private:
  DITImplement& Dits;
  Adafruit_GFX& GFX;
  TGTEbyte& Ebyte;
  char* tmpDITName; // General use array for DIT get Names
};