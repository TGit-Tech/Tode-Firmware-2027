#include "DitsUI.h"

void DitsUI::Begin(DITSEngine& _pDits, Adafruit_GFX& _pGFX, Ebyte& _pEbyte)
{
    pDits = &_pDits;
    pGFX = &_pGFX;
    pEbyte = &_pEbyte;
    tmpDITName = new char[pDits->DITNameChar];
    pinMode(DISPLIGHTPIN, OUTPUT);
    digitalWrite(DISPLIGHTPIN, DISPLIGHTACTIVE);
    
    pGFX->setRotation(2);
    pGFX->setFont(&OneSlot9pt7b);
    
    NodeScreen(NAVDISPLAY,NSTHISNODE);
    DBUIAINFO((pDits->DITNameChar),("DitsUI::Begin (pDits->DITNameChar)"))
}

//'''''''''''''''''''''''''''''''''''( UI Navigation )''''''''''''''''''''''''''''''''''''''''''''''''''
uint8_t DitsUI::ScreenNavigate(uint8_t _Button) {
  DBUIAENTER((_Button),("DitsUI::ScreenNavigate(_Button)\n"))
  if(OnScreen==nullptr) { DBUIERROR(("DitsUI::ScreenNavigate OnScreen==nullptr\n")) return _Button; }
  uint8_t NavKey = OnScreen->OnScreenNavigate(_Button);
  DBUIAINFO((NavKey,HEX),("DitsUI::ScreenNavigate returned(NavKey)\n"))

  // Screen Navigation by Screen Functions
  if(OnScreen->NScrIdx <= NSNODEMAX) { return NodeScreen(NavKey,(OnScreen->NScrIdx & NODEIDXMASK)); }
  else if(OnScreen->NScrIdx == NSSETUP) { return SetupMenu(NavKey); }
  else if(OnScreen->NScrIdx == NSTODESETUP) { return TodeSetup(NavKey); }
  else if(OnScreen->NScrIdx == NSADDDEV) { return AddDevice(NavKey); }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
// When Launching a New Screen discard return.  Example: SetupMenu(NAVDISPLAY); return NAVKEYNONE;
uint8_t DitsUI::NodeScreen(uint8_t NavKey, uint16_t NodeIdx) {
  DBUIAAENTER((NavKey,HEX),(NodeIdx),("DitsUI::NodeScreen(NavKey,NodeIdx)\n"))

  DITSEngine::NodeDIT node = pDits->Node(NodeIdx);
  if(NavKey==NAVKEYLEFT) {
    DBUIINFO(("DitsUI::Navigate NavKey==NAVKEYLEFT Previous Node\n"))
    if (node.Prev()) { NavKey=NAVDISPLAY; }
  }
  if(NavKey==NAVKEYRIGHT) {
    DBUIINFO(("DitsUI::Navigate NavKey==NAVKEYRIGHT Next Node\n"))
    if (node.Next()) { NavKey=NAVDISPLAY; }
  }
  if(NavKey==NAVKEYGET) { NavKey=NAVGETVALS; }
  if(NavKey==NAVKEYSET) { // SET on This Node Title triggers Setup Menu
    if(node.NodeIdx()==0 && OnScreen->SelectedWHandleUID()==WHTITLE) {
      SetupMenu(NAVDISPLAY); return NAVKEYNONE;
    }
  }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen; OnScreen = nullptr; int row = 0;
    OnScreen = new TGTScreen(*pGFX,node.NodeIdx());
    node.NGetName(tmpDITName);
    OnScreen->NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),pDits->DITNameChar,tmpDITName,WHTITLE);

    DITSEngine::DeviceDIT dev = pDits->Device();
    while (row < TFT_ROWS - 1 && dev.Next(node.NodeIdx())) {
      row++; // Note: WHandleUID is DevUID
      dev.DGetName(tmpDITName);
      OnScreen->NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),pDits->DITNameChar,tmpDITName);
    }
    OnScreen->DisplayScreen();
    return NAVKEYNONE;
  }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
uint8_t DitsUI::SetupMenu(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::SetupMenu(NavKey)\n"))
  
  if(NavKey==NAVKEYGET) { 
    switch(OnScreen->SelectedWHandleUID()) {
      case WHADDRNODE: return NAVKEYNONE;
      case WHDELRNODE: return NAVKEYNONE;
      case WHEDITNODE: TodeSetup(NAVDISPLAY); return NAVKEYNONE;
      case WHADDDEV: AddDevice(NAVDISPLAY); return NAVKEYNONE;
      case WHDELDEV: return NAVKEYNONE;
      case WHEDITDEV: return NAVKEYNONE;
      case WHSYSTEM: return NAVKEYNONE;
    }
  }
  if(NavKey==NAVKEYLEFT) { NodeScreen(NAVDISPLAY,NSTHISNODE); return NAVKEYNONE; }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSSETUP);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Setup Menu"),WHTITLE);
    OnScreen->NewWFLabel  (2,TFT_COL_X(0), TFT_ROW_Y(1), F("Add RTode"), WHADDRNODE);
    OnScreen->NewWFLabel  (3,TFT_COL_X(0), TFT_ROW_Y(2), F("Del RTode"), WHDELRNODE);
    OnScreen->NewWFLabel  (4,TFT_COL_X(0), TFT_ROW_Y(3), F("Edit Tode"), WHEDITNODE);
    OnScreen->NewWFLabel  (5,TFT_COL_X(0), TFT_ROW_Y(4), F("Add Dev"),   WHADDDEV);
    OnScreen->NewWFLabel  (6,TFT_COL_X(0), TFT_ROW_Y(5), F("Del Dev"),   WHDELDEV);
    OnScreen->NewWFLabel  (7,TFT_COL_X(0), TFT_ROW_Y(6), F("Edit Dev"),  WHEDITDEV);
    OnScreen->NewWFLabel  (8,TFT_COL_X(0), TFT_ROW_Y(7), F("System"),    WHSYSTEM);

    OnScreen->DisplayScreen();
    return NAVKEYNONE;
  }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
