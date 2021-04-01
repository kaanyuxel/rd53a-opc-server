#ifndef RD53A_BCR_H
#define RD53A_BCR_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * Resets the internal BCID counter, which is one of the two ways of identifying the events.
 * Required to syncrhonize the BCID with the DAQ, and recommended at the start of a new run.
 * The symbol identifier of this command is 0x59 and doesn't contain any data.
 *
 * | Bit   |  0 | 15 |
 * | ----- | -- | -- |
 * | Desc  | Symbol ||
 * | Value | 0x5959 ||
 *
 * @brief RD53A Bunch Counter Reset
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class BCR: public Command{
  
 public:
  
  /**
   * Create an RD53A BCR
   **/
  BCR();
  
  /**
   * Create a BCR as copy from another one
   * @param copy The BCR to be copied
   */
  BCR (BCR * copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~BCR();
  
  /**
   * Return a clone of this BCR
   * @return A clone of this BCR
   **/
  BCR * Clone();

  /**
   * Return a human readable representation of the BCR
   * @return The human readable representation of the BCR
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
