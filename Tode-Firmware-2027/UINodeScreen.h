#include <TGTWidgets.h>
#include "DITImplement.h"

constexpr uint16_t WHTITLE      =0x0A00;
constexpr uint8_t  NAVTO_SETUP  =0xD3;

class NodeScreen : public TGTScreen {
public:
  NodeScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITSImplement& _Dits, uint16_t _NodeIdx)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), NodeIdx(_NodeIdx), node(_Dits.Node(_NodeIdx));
  ~NodeScreen() { delete[] tmpDITName; }

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button);
uint8_t DitsUI::NodeScreen(uint8_t NavKey, uint16_t NodeIdx) {
  DBUIAAENTER((NavKey,HEX),(NodeIdx),("DitsUI::NodeScreen(NavKey,NodeIdx)\n"))

  DITSEngine::NodeDIT node = Dits.Node(NodeIdx);
  if(NavKey==NAVKEYLEFT) {
    DBUIINFO(("DitsUI::Navigate NavKey==NAVKEYLEFT Previous Node\n"))
    if (node.Prev()) { NavKey=NAVDISPLAY; }
  }
  if(NavKey==NAVKEYRIGHT) {
    DBUIINFO(("DitsUI::Navigate NavKey==NAVKEYRIGHT Next Node\n"))
    if (node.Next()) { NavKey=NAVDISPLAY; }
  }
  if(NavKey==NAVKEYGET) { NavKey=NAVGETVALS; }



  //'''''''''' NAVDISPLAY
  if(NavKey==NAVDISPLAY) {
    delete OnScreen; OnScreen = nullptr; int row = 0;
    OnScreen = new TGTScreen(*pGFX,node.NodeIdx());
    
    return NAVKEYNONE;
  }
  /*
  if(NavKey==NAVGETVALS || NavKey==NAVGETSETVAL) {
    bool bAll = (NavKey==NAVGETVALS);
    if(SelectedWH==WHNODENAME||bAll) {
      DITSEngine::NodeDIT node = Dits.Node(0); node.NGetName(tmpDITName);
      this->gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
    }
    if(SelectedWH==WHSECNET||bAll)   { this->gotValue(NSTODESETUP, WHSECNET, Dits.SecNetCode()); }
    if(SelectedWH==WHRADIOID||bAll)  { this->gotValue(NSTODESETUP, WHRADIOID, pEbyte->RadioID()); }
    if(SelectedWH==WHNETID||bAll)    { this->gotValue(NSTODESETUP, WHNETID, pEbyte->NetID()); }
    if(SelectedWH==WHRFADDR||bAll)   { this->gotValue(NSTODESETUP, WHRFADDR, pEbyte->Address()); }
    if(SelectedWH==WHRFFREQ||bAll)   { this->gotValue(NSTODESETUP, WHRFFREQ, pEbyte->Frequency()); }
    return NAVKEYNONE;
  }
  */
  return NavKey;
}
  }
private:
  DITImplement& Dits;
  uint16_t NodeIdx;
  DITSEngine::NodeDIT node;
  char* tmpDITName; // General use array for DIT get Names
};