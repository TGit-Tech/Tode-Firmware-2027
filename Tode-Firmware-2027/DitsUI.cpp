#include "DitsUI.h"

void DitsUI::Begin(DITSEngine& _pDits, Adafruit_GFX& _pGFX, TGTEbyte& _pEbyte)
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
bool DitsUI::gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, uint16_t _Value) {
  if(OnScreen->NScrIdx!=_NScrIdx) return false;
  OnScreen->gotValue(_NScrIdx,_WHandleUID,_Value);
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
      OnScreen->NewWChLabel(0,TFT_COL_X(0), TFT_ROW_Y(row),pDits->DITNameChar,tmpDITName); // Label
      switch(dev.DevType()) {
        case DT_ONOFF:
        case DT_STSTP3W:
          OnScreen->NewWEnumVal (row+1,TFT_COL_X(10), TFT_ROW_Y(row), true, dev.DevUID());
          OnScreen->AddEnum(dev.DevUID(),0,F("OFF"));
          OnScreen->AddEnum(dev.DevUID(),1,F("ON"));
          break;
        case DT_DIST:
          OnScreen->NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row), TUINT+TDEC,dev.DevUID());
          break;
        case DT_ANAINPUT:
          OnScreen->NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC,dev.DevUID());
          break;
        case DT_ANAOUTPUT:
          OnScreen->NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC+TSETPD,dev.DevUID());
          break;
        case DT_DLSETPOINT:
          OnScreen->NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC+TSETPD,dev.DevUID());
          break;
        case DT_DLMATH:
          OnScreen->NewWNumVal( row+1,TFT_COL_X(10), TFT_ROW_Y(row),  TUINT+TDEC,dev.DevUID());
          break;
      }
    }
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
      case WHTODESETUP: TodeSetup(NAVDISPLAY); return NAVKEYNONE;
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
    OnScreen->NewWFLabel  (4,TFT_COL_X(0), TFT_ROW_Y(3), F("Tode Setup"), WHTODESETUP);
    OnScreen->NewWFLabel  (5,TFT_COL_X(0), TFT_ROW_Y(4), F("Add Dev"),   WHADDDEV);
    OnScreen->NewWFLabel  (6,TFT_COL_X(0), TFT_ROW_Y(5), F("Del Dev"),   WHDELDEV);
    OnScreen->NewWFLabel  (7,TFT_COL_X(0), TFT_ROW_Y(6), F("Edit Dev"),  WHEDITDEV);
    OnScreen->NewWFLabel  (8,TFT_COL_X(0), TFT_ROW_Y(7), F("System"),    WHSYSTEM);

    return NAVKEYNONE;
  }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
