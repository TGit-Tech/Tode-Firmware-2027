/******************************************************************************************************************/ /**
 * @file    DITImplement.h
 * @brief   RF-DITS Engine Configuration.
 *********************************************************************************************************************/
#pragma once

#include "Config.h"
#include <rf_dits.h>        // RF-DITS protocol engine
#include "LDevices.h"       // LDevices
#include <TGTDB.h>

#define RAMEEPROM           // Use RAM as EEPROM

// Notice - Changing these will make previous records void
#define STARTADDR     0
#define MAXDITRECORDS 40
#define NAMEBYTES     10

#define MAXREMVALS 50
#define RDEVLOADED 0x01

//--------------------------------------------------
#if defined(RAMEEPROM)
  #define PMEMREAD(a) RAM_EEPROM[(a)]
  #define PMEMWRITE(a,v) RAM_EEPROM[(a)]=(v)
#else
  #include <EEPROM.h>
  #define PMEMREAD(a) EEPROM.read((a))
  #define PMEMWRITE(a,v) EEPROM.update((a),(v))
#endif

///@class   DITImplement
///@brief   A Serial RF-DITS engine using LDevices* for local device get/set and RNode Values Buffer.
///@details Responsibilities:
///     - ProcLoop() calls SPort->RxData, LDevProcLoop() & this->ProcessLoop().
///     - Remote Node Values are stored in RNodeIdx, RdevUID[], RdevVal[] alloc to MAXREMVALS
///     - ProcLoop() returns RDEVLOADED when RNode values buffer has values.
///     - Call RdevValReset() after reading buffered RNode values.
///     - RNode values are buffer loaded by packet loop.  i.e. It has all values or nothing.
//-------------------------------------------------------------------------------------------------
class DITImplement : public DITSEngine {
  public:
    
    DITImplement(HardwareSerial& SerialPort, LDevices& _LDev) 
    : DITSEngine(STARTADDR,MAXDITRECORDS,NAMEBYTES), SPort(SerialPort), LDev(_LDev) {
    //DITSEngine(uint16_t _pmemBeginAddr, byte _MaxDITRecords, byte _NameFieldBytes);
      #if defined(RAMEEPROM)
        memset(RAM_EEPROM, 0xFF, sizeof(RAM_EEPROM));
      #endif
      tmpname = new char[NAMEBYTES];
    }
    
    void LoadDevices() {
      DITSEngine::DeviceDIT dev = this->Device();
      while (dev.Next(0)) { 
        LDev.LoadDevice(dev.DevUID(),dev.DevType()); 
      }
    }
    void setRadioChannel(uint8_t _RadioChannel) {RadioChannel=_RadioChannel;}
    // Remote Device Values Buffer
    uint8_t RNodeIdx = 0;
    void RdevValReset() { RNodeIdx=0; for(uint8_t i=0; i<MAXREMVALS; i++){RdevUID[i]=0;RdevVal[i]=0;} }

    uint8_t ProcLoop() {
      while (SPort.available()>0) {
        uint8_t Rx_byte = SPort.read();
        DBLAINFO((Rx_byte,HEX),("Rx "));
        this->RxData(Rx_byte);
      } // Serial Bytes to RF-Dits
      this->ProcessLoop();                                      // RF-DITS Process Loop
      LDev.LDevProcLoop();                                     // LDevProc Loop
      return (Rdevidx!=0) ? RDEVLOADED : BNONE;                 // Return if Remote Values are Loaded
    }
  protected:
    virtual int RxReqDeviceValue(byte _DevUID) override { 
      uint16_t RtnValue = LDev.Value(_DevUID);
      DBHAAINFO((_DevUID,HEX),(RtnValue),("DITImplement::RxReqDeviceValue LDev.Value(_DevUID)=(RtnValue)\n"))
      return RtnValue;
    }
    virtual void RxReqDeviceSet(byte _DevUID, int value) override { 
      LDev.Value(_DevUID,value);
      DBHAAINFO((_DevUID,HEX),(value),("DITImplement::RxReqDeviceSet LDev.Value(_DevUID,value)\n")) 
    }
    virtual void RxDataDevValue(byte _NodeIdx, byte _DevUID, int value) override {
      // Buffer supplied values of a remote node.
      // Acceptance is flagged by setting RNodeIdx=0
      if(Rdevidx>=MAXREMVALS) return;
      if(RNodeIdx!=_NodeIdx && RNodeIdx!=0) return;
      RNodeIdx = _NodeIdx;
      RdevUID[Rdevidx] = _DevUID;
      RdevVal[Rdevidx] = value;
      Rdevidx++;
    }
    virtual bool TxReady(uint16_t _ToRFAddr) override {
      if(SPort.availableForWrite()<58) return false;
      DBLAAINFO((_ToRFAddr,HEX),(RadioChannel),("TxReady(ToRFAddr,RadioChannel)"))
      SPort.write(highByte(_ToRFAddr));SPort.write(lowByte(_ToRFAddr));SPort.write(RadioChannel); // Comment out for Jumper Test (Radio Only)
      return true;
    }
    virtual void TxData(byte _TxByte) override { DBLINFOSP((_TxByte,HEX)) SPort.write(_TxByte); }
    virtual byte pmem_read(int addr) override { return PMEMREAD(addr); }
    virtual void pmem_write(int addr, byte val) override { PMEMWRITE(addr, val); }
  private:
    HardwareSerial& SPort;
    LDevices& LDev;
    #if defined(RAMEEPROM)
      byte RAM_EEPROM[0x0300];
    #endif
    uint8_t RdevUID[MAXREMVALS] = {0};
    uint16_t RdevVal[MAXREMVALS] = {0};
    uint8_t Rdevidx = 0;
    char* tmpname; // General use array for DIT get Names
    uint8_t RadioChannel = 0;
};