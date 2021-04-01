#ifndef RD53A_SYNC_H
#define RD53A_SYNC_H

#include "RD53Emulator/Command.h"

namespace RD53A{


/**
 * Filler for the space between other commands.
 * Unlike other commands, the Sync does not have a symbol identifier that is repeated twice.
 * In this case the 16-bit symbol identifier is 0x417E and does not contain any data.
 *
 * | Bit   |  0 | 15 |
 * | ---   | -- | -- |
 * | Desc  | Symbol ||
 * | Value | 0x417E ||
 *
 * @brief RD53A Sync Command symbol
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Sync: public Command{
  
 public:
  
  /**
   * Create a Sync
   **/
  Sync();

  /**
   * Create a Sync as copy from another one
   * @param copy The Sync to be copied
   */
  Sync (Sync * copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~Sync();

  /**
   * Return a clone of this Sync
   * @return A clone of this Sync
   **/
  Sync * Clone();
  
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
