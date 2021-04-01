#ifndef RD53A_WRREG_H
#define RD53A_WRREG_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * Command to write to a register given the address and chip ID.
 * Chip ID is 3-bit plus 1 bit for broadcast (WrReg::SetChipId, WrReg::GetChipId),
 * the address is 9 bits (WrReg::SetAddress, WrReg::GetAddress), and
 * the payload is either one in mode 0 or 6 16-bit values in mode 1 (WrReg::SetValue, WrReg::GetValue)
 * by specifying the mode bit (WrReg::SetMode, WrReg::GetMode).
 * The symbol identifier of this command is 0x6666.
 * Note the payload is encoded by a custom 5 to 8 bits protocol.
 *
 * The Command encoding uses 30 bits for 1 value, and 110 bits for 6 values.
 * In the case of 1 value it looks like the following:
 *
 * 
 * Bit     |  0 | 15 | 16 | 23 | 24 | 31 | 32 | 39 | 40 | 47 | 48 | 55 | 56 | 63 |
 * ----    | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
 * Byte    |  0-1   ||  2     ||  3     ||  4     ||  5     ||  6     ||  7     ||
 * Payload | 0x6666 || 0  | 4  | 5  | 9  | 10 | 14 | 15 | 19 | 20 | 24 | 25 | 29 |
 *
 * The Payload in this case is the following:
 *
 * Payload |  0 |  3 |  4 |  5 |  9 | 10 | 13 | 14 | 15 | 19 | 20 | 24 | 25 | 29 |
 * ------- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
 * Rel     |  0 |  3 |  4 |  0 |  4 |  0 |  3 |  4 |  0 |  4 |  0 |  4 |  0 |  4 |
 * Byte    |  2         |||  3     ||  4         |||  5     ||  6     ||  7     ||
 * Descrip | ChipID ||  0 |  Address       |||| Value                      |||||||
 * Size    |  3     ||  1 |  9             |||| 16                         |||||||
 *
 * In the case of 6 values it looks like the following:
 *
 * Bit     |0|7|8|15 |16|23|24|31|32|39|40|47|48|55|56|63|64|71|72|79|80|87|88|95|96|103|104|111|112|119|120|127|128|135|136|143|144|151|152|159|160|167|168|175|176|183|184|191|
 * ---     |-|-|-|-- |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
 * Byte    |0 ||1   || 2  || 3  || 4  || 5  || 6  || 7  || 8  || 9  ||10  ||11  ||12   || 13   || 14   || 15   || 16   || 17   || 18   || 19   || 20   || 21   || 22   || 23   ||
 * Payload |0x6666|||| 0| 4| 5| 9|10|14|15|19|20|24|25|29|30|34|35|39|40|44|45|49|50| 54| 55| 59| 60| 64| 65| 69| 70| 74| 75| 79| 80| 84| 85| 89| 90| 94| 95| 99|100|104|105|109|
 *
 * The Payload in this case is the following:
 *
 * Payload |  0|  3| 4| 5| 9|10|13|14|15|19|20|24|25|29|30|34|35|39|40|44|45|46|49|50|54|55|59|60|61|62|64|65|69|70|74|75|77|78|79|80|84|85|89|90|93|94|95|99|100|104|105|109|
 * ------- |---|---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---|---|---|---|
 * Rel     | 0 | 3 | 4| 0| 4| 0| 3| 4| 0| 4| 0| 4| 0| 4| 0| 4| 0| 4| 0| 4| 0| 1| 4| 0| 4| 0| 4| 0| 1| 2| 4| 0| 4| 0| 4| 0| 2| 3| 4| 0| 4| 0| 4| 0| 3| 4| 0| 4|  0|  4|  0|  4|
 * Byte    | 2      ||| 3  || 4    ||| 5  || 6  || 7  || 8  || 9  ||10  ||11    ||| 12 || 13 || 14     |||| 15 || 16 || 17     |||| 18 || 19 || 20   ||| 21 || 22   || 23   ||
 * Descrip |ChipID|| 1| Address|||| Value 0      ||||||| Value 1      ||||||| Value 2        |||||||| Value 4        |||||||| Value 5        |||||||| Value 6          |||||||
 * Size    |  3   || 1| 9      |||| 16           ||||||| 16           ||||||| 16             |||||||| 16             |||||||| 16             |||||||| 16               |||||||
 *
 *
 *
 *
 * @brief RD53A Write Register Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class WrReg: public Command{
  
 public:
  
  /**
   * Create a WrReg
   **/
  WrReg();

  /**
   * Create a WrReg from another one
   * @param copy WrReg to copy
   **/
  WrReg(WrReg * copy);

  /**
   * Create a WrReg with parameters
   * @param chipid Chip ID [0:3] + bcast [4]
   * @param address the address [0:9]
   * @param value the value to write [0:15]
   **/
  WrReg(uint32_t chipid, uint32_t address, uint32_t value);

  /**
   * Empty destructor just for completeness.
   **/
  ~WrReg();
  
  /**
   * Clone this WrReg
   * @return A copy of this WrReg
   **/
  WrReg * Clone();

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

  /**
   * Set the value to write 
   * @param value the value to write [0:15]
   * @param index index to write [0:5]
   **/
  void SetValue(uint32_t value, uint32_t index=0);

  /**
   * Get the value to write 
   * @param index index to write [0:5]
   * @return the value to write [0:15]
   **/
  uint32_t GetValue(uint32_t index=0);

  /**
   * Set the mode (0: short, 1: long) 
   * @param mode the write mode (0: short, 1: long)
   **/
  void SetMode(uint32_t mode);

  /**
   * Get the mode (0: short, 1: long) 
   * @return the write mode (0: short, 1: long)
   **/
  uint32_t GetMode();

 private:

  uint32_t m_symbol;
  uint32_t m_chipid;
  uint32_t m_address;
  uint32_t m_mode;
  std::vector<uint32_t> m_value;
};

}

#endif