uint8_t DitsUI::TodeSetup(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::TodeSetup(NavKey)\n"))
  uint16_t SetWH = OnScreen->SetValueWH;

  if(NavKey==NAVSETVAL)   { NavKey=NAVSETVAL; }
  if(NavKey==NAVKEYLEFT)  { SetupMenu(NAVDISPLAY); return NAVKEYNONE; }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSTODESETUP);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Edit Tode"));
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), pDits->DITNameChar, true, WHNODENAME);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("SecNet:"));
    OnScreen->NewWNumVal  (3,TFT_COL_X(7), TFT_ROW_Y(2), VDTBYTE, HEX, true, WHSECNET);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Radio:"));
    OnScreen->NewWEnumVal (0,TFT_COL_X(6), TFT_ROW_Y(3), false, WHRADIOID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("NetID:"));
    OnScreen->NewWNumVal  (4,TFT_COL_X(6), TFT_ROW_Y(4), VDTUINT, HEX, true, WHNETID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("RFAddr:"));
    OnScreen->NewWNumVal  (5,TFT_COL_X(7), TFT_ROW_Y(5), VDTUINT, HEX, true, WHRFADDR);

    OnScreen->AddEnum(WHRADIOID,0,F("NOTGOT"));
    OnScreen->AddEnum(WHRADIOID,1,F("UNKNOWN"));
    OnScreen->AddEnum(WHRADIOID,2,F("E22400T30D"));
    OnScreen->AddEnum(WHRADIOID,3,F("E22400T33D"));
    OnScreen->AddEnum(WHRADIOID,4,F("E22900T30D"));
    OnScreen->AddEnum(WHRADIOID,5,F("E32433T20D"));
    OnScreen->AddEnum(WHRADIOID,6,F("E32433T30D"));
    OnScreen->AddEnum(WHRADIOID,7,F("E220400T30D"));

    OnScreen->DisplayScreen();
    NavKey = NAVGETVALS;
  }
  if(NavKey==NAVSETVAL) {
    if(SetWH==WHNODENAME) { pDits->UpdateThisNodeName(OnScreen->chSetValue); }
    if(SetWH==WHSECNET)   { pDits->SecNetCode(OnScreen->SetValue); }
    if(SetWH==WHNETID)    { pEbyte->NetID(OnScreen->SetValue); }
    if(SetWH==WHRFADDR)   { pEbyte->Address(OnScreen->SetValue); }
    OnScreen->SetValueWH = WHNONE;
    NavKey=NAVGETSETVAL;
  }
  if(NavKey==NAVGETVALS || NavKey==NAVGETSETVAL) {
    bool bAll = (NavKey==NAVGETVALS);
    if(SetWH==WHNODENAME||bAll) {
      DITSEngine::NodeDIT node = pDits->Node(0); node.NGetName(tmpDITName);
      OnScreen->gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
    }
    if(SetWH==WHSECNET||bAll)   { OnScreen->gotValue(NSTODESETUP, WHSECNET, pDits->SecNetCode()); }
    if(SetWH==WHRADIOID||bAll)  { OnScreen->gotValue(NSTODESETUP, WHRADIOID, pEbyte->RadioID()); }
    if(SetWH==WHNETID||bAll)    { OnScreen->gotValue(NSTODESETUP, WHNETID, pEbyte->NetID()); }
    if(SetWH==WHRFADDR||bAll)   { OnScreen->gotValue(NSTODESETUP, WHRFADDR, pEbyte->Address()); }
    return NAVKEYNONE;
  }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
uint8_t DitsUI::AddDevice(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::AddDevice(NavKey)\n"))
  
  if(NavKey==NAVKEYLEFT)  { SetupMenu(NAVDISPLAY); return NAVKEYNONE; }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSADDDEV);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Add Device"),WHTITLE);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), pDits->DITNameChar, true, WHDEVNAME);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("Type:"));
    OnScreen->NewWEnumVal (3,TFT_COL_X(6), TFT_ROW_Y(2), true, WHDEVTYPE);
    //OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("SecNet:"));
    //OnScreen->NewWNumVal  (3,TFT_COL_X(7), TFT_ROW_Y(2), VDTBYTE, HEX, true, WHSECNET);
    //OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Radio:"));
    
    //OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("NetID:"));
    //OnScreen->NewWNumVal  (4,TFT_COL_X(6), TFT_ROW_Y(4), VDTUINT, HEX, true, WHNETID);
    //OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("RFAddr:"));
    //OnScreen->NewWNumVal  (5,TFT_COL_X(7), TFT_ROW_Y(5), VDTUINT, HEX, true, WHRFADDR);

    OnScreen->AddEnum(WHDEVTYPE,DT_ONOFF,F("OnOff"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DIST,F("Distance"));
    OnScreen->AddEnum(WHDEVTYPE,DT_STSTP3W,F("StartStop3W"));
    OnScreen->AddEnum(WHDEVTYPE,DT_ANAINPUT,F("AnalogIn"));
    OnScreen->AddEnum(WHDEVTYPE,DT_ANAOUTPUT,F("AnalogOut"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DLSETPOINT,F("DLSetPoint"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DLMATH,F("DLMath"));

    OnScreen->DisplayScreen();
    NavKey = NAVGETVALS;
  }
}