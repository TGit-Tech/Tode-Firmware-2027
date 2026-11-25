#include "UINodeScreen.h"

void NodeScreen::DisplayScreen() {
  uint8_t row=0; ClearScreen();
  node.NGetName(tmpDITName);
  NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),Dits.DITNameChar,tmpDITName,WHTITLE);

  DITSEngine::DeviceDIT dev = Dits.Device();
  while (row < TFT_ROWS - 1 && dev.Next(node.NodeIdx())) {
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
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("NodeScreen::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVSETVAL) {
    if(node.NodeIdx()==0) {
      // This Node value Set //Value(uint8_t _devUID, uint16_t _Value)
      Dits.LDev.Value(SelectedWHandleUID(),SelectedWValue());
    } else {
      // Remote Node value Set
      Dits.TxSetRemoteDevVal(node.NodeIdx(), SelectedWHandleUID(), SelectedWValue());
    }
  }
  if(RtnNavKey==NAVKEYGET) {
    if(node.NodeIdx()==0) {

    } else {
      Dits.TxGetRemoteDevVals(node.NodeIdx());
      // Then;  How to disperse values from buffer.
    }
  }
  return RtnNavKey;
}