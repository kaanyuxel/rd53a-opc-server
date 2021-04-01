#ifndef RD53A_RDREG_H
#define RD53A_RDREG_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * Initiate the readout of the addressed register given a chip ID.
 * Chip ID is 3-bit plus 1 bit for broadcast (RdReg::SetChipId, RdReg::GetChipId).
 * The address is 9 bits (RdReg::SetAddress, RdReg::GetAddress).
 *
 * The symbol identifier of this command is 0x6565, and is followed by two 16-bit frames.
 * Command uses 48 bits:
 *
 * Bit     |  0 | 15 | 16 | 23 | 24 | 31 | 32 | 39 | 40 | 47 |
 * ----    | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
 * Byte    |  0-1   ||  2     ||  3     ||  4     ||  5     ||
 * Payload | 0x6565 || 0  | 4  | 5  | 9  | 10 | 14 | 15 | 19 |
 *
 * The Payload in this case is the following:
 *
 * Payload |  0 |  3 |  4 |  5 |  9 | 10 | 13 | 14 | 15 | 19 |
 * ------- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
 * Rel     |  0 |  3 |  4 |  0 |  4 |  0 |  3 |  4 |  0 |  4 |
 * Byte    |  2         |||  3     ||  4         |||  5     ||
 * Descrip | ChipID ||  0 |  Address       |||| 0          |||
 * Size    |  3     ||  1 |  9             |||| 16         |||
 *
 * @brief RD53A Read Register Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class RdReg: public Command{
  
 public:
  
  /**
   * Create a read register command
   **/
  RdReg();
  
  /**
   * Create a RdReg from another one
   * @param copy RdReg to copy
   **/
  RdReg(RdReg * copy);

  /**
   * Create a read register Command with parameters
   * @param chipid Chip ID [0:3] + bcast [4]
   * @param address the address [0:9]
   **/
  RdReg(uint32_t chipid, uint32_t address);

  /**
   * Empty destructor just for completeness.
   **/
  ~RdReg();
  
  /**
   * Clone this RdReg
   * @return A copy of this RdReg
   **/
  RdReg * Clone();

  /**
   * Return a human readable representation of the symbol
   * @return The human readable representation of the symbol
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
   * Get the type of command
   * @return the type of the command
   **/
  uint32_t GetType();

  /**
   * Set the chipid for the pulse
   * @param chipid Chip ID [0:3] + bcast [4]
   **/
  void SetChipId(uint32_t chipid);

  /**
   * Get the chipid for the pulse
   * @return chipid Chip ID [0:3] + bcast [4]
   **/
  uint32_t GetChipId();

  /**
   * Set the address for the readout
   * @param address the address [0:9]
   **/
  void SetAddress(uint32_t address);

  /**
   * Get the address for the readout
   * @return the address [0:9]
   **/
  uint32_t GetAddress();


 private:

  uint32_t m_symbol;
  uint32_t m_chipid;
  uint32_t m_address;

};

}
#endif
