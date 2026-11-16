///@file    DitsUI.h
///@brief   TFT & Keypad user interface (UI) for RF-DITS protocol.
#pragma once

#include "Config.h"
#include <rf_dits.h>
#include <Adafruit_ST7735.h>
#include <OneSlot9pt7b.h>
#include <Ebyte.h>
#include <TGTWidgets.h>     // Setup Base Class

// UI Navigation Screens & Navigation Commands
// Nav Screens Dynamic
#define NODEIDXMASK 0x00FF
#define NSTHISNODE  0x0000
#define NSNODEMAX   0x09FF   // Sub-Screen 9 of NodeIdx 255(FF).

// Nav Screens Identity
//#define NSNONE        0xFFFF  // Used in UIBase.h
#define NSNODE       0x0A01
#define NSSETUP      0x0A03
#define NSEDITNODE   0x0A04
#define NSSYSTEM     0x0A05

// Multi-Use Widget Handles
//#define WHNONE          0xFFFF  // Reserved. Used in UIBase.h
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
#define WHEDITNODE    0x0A09
#define WHADDDEV      0x0A0A
#define WHDELDEV      0x0A0B
#define WHEDITDEV     0x0A0C
#define WHSYSTEM      0x0A0D

// 0x[F]-Key Presses Reserved in UIBase/Config.h
//#define NAVSTAGEVAL   0xEF  // Used in UIBase.h
//#define NAVSETVAL     0xEE  // Used in UIBase.h
#define NAVDISPLAY    0xED  
#define NAVGETVALS    0xEC
#define NAVGETSETVAL  0xEB    // Only Used to verify a SETVAL

class DitsUI
{
public:
  DitsUI::~DitsUI() {   }
  
  void Begin(DITSEngine& _dits, Adafruit_GFX& _gfx, Ebyte& _pEbyte);
  uint8_t ScreenNavigate(uint8_t _Button);

private:
  DITSEngine* pDits = nullptr;
  Adafruit_GFX* pGFX = nullptr;
  Ebyte* pEbyte = nullptr;
  void DumpRadio1DITS();
  TGTScreen* OnScreen = nullptr;
  void SGValues(bool _Set = false);
  void NodeScreen(DITSEngine::NodeDIT& node, uint8_t NavKey);
  void SetupScreen(uint8_t NavKey);
  void NodeEditScreen(uint8_t NavKey);
  char* tmpDITName; // General use array for DIT get Names
};