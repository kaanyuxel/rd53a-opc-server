#ifndef RD53A_TRIGGER_H
#define RD53A_TRIGGER_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * A Trigger Command is one 16-bit frame that spans 4 BCs, thus
 * it must specify which of the 4 BCs should be triggered. 
 * This is encoded in a 8-bit symbol, and the other 8 bits are 
 * used to specify the Trigger Tag for those triggers.
 * The tag will be returned with the data corresponding to that trigger.
 * The Command looks like the following:
 *
 * | Bit   |  0 |  7 |  8 | 15 |
 * | ---   | -- | -- | -- | -- |
 * | Desc  | Symbol || Tag    ||
 *
 * The Trigger symbols are the following:
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
 * The Trigger Tag is encoded with the custom 5 to 8 bit protocol.
 * There are 31 possible values for the Trigger Tag.
 * The lookup tables are defined in
 * Command::m_symbol2data, and Command::m_data2symbol.
 *
 * @brief RD53A Trigger Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Trigger: public Command{

 public:

  /**
   * Encoding of the trigger patterns in 2-bytes
   **/
  enum{
    Trigger_01=0x2B, /**< Trigger 000T */
    Trigger_02=0x2D, /**< Trigger 00T0 */
    Trigger_03=0x2E, /**< Trigger 00TT */
    Trigger_04=0x33, /**< Trigger 0T00 */
    Trigger_05=0x35, /**< Trigger 0T0T */
    Trigger_06=0x36, /**< Trigger 0TT0 */
    Trigger_07=0x39, /**< Trigger 0TTT */
    Trigger_08=0x3A, /**< Trigger T000 */
    Trigger_09=0x3C, /**< Trigger T00T */
    Trigger_10=0x4B, /**< Trigger T0T0 */
    Trigger_11=0x4D, /**< Trigger T0TT */
    Trigger_12=0x4E, /**< Trigger TT00 */
    Trigger_13=0x53, /**< Trigger TT0T */
    Trigger_14=0x55, /**< Trigger TTT0 */
    Trigger_15=0x56  /**< Trigger TTTT */
  };

  /**
   * Create an empty Trigger.
   * Define which BC to trigger with 
   * Trigger::SetTrigger or Trigger::SetPattern
   **/
  Trigger();

  /**
   * Create a Trigger as copy from another one
   * @param copy The Trigger to be copied
   */
  Trigger (Trigger * copy);

  /**
   * Create a Trigger with a given pattern and tag
   * Possible pattern values are:
   *  - Trigger::Trigger_01
   *  - Trigger::Trigger_02
   *  - Trigger::Trigger_03
   *  - Trigger::Trigger_04
   *  - Trigger::Trigger_05
   *  - Trigger::Trigger_06
   *  - Trigger::Trigger_07
   *  - Trigger::Trigger_08
   *  - Trigger::Trigger_09
   *  - Trigger::Trigger_10
   *  - Trigger::Trigger_11
   *  - Trigger::Trigger_12
   *  - Trigger::Trigger_13
   *  - Trigger::Trigger_14
   *  - Trigger::Trigger_15
   * @param pattern The Trigger pattern (1 of 15 combinations)
   * @param tag The 5-bit Trigger tag
   */
  Trigger(uint32_t pattern, uint32_t tag);

  /**
   * Create a Trigger as copy from another one
   * @param bc1 Enable the Trigger in the first bunch crossing
   * @param bc2 Enable the Trigger in the second bunch crossing
   * @param bc3 Enable the Trigger in the third bunch crossing
   * @param bc4 Enable the Trigger in the fourth bunch crossing
   * @param tag The 5-bit Trigger tag
   */
  Trigger(bool bc1, bool bc2, bool bc3, bool bc4, uint32_t tag);

  /**
   * Delete the Trigger.
   * Clear internal memories.
   **/
  ~Trigger();

  /**
   * Return a clone of this Trigger
   * @return A clone of this Trigger
   **/
  Trigger * Clone();

  /**
   * Return a human readable representation of the trigger
   * @return The human readable representation of the trigger
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
   * Since the trigger command is 4 BC long, 
   * there are 4 possible BCs in which to trigger. 
   * This method sets which of the 4 BCs should be triggered
   * given a pattern: 
   *  - Trigger::Trigger_01 000T
   *  - Trigger::Trigger_02 00T0
   *  - Trigger::Trigger_03 00TT
   *  - Trigger::Trigger_04 0T00
   *  - Trigger::Trigger_05 0T0T
   *  - Trigger::Trigger_06 0TT0
   *  - Trigger::Trigger_07 0TTT
   *  - Trigger::Trigger_08 T000
   *  - Trigger::Trigger_09 T00T
   *  - Trigger::Trigger_10 T0T0
   *  - Trigger::Trigger_11 T0TT
   *  - Trigger::Trigger_12 TT00
   *  - Trigger::Trigger_13 TT0T
   *  - Trigger::Trigger_14 TTT0
   *  - Trigger::Trigger_15 TTTT
   * @param pattern one of the 15 different patterns or zero
   **/
  void SetPattern(uint32_t pattern);

  /**
   * Since the trigger command is 4 BC long, 
   * there are 4 possible BCs in which to trigger. 
   * This method returns which of the 4 BCs should be triggered
   * following the encoding: 
   *  - Trigger::Trigger_01 000T
   *  - Trigger::Trigger_02 00T0
   *  - Trigger::Trigger_03 00TT
   *  - Trigger::Trigger_04 0T00
   *  - Trigger::Trigger_05 0T0T
   *  - Trigger::Trigger_06 0TT0
   *  - Trigger::Trigger_07 0TTT
   *  - Trigger::Trigger_08 T000
   *  - Trigger::Trigger_09 T00T
   *  - Trigger::Trigger_10 T0T0
   *  - Trigger::Trigger_11 T0TT
   *  - Trigger::Trigger_12 TT00
   *  - Trigger::Trigger_13 TT0T
   *  - Trigger::Trigger_14 TTT0
   *  - Trigger::Trigger_15 TTTT
   * @return one of the 15 different patterns or zero
   **/
  uint32_t GetPattern();
  
  /**
   * Set the tag for this trigger
   * @param tag the trigger tag from 0 to 31 
   **/
  void SetTag(uint32_t tag);
 
  /**
   * Get the tag for this trigger
   * @return the trigger tag from 0 to 31 
   **/
  uint32_t GetTag();
  
  /**
   * Since the trigger command is 4 BC long, 
   * there are 4 possible BCs in which to trigger. 
   * This method sets one of the 4 BCs to be triggered
   * given the bc index and the enable.
   * The selected BC with bc will be triggered if enable is true. 
   * @param bc BC 
   * @param enable boolean value
   **/
  void SetTrigger(uint32_t bc, bool enable);
  
  /**
   * Since the trigger command is 4 BC long, 
   * there are 4 possible BCs in which to trigger. 
   * This method returns if one given bc index will be triggered.
   * @param bc BC 
   * @return true if bc will be triggered
   **/
  bool GetTrigger(uint32_t bc);

 private:
  
  std::vector<bool> m_bcs;
  uint32_t m_pattern;
  uint32_t m_tag;

  void pack();
  void unpack();

};

}

#endif