uint8_t DitsUI::TodeSetup(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::TodeSetup(NavKey)\n"))
  uint16_t SelectedWH = OnScreen->SelectedWHandleUID();

  if(NavKey==NAVSETVAL)   { NavKey=NAVSETVAL; }
  if(NavKey==NAVKEYLEFT)  { SetupMenu(NAVDISPLAY); return NAVKEYNONE; }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSTODESETUP);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Tode Setup"));
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), pDits->DITNameChar, true, WHNODENAME);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("SecNet:"));
    OnScreen->NewWNumVal  (3,TFT_COL_X(7), TFT_ROW_Y(2), TBYTE+THEX+TSETPD, WHSECNET);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Radio:"));
    OnScreen->NewWEnumVal (0,TFT_COL_X(6), TFT_ROW_Y(3), false, WHRADIOID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("NetID:"));
    OnScreen->NewWNumVal  (4,TFT_COL_X(6), TFT_ROW_Y(4), TUINT+THEX+TSETPD, WHNETID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("RFAddr:"));
    OnScreen->NewWNumVal  (5,TFT_COL_X(7), TFT_ROW_Y(5), TUINT+THEX+TSETPD, WHRFADDR);

    OnScreen->AddEnum(WHRADIOID,0,F("NOTGOT"));
    OnScreen->AddEnum(WHRADIOID,1,F("UNKNOWN"));
    OnScreen->AddEnum(WHRADIOID,2,F("E22400T30D"));
    OnScreen->AddEnum(WHRADIOID,3,F("E22400T33D"));
    OnScreen->AddEnum(WHRADIOID,4,F("E22900T30D"));
    OnScreen->AddEnum(WHRADIOID,5,F("E32433T20D"));
    OnScreen->AddEnum(WHRADIOID,6,F("E32433T30D"));
    OnScreen->AddEnum(WHRADIOID,7,F("E220400T30D"));

    NavKey = NAVGETVALS;
  }
  if(NavKey==NAVSETVAL) {
    if(SelectedWH==WHNODENAME) { pDits->UpdateThisNodeName(OnScreen->SelectedWchValue()); }
    if(SelectedWH==WHSECNET)   { pDits->SecNetCode(OnScreen->SelectedWValue()); }
    if(SelectedWH==WHNETID)    { pEbyte->NetID(OnScreen->SelectedWValue()); }
    if(SelectedWH==WHRFADDR)   { pEbyte->Address(OnScreen->SelectedWValue()); }
    NavKey=NAVGETSETVAL;
  }
  if(NavKey==NAVGETVALS || NavKey==NAVGETSETVAL) {
    bool bAll = (NavKey==NAVGETVALS);
    if(SelectedWH==WHNODENAME||bAll) {
      DITSEngine::NodeDIT node = pDits->Node(0); node.NGetName(tmpDITName);
      OnScreen->gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
    }
    if(SelectedWH==WHSECNET||bAll)   { OnScreen->gotValue(NSTODESETUP, WHSECNET, pDits->SecNetCode()); }
    if(SelectedWH==WHRADIOID||bAll)  { OnScreen->gotValue(NSTODESETUP, WHRADIOID, pEbyte->RadioID()); }
    if(SelectedWH==WHNETID||bAll)    { OnScreen->gotValue(NSTODESETUP, WHNETID, pEbyte->NetID()); }
    if(SelectedWH==WHRFADDR||bAll)   { OnScreen->gotValue(NSTODESETUP, WHRFADDR, pEbyte->Address()); }
    return NAVKEYNONE;
  }
  return NavKey;
}
//------------------------------------------------------------------------------------------------
uint8_t DitsUI::AddDevice(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::AddDevice(NavKey)\n"))
  uint16_t SelectedWH = OnScreen->SelectedWHandleUID();

  if(NavKey==NAVKEYLEFT)  { SetupMenu(NAVDISPLAY); return NAVKEYNONE; }
  if(NavKey==NAVKEYGET) {
    if(SelectedWH==WHADDBTN) {
      DBUIAAINFO((Add_devType),(tmpDITName),("DitsUI::AddDevice pDits->AddThisNodeDevice(devType, Name)"))
      uint8_t AdddevUID = pDits->AddThisNodeDevice(Add_devType, 0x00, tmpDITName);
      if(AdddevUID!=BNONE) {
        SETGSDigital devsetgs(AdddevUID);
        devsetgs.Pin1(Add_IOPin);
        devsetgs.Pin1Mode(Add_pinMode);
        devsetgs.Pin2Mode(Add_Active);
        OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(9), F("...Device Added!"));
        return NAVNEWDEVICE;
      }
    }
  }

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSADDDEV);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Add Device"),WHTITLE);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), pDits->DITNameChar, true, WHDEVNAME);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("Type:"));
    OnScreen->NewWEnumVal (3,TFT_COL_X(5), TFT_ROW_Y(2), true, WHDEVTYPE);

    OnScreen->AddEnum(WHDEVTYPE,DT_ONOFF,F("OnOff"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DIST,F("Distance"));
    OnScreen->AddEnum(WHDEVTYPE,DT_STSTP3W,F("StartStop3W"));
    OnScreen->AddEnum(WHDEVTYPE,DT_ANAINPUT,F("AnalogIn"));
    OnScreen->AddEnum(WHDEVTYPE,DT_ANAOUTPUT,F("AnalogOut"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DLSETPOINT,F("DLSetPoint"));
    OnScreen->AddEnum(WHDEVTYPE,DT_DLMATH,F("DLMath"));
    
    // Use tmpDITName for new name.
    memset(tmpDITName, 0, pDits->DITNameChar);
    OnScreen->gotValue(NSADDDEV, WHDEVNAME, tmpDITName);

    NavKey = NAVGETVALS;
  }
  if(NavKey==NAVSETVAL) {
    if(SelectedWH==WHDEVTYPE) {
      if(Add_devType!=DT_NOTSET) { /*Clear Bottom part of Screen*/ }
      Add_devType = OnScreen->SelectedWValue();
      if(Add_devType==DT_ONOFF) {
        OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Pin:"));
        OnScreen->NewWNumVal  (4,TFT_COL_X(4), TFT_ROW_Y(3), TBYTE+TDEC+TSET, WHIOPIN,3,54);
        OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("pinMode:"));
        OnScreen->NewWEnumVal (5,TFT_COL_X(8), TFT_ROW_Y(4), true, WHPINMODE);
        OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("Active:"));
        OnScreen->NewWEnumVal (6,TFT_COL_X(7), TFT_ROW_Y(5), true, WHACTIVE);
        OnScreen->NewWFLabel  (7,TFT_COL_X(5), TFT_ROW_Y(6), F("[ADD]"),WHADDBTN);

        OnScreen->AddEnum(WHACTIVE,ACTIVEHION,F("HiON"));
        OnScreen->AddEnum(WHACTIVE,ACTIVELOWON,F("LoON"));
        OnScreen->AddEnum(WHPINMODE,INPUT,F("IN"));
        OnScreen->AddEnum(WHPINMODE,OUTPUT,F("OUT"));
        OnScreen->AddEnum(WHPINMODE,INPUT_PULLUP,F("INUP"));
        return NAVKEYNONE;
      }
    }
    if(SelectedWH==WHIOPIN) {Add_IOPin=OnScreen->SelectedWValue();}
    if(SelectedWH==WHPINMODE) {Add_pinMode=OnScreen->SelectedWValue();}
    if(SelectedWH==WHACTIVE) {Add_Active=OnScreen->SelectedWValue();}
  }
}