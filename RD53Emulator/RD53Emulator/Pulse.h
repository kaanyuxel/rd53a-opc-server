#ifndef RD53A_PULSE_H
#define RD53A_PULSE_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * The global Pulse Command sends a single pulse to a given Chip ID with a given duration.
 * The chip ID is 3-bit plus 1 bit for broadcast.
 * The duration is calculated from the Pulse values are from 0 to 512, resulting in durations of 2^value
 * with a maximum duration of 512, in cycles of the 160 MHz clock.
 * The symbol identifier of this command is 0x5C5C, and is followed by another 16-bit frame.
 * Encoded with the custom 5 to 8 bit encoding. Meaning:
 * 
 * Bits    | Description
 * ------- | -----------
 * [9:6]   | Chip ID [3:0]
 * [5]     | Reserved (0)
 * [4:1]   | Length [3:0]
 * [0]     | Reserved (0)
 *
 *
 *
 * The encoded Command uses 32 bits, and it looks like the following:
 *
 * Bit     |  0 | 15 | 16 | 23 | 24 | 31 |
 * ----    | -- | -- | -- | -- | -- | -- |
 * Byte    |  0-1   ||  2     ||  3     ||
 * Payload | 0x6666 || 0  | 4  | 5  | 9  |
 *
 * The Payload is the following:
 *
 * Payload |  0 |  3 |  4 |  5 |  8  |  9 |
 * ------- | -- | -- | -- | -- | --  | -- |
 * Rel     |  0 |  3 |  4 |  0 |  3  |  4 |
 * Byte    |  2         |||  3          |||
 * Descrip | ChipID ||  0 |  Length ||  0 |
 * Size    |  3     ||  1 |  4      ||  1 |
 * 
 * Pulse duration
 *
 * Length | Cycles | Time [ns]
 * ------ | ------ | ---------
 *      0 |      1 |     6.25
 *      1 |      2 |    12.50
 *      2 |      4 |    25.00
 *      3 |      8 |    50.00
 *      4 |     16 |   100.00
 *      5 |     32 |   200.00
 *      6 |     64 |   400.00
 *      7 |    128 |   800.00
 *      8 |    256 |  1600.00
 *      9 |    512 |  3200.00
 *     .. |    512 |  3200.00
 *     15 |    512 |  3200.00
 *
 *
 * @brief RD53A Global Pulse Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Pulse: public Command{
  
 public:
  
  /**
   * Create a Pulse
   **/
  Pulse();
  
  /**
   * Create a Pulse with settings
   * @param chipid Chip ID [0:3] + bcast [4]
   * @param length Pulse duration in powers of 2 of the 160 MHz clock
   **/
  Pulse(uint32_t chipid, uint32_t length=4);

  /**
   * Create a Pulse from another one
   * @param copy A Pulse to copy
   */
  Pulse(Pulse * copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~Pulse();
  
  /**
   * Clone this Pulse
   * @return A clone of this Pulse
   */
  Pulse * Clone();

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
   * Set the chipid for the pulse
   * @return chipid Chip ID [0:3] + bcast [4]
   **/
  uint32_t GetChipId();

  /**
   * Set the length of the pulse
   * @param length The length of the pulse in powers of 2 of the 160 MHz clock
   **/
  void SetLength(uint32_t length);

  /**
   * Get the length of the pulse
   * @return The length of the pulse in powers of 2 of the 160 MHz clock
   **/
  uint32_t GetLength();

 private:

  uint32_t m_symbol;
  uint32_t m_chipid;
  uint32_t m_length;

};

}

#endif
