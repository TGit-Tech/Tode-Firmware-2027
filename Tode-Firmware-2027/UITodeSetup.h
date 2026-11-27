#pragma once

#include <TGTWidgets.h>
#include "DITImplement.h"
#include <TGTEbyte.h>

#define NSTHISNODE  0x0000
#define NAVGETVALS    0xCC
#define NAVGETSETVAL  0xCB    // Only Used to verify a SETVAL

#define WHTITLE       0x0A00
// Setup Screen Widget Handles
#define WHDELRNODE    0xDB08
#define WHTODESETUP   0xDB09
#define WHADDDEV      0xDB0A
#define WHDELDEV      0xDB0B
#define WHEDITDEV     0xDB0C
#define WHSYSTEM      0xDB0D
#define WHADDONOFF    0xDB0E

// Tode Setup Screens (NScrIdx) Identity
#define NSSETUP      0x0A03
#define NSTODESETUP  0x0A04
#define NSSYSTEM     0x0A05
//-------------------------------------------------------------------------------------------------
class SetupMenu : public TGTScreen {
public:
  SetupMenu(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits) 
  {tmpDITName = new char[Dits.DITNameChar];}
  ~SetupMenu() { delete[] tmpDITName; }

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
  DITImplement& Dits;
  char* tmpDITName; // General use array for DIT get Names
};
//-------------------------------------------------------------------------------------------------
class TodeSetup : public TGTScreen {
public:
  TodeSetup(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits, TGTEbyte& _Ebyte)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), node(_Dits.Node(_NScrIdx&0x00FF)), Ebyte(_Ebyte)
  {tmpDITName = new char[Dits.DITNameChar];}
  ~TodeSetup() { delete[] tmpDITName; }

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
  DITImplement& Dits;
  DITSEngine::NodeDIT node;
  TGTEbyte& Ebyte;
  char* tmpDITName; // General use array for DIT get Names
  void getValues(bool SelOnly = false);
};