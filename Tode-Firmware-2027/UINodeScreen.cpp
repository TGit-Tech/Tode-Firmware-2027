#include "UINodeScreen.h"

NodeScreen::NodeScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITSImplement& _Dits, uint16_t _NodeIdx) {
  tmpDITName = new char[Dits.DITNameChar];
}

void NodeScreen::DisplayScreen() {
    row=0; ClearScreen();
    node.NGetName(tmpDITName);
    NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),Dits.DITNameChar,tmpDITName,WHTITLE);

    DITSEngine::DeviceDIT dev = Dits.Device();
    while (row < TFT_ROWS - 1 && dev.Next(NodeIdx)) {
      row++; // Note: WHandleUID is DevUID
      dev.DGetName(tmpDITName);
      NewWChLabel(0,TFT_COL_X(0), TFT_ROW_Y(row),Dits.DITNameChar,tmpDITName); // Label
      switch(dev.DevType()) {
        case DT_ONOFF:
        case DT_STSTP3W:
          NewWEnumVal (row+1,TFT_COL_X(10), TFT_ROW_Y(row), true, dev.DevUID());
          AddEnum(dev.DevUID(),0,F("OFF"));
          AddEnum(dev.DevUID(),1,F("ON"));
          break;
        case DT_DIST:
          NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row), TUINT+TDEC,dev.DevUID());
          break;
        case DT_ANAINPUT:
          NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC,dev.DevUID());
          break;
        case DT_ANAOUTPUT:
          NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC+TSETPD,dev.DevUID());
          break;
        case DT_DLSETPOINT:
          NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC+TSETPD,dev.DevUID());
          break;
        case DT_DLMATH:
          NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC,dev.DevUID());
          break;
      }
    }
}
uint8_t NodeScreen::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(uint8_t _Button);
  DBUIAENTER((RtnNavKey,HEX),("NodeScreen::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVKEYSET && NodeIdx==0 && pSelected!=nullptr ) { // SET on This Node(0) Title triggers Setup Menu
    if(pSelected->WHandleUID==WHTITLE) return NAVTO_SETUPMENU;
    }
    if(node.NodeIdx()==0 && ) {
      SetupMenu(NAVDISPLAY); return NAVKEYNONE;
    }
  }
  return RtnNavKey;
}