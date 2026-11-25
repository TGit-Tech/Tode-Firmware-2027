#include "DitsUI.h"

void DitsUI::Begin() { 
  tmpDITName = new char[Dits.DITNameChar];
  pinMode(DISPLIGHTPIN, OUTPUT);
  digitalWrite(DISPLIGHTPIN, DISPLIGHTACTIVE);  
  GFX.setRotation(2);
  GFX.setFont(&OneSlot9pt7b);
  
  //NodeScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits)
  OnScreen = new NodeScreen(GFX,NSTHISNODE,Dits);
  OnScreen->DisplayScreen(); 
}
bool DitsUI::gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, uint16_t _Value) {
  if(OnScreen->NScrIdx!=_NScrIdx) return false;
  OnScreen->gotValue(_NScrIdx,_WHandleUID,_Value);
}
//'''''''''''''''''''''''''''''''''''( UI Navigation )''''''''''''''''''''''''''''''''''''''''''''''''''
// If switching Screens, the navigation belongs here
uint8_t DitsUI::ScreenNavigate(uint8_t _Button) {
  DBUIAENTER((_Button,HEX),("DitsUI::ScreenNavigate(_Button)\n"))
  if(OnScreen==nullptr) { DBUIERROR(("DitsUI::ScreenNavigate OnScreen==nullptr\n")) return _Button; }
  uint8_t RtnNavKey = OnScreen->Navigate(_Button);
  DBUIAINFO((RtnNavKey,HEX),("DitsUI::ScreenNavigate returned(RtnNavKey)\n"))

  // Node Screen Navigation
  if(OnScreen->NScrIdx <= NSNODEMAX) {
    if(RtnNavKey==NAVKEYLEFT||RtnNavKey==NAVKEYRIGHT) {
      DITSEngine::NodeDIT node = Dits.Node(OnScreen->NScrIdx&NODEIDXMASK);
      if(RtnNavKey==NAVKEYLEFT) {
        DBUIINFO(("DitsUI::ScreenNavigate [<=NSNODEMAX] NAVKEYLEFT Previous Node\n"))
        if (node.Prev()) {
          //NodeScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits)
          delete OnScreen; OnScreen=nullptr; 
          OnScreen = new NodeScreen(GFX,node.NodeIdx(),Dits);
          OnScreen->DisplayScreen();
          return NAVKEYNONE;
        }
      }    
      if(RtnNavKey==NAVKEYRIGHT) {
        DBUIINFO(("DitsUI::Navigate [<=NSNODEMAX] NAVKEYRIGHT Next Node\n"))
        if (node.Next()) { 
          delete OnScreen; OnScreen=nullptr; 
          OnScreen = new NodeScreen(GFX,node.NodeIdx(),Dits);
          OnScreen->DisplayScreen();
          return NAVKEYNONE;
        }
      }
    }
    if(RtnNavKey==NAVKEYSET) { // SET on This Node Title triggers Setup Menu
      if((OnScreen->NScrIdx&NODEIDXMASK) == NSTHISNODE && OnScreen->SelectedWHandleUID()==WHTITLE ) {
        DBUIINFO(("DitsUI::Navigate [<=NSNODEMAX] NAVKEYSET Node->SetupMenu\n"))
        delete OnScreen; OnScreen=nullptr;
        OnScreen = new SetupMenu(GFX,NSSETUP);
        OnScreen->DisplayScreen();
        return NAVKEYNONE;
      }
    }

  // Setup Menu Navigation
  } else if(OnScreen->NScrIdx == NSSETUP) { 
    if(RtnNavKey==NAVKEYLEFT) {
      DBUIINFO(("DitsUI::ScreenNavigate [NSSETUP] NAVKEYLEFT SetupMenu->Node\n"))
      delete OnScreen; OnScreen=nullptr;
      OnScreen = new NodeScreen(GFX,NSTHISNODE,Dits);
      OnScreen->DisplayScreen();
      return NAVKEYNONE;
    }   
    if(RtnNavKey==NAVKEYGET) { 
      switch(OnScreen->SelectedWHandleUID()) {
        case WHADDRNODE: 
          DBUIINFO(("DitsUI::ScreenNavigate [NSSETUP] NAVKEYLEFT SetupMenu->AddRNode\n"))
          return NAVKEYNONE;
        case WHDELRNODE: 
          DBUIINFO(("DitsUI::ScreenNavigate [NSSETUP] NAVKEYLEFT SetupMenu->DelRNode\n"))
          return NAVKEYNONE;
        case WHTODESETUP: 
          DBUIINFO(("DitsUI::ScreenNavigate [NSSETUP] NAVKEYLEFT SetupMenu->TodeSetup\n"))
          delete OnScreen; OnScreen=nullptr;
          OnScreen = new TodeSetup(GFX,NSTODESETUP,Dits,Ebyte);
          OnScreen->DisplayScreen();
          return NAVKEYNONE;
        case WHADDDEV:
          DBUIINFO(("DitsUI::ScreenNavigate [NSSETUP] NAVKEYLEFT SetupMenu->AddDevice\n"))
          delete OnScreen; OnScreen=nullptr;
          OnScreen = new AddDevice(GFX,NSADDDEV,Dits);
          OnScreen->DisplayScreen();
          return NAVKEYNONE;
        case WHDELDEV: 
          return NAVKEYNONE;
        case WHEDITDEV: 
          return NAVKEYNONE;
        case WHSYSTEM: 
          return NAVKEYNONE;
      }
    }  
  }
  return RtnNavKey;
}
