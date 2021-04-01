#ifndef RD53A_REGISTERFRAME_H
#define RD53A_REGISTERFRAME_H

#include "RD53Emulator/Frame.h"

namespace RD53A{

/**
 * RD53A RegisterFrame is a type of Frame (type-e) from the output data path of the RD53A.
 * One RegisterFrame is interleaved every N DataFrame frames, where
 * N is configurable by the user (Configuration::SKP_TRIG_CNT), cannot be disabled, and has a default value.
 *
 * A 64-bit RegisterFrame contains the value of 2 16-bit registers,
 * (RegisterFrame::GetValue),
 * 2 10-bit addresses (RegisterFrame::GetAddress),
 * an 8-bit aurora code, and a 4-bit status.
 * Addresses from 0 to 511 are global register. From 512 to 895 are offset pixel row numbers.
 * The Aurora code (RegisterFrame::GetAuroraCode) indicates if any of the two
 * registers is from an RdReg Command, or is an auto-read.
 * The status code (RegisterFrame::GetStatus) indicates possible errors during the reading of the registers.
 *
 * Possible Aurora codes are:
 *
 * Code | Description
 * ---- | -----------
 * 0xB4 | Both register fields are of type auto-read
 * 0x55 | First frame is auto-read, second is from a read register command
 * 0x99 | First is from a read register command, second frame is auto-read
 * 0xD2 | Both register fields are from read register commands
 * 0xCC | Indicates an error. Fields are meaningless
 *
 * Possible status codes are:
 * 
 * Code | Description
 * ---- | -----------
 * 0x0  | Ready
 * 0x1  | There has been an error since the last register frame 
 * 0x2  | There has been a warning since the last register frame 
 * 0x3  | Both 1 and 2
 * 0x4  | Ready
 * 0x5  | Trigger queue is full (busy)
 * 0x6  | No input channel lock
 * 0x7  | Spare
 * ...  | Spare
 * 0xF  | Spare
 *
 * The format of the Register frame is the following:
 *
 * | Bit  | 63 | 56 | 55 | 52 | 51 | 48 | 47 | 42 | 41 | 40 | 39 | 32 | 31 | 26 | 25 | 24 | 23 | 16 | 15 | 8 | 7 | 0 |
 * | ---  | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | - | - | - |
 * | Rel  |  7 |  0 |  7 |  4 |  3 |  0 |  7 |  2 |  1 |  0 |  7 |  0 |  7 |  2 |  1 |  0 |  7 |  0 |  7 | 0 | 7 | 0 |
 * | Byte |  0     ||  1             ||||  2             ||||  3     ||  4             ||||  5     ||  6    || 7    ||
 * | Desc | Aurora || Status || Address 1      |||| Value 1                |||||| Address 2      |||| Value 2     ||||
 * | Size | 8      || 4      || 10             |||| 16                     |||||| 10             |||| 16          ||||
 *
 * @brief RD53A RegisterFrame
 * @author Carlos.Solans@cern.ch
 * @date March 2021
 **/

class RegisterFrame: public Frame{
  
 public:

  static const uint32_t AURORA_AUTO_AUTO=0xB4;     /**< Both register fields are of type auto-read **/
  static const uint32_t AURORA_AUTO_READ=0x55;     /**< First frame is auto-read, second is from a read register command **/
  static const uint32_t AURORA_READ_AUTO=0x99;     /**< First is from a read register command, second frame is auto-read **/
  static const uint32_t AURORA_READ_READ=0xD2;     /**< Both register fields are from read register commands **/
  static const uint32_t AURORA_ERROR=0xCC;         /**< Indicates an error. Fields are meaningless **/

  static const uint32_t STATUS_READY=0;             /**< Ready **/
  static const uint32_t STATUS_ERROR=1;             /**< There has been an error since the last register frame **/
  static const uint32_t STATUS_WARNING=2;           /**< There has been an warning since the last register frame **/
  static const uint32_t STATUS_ERROR_AND_WARNING=3; /**< There has been an error and a warning since the last register frame **/
  static const uint32_t STATUS_BUSY=5;              /**< Trigger queue is full (busy) **/
  static const uint32_t STATUS_NOLOCK=6;            /**< No input channel lock **/

  /**
   * Default constructor. Create an empty RegisterFrame with all zeros.
   **/
  RegisterFrame();

  /**
   * Copy constructor. Create a new RegisterFrame by copying the contents from copy
   * @param copy RegisterFrame to use as a reference for the new one
   **/
  RegisterFrame(RegisterFrame *copy);

