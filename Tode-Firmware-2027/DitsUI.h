///@file    DitsUI.h
///@brief   TFT & Keypad user interface (UI) for RF-DITS protocol.
#pragma once

#include "Config.h"
#include <Arduino.h>
#include <rf_dits.h>
#include <Adafruit_ST7735.h>
#include <OneSlot9pt7b.h>
#include <TGTEbyte.h>
#include <TGTWidgets.h>     // Setup Base Class
#include "SETGS.h"
//#include "LDeviceI.h"

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
*/
#define BNONE     0xFF    ///< Byte NONE/UNSET
#define INONE     0xFFFF  ///< Integer NONE/UNSET
#define SINONE    0x8000  ///< Signed Integer NONE/UNSET

// UI Navigation Screens & Navigation Commands
// Nav Screens Dynamic
#define NODEIDXMASK 0x00FF
#define NSTHISNODE  0x0000
#define NSNODEMAX   0x09FF   // Sub-Screen 9 of NodeIdx 255(FF).

// Nav Screens Identity
#define NSNODE       0x0A01
#define NSSETUP      0x0A03
#define NSTODESETUP  0x0A04
#define NSSYSTEM     0x0A05
#define NSADDDEV     0x0A06

// Multi-Use Widget Handles
#define WHTITLE         0x0A00
#define WHTITLENN       0x0A01    // Screen Title Node Name

// Node Edit Widget Handles
#define WHNODENAME      0x0A02
#define WHSECNET        0x0A03
#define WHRADIOID       0x0A04
#define WHNETID         0x0A05
#define WHRFADDR        0x0A06

// Setup Screen Widget Handles
#define WHADDRNODE    0x0A07
#define WHDELRNODE    0x0A08
#define WHTODESETUP   0x0A09
#define WHADDDEV      0x0A0A
#define WHDELDEV      0x0A0B
#define WHEDITDEV     0x0A0C
#define WHSYSTEM      0x0A0D

// Add Device Widget Handlers
#define WHDEVNAME     0x0A0E
#define WHDEVTYPE     0x0A0F
#define WHIOPIN       0x0A10
#define WHPINMODE     0x0A11
#define WHACTIVE      0x0A12
#define WHADDBTN      0x0A13

// 0x[E]- Event Drivers ... Key Presses Reserved in UIBase/Config.h
#define NAVDISPLAY    0xED  
#define NAVGETVALS    0xEC
#define NAVGETSETVAL  0xEB    // Only Used to verify a SETVAL
#define NAVNEWDEVICE  0xEA
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
/*
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), pDits->DITNameChar, true, WHDEVNAME); //tmpDITName
    memset(tmpDITName, 0, pDits->DITNameChar);
    OnScreen->gotValue(NSTODESETUP, WHDEVNAME, tmpDITName);
*/    
/*
///@defgroup DT [DT] Device-Type enumeration.
/// DITS provides 1-byte to designate Device Type definitions.
/// Never (0) as RF uses null-termination '\0' to seperate records.
/// Prevent usage of (0xF?) packet types as they help signal beginning of packets.
///@{
// 0x[7] Device Types
#define DT_NOTSET       0x7F
#define DT_ONOFF        0x7E    ///< On/Off Switching Device
#define DT_DIST         0x7A    ///< Distance Sensing Device
#define DT_STSTP3W      0x79    ///< Start Stop 3-Wire
#define DT_ANAINPUT     0x78    ///< Analog Device (Pressure, Temperature)
#define DT_ANAOUTPUT    0x77    ///< Analog Output (PWM control)
#define DT_DLSETPOINT   0x76    ///< Dev Logic Control Boundary
#define DT_DLMATH       0x75    ///< Dev Logic Compare / Math Device

#define DT_MINBOUNDARY  0x75    ///< MINIMUM BOUNDARY CHECK FOR ADD DEVICE KEYS
///@}
*/
class DitsUI
{
public:
  DitsUI::~DitsUI() {   }
  
  byte Add_devType = BNONE;
  uint8_t AdddevUID = BNONE;

  void Begin(DITSEngine& _dits, Adafruit_GFX& _gfx, TGTEbyte& _pEbyte);
  uint8_t ScreenNavigate(uint8_t _Button);
  //bool gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
  bool gotValue(uint16_t NScrIdx, uint16_t WHandleUID, uint16_t _Value);

private:
  DITSEngine* pDits = nullptr;
  Adafruit_GFX* pGFX = nullptr;
  TGTEbyte* pEbyte = nullptr;
  TGTScreen* OnScreen = nullptr;
  char* tmpDITName; // General use array for DIT get Names

  byte Add_IOPin = 0;
  byte Add_pinMode = 0;
  byte Add_Active = 0;

  uint8_t NodeScreen(uint8_t NavKey, uint16_t NodeIdx);
  uint8_t SetupMenu(uint8_t NavKey);
  uint8_t TodeSetup(uint8_t NavKey);
  uint8_t AddDevice(uint8_t NavKey);
};