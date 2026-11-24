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

// UI Navigation Screens & Navigation Commands
// Nav Screens Dynamic
#define NODEIDXMASK 0x00FF
#define NSTHISNODE  0x0000
#define NSNODEMAX   0x09FF   // Sub-Screen 9 of NodeIdx 255(FF).

// Nav Screens (NScrIdx) Identity
#define NSNODE       0x0A01
#define NSSETUP      0x0A03
#define NSTODESETUP  0x0A04
#define NSSYSTEM     0x0A05
#define NSADDDEV     0x0A06

// Multi-Use Widget Handles
//#define WHTITLE         0x0A00
#define WHTITLENN       0x0A01    // Screen Title Node Name

// Node Edit Widget Handles
#define WHNODENAME      0xDA02    // D for Widget
#define WHSECNET        0xDA03
#define WHRADIOID       0xDA04
#define WHNETID         0xDA05
#define WHRFADDR        0xDA06
#define WHRFFREQ        0xDA07

// Setup Screen Widget Handles
#define WHADDRNODE    0xDB07
#define WHDELRNODE    0xDB08
#define WHTODESETUP   0xDB09
#define WHADDDEV      0xDB0A
#define WHDELDEV      0xDB0B
#define WHEDITDEV     0xDB0C
#define WHSYSTEM      0xDB0D

// Add Device Widget Handlers
#define WHDEVNAME     0xDC0E
#define WHDEVTYPE     0xDC0F
#define WHIOPIN       0xDC10
#define WHPINMODE     0xDC11
#define WHACTIVE      0xDC12
#define WHADDBTN      0xDC13

// 0x[E]- Event Drivers ... Key Presses Reserved in UIBase/Config.h
#define NAVDISPLAY    0xCD  // C for Command
#define NAVGETVALS    0xCC
#define NAVGETSETVAL  0xCB    // Only Used to verify a SETVAL
#define NAVNEWDEVICE  0xCA
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
  DitsUI::~DitsUI() {   }
  
  byte Add_devType = BNONE;
  uint8_t AdddevUID = BNONE;
  TGTScreen* OnScreen = nullptr;

  void Begin(DITSEngine& _dits, Adafruit_GFX& _gfx, TGTEbyte& _pEbyte);
  uint8_t ScreenNavigate(uint8_t _Button);
  bool gotValue(uint16_t NScrIdx, uint16_t WHandleUID, uint16_t _Value);

private:
  DITSEngine* pDits = nullptr;
  Adafruit_GFX* pGFX = nullptr;
  TGTEbyte* pEbyte = nullptr;
  char* tmpDITName; // General use array for DIT get Names

  byte Add_IOPin = 0;
  byte Add_pinMode = 0;
  byte Add_Active = 0;

  uint8_t NodeScreen(uint8_t NavKey, uint16_t NodeIdx);
  uint8_t SetupMenu(uint8_t NavKey);
  uint8_t TodeSetup(uint8_t NavKey);
  uint8_t AddDevice(uint8_t NavKey);
};