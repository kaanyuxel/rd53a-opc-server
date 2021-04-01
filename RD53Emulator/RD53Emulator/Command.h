#ifndef RD53A_COMMAND_H
#define RD53A_COMMAND_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace RD53A{

/**
 * The RD53A Command protocol is composed of a stream of 16-bit frames.
 * Every command (ECR, BCR, Pulse, Cal, WrReg, RdReg, Noop, Sync) except the Trigger
 * is identified by a 16-bit Symbol like the following:
 *
 * | Command | Symbol (bin)        | Hex    |
 * | ------- | ------------        | ---    |
 * | ECR     | 0101_1010_0101_1010 | 0x5A5A |
 * | BCR     | 0101_1001_0101_1001 | 0x5959 |
 * | Pulse   | 0101_1100_0101_1100 | 0x5C5C |
 * | Cal     | 0110_0011_0110_0011 | 0x6363 |
 * | WrReg   | 0110_0110_0110_0110 | 0x6666 |
 * | RdReg   | 0110_0101_0110_0101 | 0x6565 |
 * | Noop    | 0110_1001_0110_1001 | 0x6969 |
 * | Sync    | 1000_0001_0111_1110 | 0x417E |
 *
 * The Trigger Command is composed of a stream of 16 bits
 * with an 8-bit identifier, followed by an 8-bit Tag.
 * The possible Trigger values are the following:
 *
 * Symbol     | Encoding  | Hex  | Trigger
 * ---------- | --------- | ---- | -------
 * Trigger_01 | 0010_1011 | 0x2B | 000T
 * Trigger_02 | 0010_1101 | 0x2D | 00T0
 * Trigger_03 | 0010_1110 | 0x2E | 00TT
 * Trigger_04 | 0011_0011 | 0x33 | 0T00
 * Trigger_05 | 0011_0101 | 0x35 | 0T0T
 * Trigger_06 | 0011_0110 | 0x36 | 0TT0
 * Trigger_07 | 0011_1001 | 0x39 | 0TTT
 * Trigger_08 | 0011_1010 | 0x3A | T000
 * Trigger_09 | 0011_1100 | 0x3C | T00T
 * Trigger_10 | 0100_1011 | 0x4B | T0T0
 * Trigger_11 | 0100_1101 | 0x4D | T0TT
 * Trigger_12 | 0100_1110 | 0x4E | TT00
 * Trigger_13 | 0101_0011 | 0x53 | TT0T
 * Trigger_14 | 0101_0101 | 0x55 | TTT0
 * Trigger_15 | 0101_0110 | 0x56 | TTTT
 *
 * Some commands are followed by data fields encoded by a custom 5 to 8 bit protocol,
 * accessible in in the symbol to value (Command::m_symbol2data),
 * and value to symbol (Command::m_data2symbol) look up tables:
 * 
 * Symbol  | Encoding  | Hex
 * ------- | --------- | ----
 * Data_00 | 0110_1010 | 0x6A
 * Data_01 | 0110_1100 | 0x6C
 * Data_02 | 0111_0001 | 0x71
 * Data_03 | 0111_0010 | 0x72
 * Data_04 | 0111_0100 | 0x74
 * Data_05 | 1000_1011 | 0x8B
 * Data_06 | 1000_1101 | 0x8D
 * Data_07 | 1000_1110 | 0x8E
 * Data_08 | 1001_0011 | 0x93
 * Data_09 | 1001_0101 | 0x95
 * Data_10 | 1001_0110 | 0x96
 * Data_11 | 1001_1001 | 0x99
 * Data_12 | 1001_1010 | 0x9A 
 * Data_13 | 1001_1100 | 0x9C
 * Data_14 | 1010_0011 | 0xA3 
 * Data_15 | 1010_0101 | 0xA5
 * Data_16 | 1010_0110 | 0xA6
 * Data_17 | 1010_1001 | 0xA9
 * Data_18 | 1010_1010 | 0xAA
 * Data_19 | 1010_1100 | 0xAC
 * Data_20 | 1011_0001 | 0xB1
 * Data_21 | 1011_0010 | 0xB2
 * Data_22 | 1011_0100 | 0xB6
 * Data_23 | 1100_0011 | 0xC3
 * Data_24 | 1100_0101 | 0xC5
 * Data_25 | 1100_0110 | 0xC6
 * Data_26 | 1100_1001 | 0xC9
 * Data_27 | 1100_1010 | 0xCA
 * Data_28 | 1100_1100 | 0xCC
 * Data_29 | 1101_0001 | 0xD1
 * Data_30 | 1101_0010 | 0xD2
 * Data_31 | 1101_0100 | 0xD4
 *
 * This class defined the byte array to object method (Command::UnPack),
 * and the object to byte array (Command::Pack).
 * Command::ToString returns a human readable representation of the command.
 * And Command::GetType is used to return the command type.
 *
 * @brief Abstract RD53A Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Command{
  
 public:

  static const uint32_t UNKNOWN=0; /**< Default command type */
  static const uint32_t RDREG=1;   /**< Type for the RD53ARdReg command */
  static const uint32_t WRREG=2;   /**< Type for the RD53AWrReg command */
  static const uint32_t CAL=3;     /**< Type for the RD53ACal command */
  static const uint32_t PULSE=4;   /**< Type for the RD53APulse command */
  static const uint32_t TRIGGER=5; /**< Type for the RD53ATrigger command */
  static const uint32_t ECR=6;     /**< Type for the RD53AECR command */
  static const uint32_t BCR=7;     /**< Type for the RD53ABCR command */
  static const uint32_t NOOP=8;    /**< Type for the RD53ANoop command */
  static const uint32_t SYNC=9;    /**< Type for the RD53ASync command */

  /**
   * Empty constructor
   **/
  Command();

  /**
   * Empty destructor
   **/
  virtual ~Command(){};

  /**
   * Return a clone of the current Command
   * @return A clone of the current Command
   **/
  virtual Command * Clone()=0;

  /**
   * Return a human readable representation of the command
   * @return The human readable representation of the command
   **/
  virtual std::string ToString()=0;
  
  /**
   * Extract the contents of this command from the bytes
   * @param bytes the byte array
   * @param maxlen the maximum number of bytes than can be read
   * @return the number of bytes processed
   **/
  virtual uint32_t UnPack(uint8_t * bytes, uint32_t maxlen)=0;
  
  /**
   * Set the contents of this command to bytes
   * @param bytes the byte array
   * @return the number of bytes processed
   **/
  virtual uint32_t Pack(uint8_t * bytes)=0;

  /**
   * Get the type of the command
   * @return the command type
   **/
  virtual uint32_t GetType()=0;

 protected:

  /** Lookup table from 5-bit data (0-31) to custom 8-bit symbols */
  static std::vector<uint32_t> m_data2symbol;

  /** Lookup table from custom 8-bit symbols to 5-bit data (0-31) */
  static std::map<uint32_t,uint32_t> m_symbol2data;
  
};

}

#endif 
