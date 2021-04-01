#ifndef RD53A_NOOP_H
#define RD53A_NOOP_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * No Operation Command for the RD53A. Is used as a filled between commands in a command sequence.
 * The symbol identifier of this command is 0x6969 and doesn't contain any data.
 *
 * | Bit   |  0 | 15 |
 * | ---   | -- | -- |
 * | Desc  | Symbol ||
 * | Value | 0x6969 ||
 *
 * @brief RD53A No-Operation Command
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Noop: public Command{
  
 public:
  
  /**
   * Create a Noop
   **/
  Noop();
  
  /**
   * Create a Noop as a copy from another one
   * @param copy A Noop to copy
   */
  Noop(Noop* copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~Noop();
  
  /**
   * Clone this Noop
   * @return A Noop copy of this one
   */
  Noop * Clone();

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

 private:

  uint32_t m_symbol;

};

}

#endif
