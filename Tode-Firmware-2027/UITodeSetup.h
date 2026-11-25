#include <TGTWidgets.h>
#include "DITImplement.h"
#include <TGTEbyte.h>

#define NAVNEWDEVICE  0xCA
#define NAVGETVALS    0xCC
#define NAVGETSETVAL  0xCB    // Only Used to verify a SETVAL

#define WHTITLE       0x0A00
// Setup Screen Widget Handles
#define WHADDRNODE    0xDB07
#define WHDELRNODE    0xDB08
#define WHTODESETUP   0xDB09
#define WHADDDEV      0xDB0A
#define WHDELDEV      0xDB0B
#define WHEDITDEV     0xDB0C
#define WHSYSTEM      0xDB0D

// Tode Setup Screens (NScrIdx) Identity
#define NSSETUP      0x0A03
#define NSTODESETUP  0x0A04
#define NSSYSTEM     0x0A05
#define NSADDDEV     0x0A06
//-------------------------------------------------------------------------------------------------
class SetupMenu : public TGTScreen {
public:
  SetupMenu(Adafruit_GFX& _GFX, uint16_t _NScrIdx)
  : TGTScreen(_GFX,_NScrIdx) {}

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
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
};
//-------------------------------------------------------------------------------------------------
class AddDevice : public TGTScreen {
public:
  AddDevice(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), node(_Dits.Node(0))
  { tmpDITName = new char[Dits.DITNameChar]; }
  ~AddDevice() { delete[] tmpDITName; }

  byte Add_devType = BNONE;
  uint8_t AdddevUID = BNONE;
  byte Add_IOPin = 0;
  byte Add_pinMode = 0;
  byte Add_Active = 0;

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
  DITImplement& Dits;
  DITSEngine::NodeDIT node;
  char* tmpDITName; // General use array for DIT get Names
};