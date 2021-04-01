#ifndef RD53A_ECR_H
#define RD53A_ECR_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * Flush the hit data path inside the chip, clearing all prior pending triggers and hits. 
 * Configuration and pending errors/warnings are unaffected. 
 * This command should be issued prior to sending triggers for the first time since power-up.
 * The symbol identifier of this command is 0x5A5A and doesn't contain any data.
 *
 * | Bit   |  0 | 15 |
 * | ---   | -- | -- |
 * | Desc  | Symbol ||
 * | Value | 0x5A5A ||
 *
 * @brief RD53A Event Counter Reset
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class ECR: public Command{
  
 public:
  
  /**
   * Create an RD53A ECR
   **/
  ECR();
  
  /**
   * Create an ECR as copy of another one
   * @param copy A ECR to be copied
   **/
  ECR(ECR * copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~ECR();

  /**
   * Return a clone of this ECR
   * @return A clone of this ECR
   **/
  ECR * Clone();

  /**
   * Return a human readable representation of the ECR
   * @return The human readable representation of the ECR
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