  /**
   * All in one constructor. Create a RegisterFrame with the contents provided
   * @param code The 8-bit Aurora code (0xB4, 0x55, 0x99, 0xD2, 0xCC).
   * @param status The 4-bit status code.
   * @param address1 The 10-bit address of the first register
   * @param value1 The 16-bit value of the first register
   * @param address2 The 10-bit address of the second register
   * @param value2 The 16-bit value of the second register
   **/
  RegisterFrame(uint32_t code, uint32_t status, uint32_t address1, uint32_t value1, uint32_t address2, uint32_t value2);

  /**
   * Actually empty destructor
   */
  ~RegisterFrame();

  /**
   * Return a human readable representation of the register
   * @return The human readable representation of the register
   **/
  std::string ToString();
  
  /**
   * Extract the contents of this command from the bytes
   * @param bytes the byte array
   * @param maxlen the maximum number of bytes than can be read
   * @return the number of bytes processed
   **/
  uint32_t UnPack(uint8_t * bytes, uint32_t maxlen);
  
  /**
   * Set the contents of this command to bytes
   * @param bytes the byte array
   * @return the number of bytes processed
   **/
  uint32_t Pack(uint8_t * bytes);

  /**
   * Get the aurora header
   * @return the aurora header in byte form
   **/
  uint8_t GetAuroraHeader();

  /**
   * Get the type of command
   * @return the type of the command
   **/
  uint32_t GetType();

  /**
   * Set the Aurora code of the frame. 
   * Possible values are 0xB4, 0x55, 0x99, 0xD2, 0xCC.
   * @param code Aurora code to set
   **/
  void SetAuroraCode(uint32_t code);

  /**
   * Get the Aurora code
   * @return the Aurora code in byte form
   **/
  uint32_t GetAuroraCode();

  /**
   * Get the Aurora code
   * @return the Aurora code meaning as text
   **/
  std::string GetAuroraCodeString();

  /**
   * Set the status code of the frame. 
   * Possible values are from 0 to 15.
   * @param status the status code to set
   **/
  void SetStatus(uint32_t status);

  /**
   * Get the status code
   * @return the status code in byte form
   **/
  uint32_t GetStatus();
  
  /**
   * Get the status code
   * @return the status code in text form
   **/
  std::string GetStatusString();

  /**
   * Set the error flag of the frame encoded into the status code.
   * @param enable set the error flag if true
   **/
  void SetError(bool enable);

  /**
   * Get the error flag of the frame encoded into the status code.
   * @return the error flag 
   **/
  bool GetError();

  /**
   * Set the auto-read flag for each register (pos) encoded into the status code.
   * @param pos the position of the register (0 or 1)
   * @param enable set the auto-read flag if true
   **/
  void SetAuto(uint32_t pos, bool enable);

  /**
   * Get the auto-read flag for each register (pos) encoded into the status code.
   * @param pos the position of the register (0 or 1)
   * @return the auto-read flag 
   **/
  bool GetAuto(uint32_t pos);

  /**
   * Set the address (addr) and value (val) of the register 
   * for a given position (pos) in one go.
   * @param pos the position of the register (0 or 1)
   * @param addr the address of the register (10-bit)
   * @param val the value of the register (16-bit)
   * @param autoread True of the register is autoread
   **/
  void SetRegister(uint32_t pos, uint32_t addr, uint32_t val, bool autoread=false);

  /**
   * Set the address (addr) of the register 
   * for a given position (pos).
   * @param pos the position of the register (0 or 1)
   * @param addr the address of the register (10-bit)
   **/
  void SetAddress(uint32_t pos, uint32_t addr);

  /**
   * Set the value (val) of the register 
   * for a given position (pos).
   * @param pos the position of the register (0 or 1)
   * @param val the value of the register (16-bit)
   **/
  void SetValue(uint32_t pos, uint32_t val);

  /**
   * Get the address (addr) of the register 
   * for a given position (pos).
   * @param pos the position of the register (0 or 1)
   * @return the address of the register (10-bit)
   **/
  uint32_t GetAddress(uint32_t pos);

  /**
   * Get the value (val) of the register 
   * for a given position (pos).
   * @param pos the position of the register (0 or 1)
   * @return the value of the register (16-bit)
   **/
  uint32_t GetValue(uint32_t pos);

  
 protected:

  void Pack();
  bool UnPack();

  uint8_t m_acode;
  uint8_t m_status;
  uint16_t m_addr[2];
  uint16_t m_vals[2];
  bool m_auto[2];
  bool m_err;
  static const char * const m_status2text[];
  static std::map<uint32_t,const char*> m_acode2text;

};

}

#endif 
