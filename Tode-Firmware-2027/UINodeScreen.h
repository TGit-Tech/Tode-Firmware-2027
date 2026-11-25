#include <TGTWidgets.h>
#include "DITImplement.h"

#define WHTITLE      0x0A00

#define NAVGETVALS    0xCC
#define NAVGETSETVAL  0xCB    // Only Used to verify a SETVAL

// Nav Screens (NScrIdx) Identity
#define NSNODE       0x0A01
// UI Navigation Screens & Navigation Commands
// Nav Screens Dynamic
#define NODEIDXMASK 0x00FF
#define NSTHISNODE  0x0000
#define NSNODEMAX   0x09FF   // Sub-Screen 9 of NodeIdx 255(FF).

class NodeScreen : public TGTScreen {
public:
  NodeScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), node(_Dits.Node(_NScrIdx&0x00FF))
  {tmpDITName = new char[Dits.DITNameChar];}
  ~NodeScreen() { delete[] tmpDITName; }

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button);
  
private:
  DITImplement& Dits;
  DITSEngine::NodeDIT node;
  char* tmpDITName; // General use array for DIT get Names
};