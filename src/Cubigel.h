/*******************************************************************************************************************
** This program defines the Cubigel class header. The Cubigel compressors that support the FDC1 protocol can be   **
** accessed using this library. The detailed protocol description can be downloaded as a PDF file from            **
** https://www.sv-zanshin.com/r/manuals/cubigel_fdc1_communication_protocol.pdf                                   **
**                                                                                                                **
** The most recent version of the library is at https://github.com/SV-Zanshin/Cubigel/archive/master.zip and the  **
** library wiki is to be found https://github.com/SV-Zanshin/Cubigel/wiki.                                        **
**                                                                                                                **
** This library uses the Arduino SoftwareSerial library, and it is important to only use those pins which support **
** pin change interrupts for the fridge and freezer inputs. A good description of the pins and their uses can be  **
** found at https://www.arduino.cc/en/Reference/AttachInterrupt.                                                  **
**                                                                                                                **
** While the Cubigel transmits data every 0.5 seconds, this library is designed to use its own interrupt to read  **
** the data and to compile statistics until such time as the calling program is ready to consume it. The Arduino  **
** SoftwareSerial library is used, but as it only supports a single port at a time it will not work if more than  **
** one device uses the library. This library supports the use of both hardware and software serial ports, and if  **
** more than one device is used then at most one port can use the SoftwareSerial library/class. The pointer to    **
** the appropriate class instance is passed in to this library during class instantiation and the constructor is  **
** overloaded to reflect the various options available.                                                           **
**                                                                                                                **
** Although programming for the Arduino and in c/c++ is new to me, I'm a professional programmer and have learned,**
** over the years, that it is much easier to ignore superfluous comments than it is to decipher non-existent ones;**
** so both my comments and variable names tend to be verbose. The code is written to fit in the first 80 spaces   **
** and the comments start after that and go to column 117 - allowing the code to be printed in A4 landscape mode. **
** There are several parts of code which can be somewhat optimized, but in order to make the c++ code more under- **
** standable by non-programmers some performance has been sacrificed for legibility and maintainability.          **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.2  2017-08-21 https://github.com/SV-Zanshin Removed extraneous code, changed comments                      **
** 1.0.1  2017-07-31 https://github.com/SV-Zanshin Prototypes contain optional parameter definitions, the         **
**                                                     functions no longer have them declared as non-Windows      **
**                                                     compilers fail when they do                                **
** 1.0.0  2017-04-19 https://github.com/SV-Zanshin Cleaned up debugging code and ready for first release          **
** 1.0.b1 2017-04-18 https://github.com/SV-Zanshin Added hardware serial in addition to software serial           **
** 1.0.b2 2017-02-21 https://github.com/SV-Zanshin Initial coding                                                 **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include "SoftwareSerial.h"                                                   // Software serial port emulation   //
#ifndef Cubigel_h                                                             // Guard code definition            //
  #define Cubigel_h                                                           // Define the name inside guard code//
  const uint16_t CUBIGEL_BAUD_RATE   = 1200;                                  // Cubigel has a fixed baud rate    //
  const uint8_t  CUBIGEL_MAX_DEVICES =    2;                                  // Max number of devices supported  //
  const uint8_t  MODE_DEFAULT        =    0;                                  // Default output mode              //
  const uint8_t  MODE_SETTINGS       =    1;                                  // Output settings mode             //
  /*****************************************************************************************************************
  ** This structure contains all of the variables stored per Cubigel device                                       **
  *****************************************************************************************************************/
  typedef struct {                                                            // Structure definition             //
    HardwareSerial *serialHW;                                                 // Pointer to HardwareSerial        //
    SoftwareSerial *serialSW;                                                 // Pointer to SoftwareSerial        //
    uint8_t  buffer[22];                                                      // Data buffer for device           //
    uint8_t  index;                                                           // Current index into data buffer   //
    uint16_t readings;                                                        // Number of readings stored        //
    uint32_t totalRPM;                                                        // Sum of all RPM values            //
    uint32_t totalmA;                                                         // Sum of all Milliamps values      //
    uint8_t  errorStatus;                                                     // OR'd values of all errors found  //
    uint16_t commsErrors;                                                     // Number of communications errors  //
    uint16_t minSpeed;                                                        // Minimum speed setting            //
    uint16_t maxSpeed;                                                        // Maximum speed setting            //
    uint16_t cutOut12V;                                                       // 12V Cut out voltage              //
    uint16_t cutIn12V;                                                        // 12V Cut in  voltage              //
    uint16_t cutOut24V;                                                       // 24V Cut out voltage              //
    uint16_t cutIn24V;                                                        // 24V Cut in  voltage              //
    uint16_t cutOut42V;                                                       // 42V Cut out voltage              //
    uint16_t cutIn42V;                                                        // 42V Cut in  voltage              //
    uint8_t  modeByte;                                                        // Mode setting switches            //
    bool     timeChanged;                                                     // Set to true when turned ON/OFF   //
    uint32_t onTime;                                                          // Last millis() for an ON event    //
    uint32_t offTime;                                                         // Last millis() for an OFF event   //
  } CubigelDataType; // of CubigelDataType declaration                        //                                  //
  /*****************************************************************************************************************
  ** Define the class public and private members                                                                  **
  *****************************************************************************************************************/
  class CubigelClass {                                                        //----------------------------------//
    public:                                                                   // Publicly visible class members   //
                                                                              //----------------------------------//
                                                                              // Overloaded Class Constructors    //
      CubigelClass(SoftwareSerial *serial1);                                  // 1 Software device                //
      CubigelClass(HardwareSerial *serial1);                                  // 1 Hardware device                //
      CubigelClass(HardwareSerial *serial1, SoftwareSerial *serial2);         // 2 devices - HW then SW           //
      CubigelClass(SoftwareSerial *serial1, HardwareSerial *serial2);         // 2 devices - SW then HW           //
      CubigelClass(HardwareSerial *serial1, HardwareSerial *serial2);         // 2 devices - HW and HW            //
                                                                              //----------------------------------//
                                                                              // Overloaded readValues definitions//
                                                                              //----------------------------------//
      uint16_t readValues(const uint8_t idx, uint16_t &RPM, uint16_t &mA,     // Just return RPM and mA           //
                          const bool resetReadings=true);                     //                                  //
      uint16_t readValues(const uint8_t idx, uint16_t &RPM, uint16_t &mA,     // return number of readings and    //
                          uint16_t &commsErrors, uint16_t errorStatus,        // update parameters with values    //
                          const bool resetReadings=true);                     //                                  //
                                                                              //----------------------------------//
      void readSettings(const uint8_t idx, uint16_t &compMin,                 // Return the settings values       //
                           uint16_t &compMax, uint16_t &out12V,               //                                  //
                           uint16_t &in12V, uint16_t &out24V,                 //                                  //
                           uint16_t &in24V,uint16_t &out42V,                  //                                  //
                           uint16_t &in42, uint8_t &mode);                    //                                  //
      void requestSettings(const uint8_t idx);                                // Request a settings measurement   //
      bool readTiming(const uint8_t idx,uint32_t &onTime,uint32_t &offTime);  // return the last state changes    //
      static void TimerISR();                                                 // Interim ISR calls real handler   //
                                                                              //----------------------------------//
    private:                                                                  // Declare private class members    //
      void setMode(const uint8_t idx, const uint8_t mode);                    // Set Cubigel FDC1 mode            //
      void                 StartTimer();                                      // set the interrupt vector         //
      void                 processDevice(const uint8_t deviceNumber);         // read and store data for a device //
      void                 TimerHandler();                                    // Called every millisecond for fade//
      static CubigelClass* ClassPtr;                                          // store pointer to class itself    //
      bool    _freezerPresent = false;                                        // Switch denoting if a freezer set //
      uint8_t _deviceCount   = 0;                                             // Number of devices instantiated   //
      volatile CubigelDataType devices[CUBIGEL_MAX_DEVICES];                  // Declare storage for max devices  //
  }; // of class header definition for CubigelClass                           //----------------------------------//
#endif