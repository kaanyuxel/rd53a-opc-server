#ifndef RD53A_BLANK_H
#define RD53A_BLANK_H

#include "RD53Emulator/Frame.h"

namespace RD53A{


/**
 * RD53A basic data block (type-c) identified by 0x1E.
 *
 * @brief RD53A BlankFrame
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class BlankFrame: public Frame{
  
 public:

  BlankFrame();

  /**
   * Return a human readable representation of the data
   * @return The human readable representation of the data
   **/
  ~BlankFrame();

  /**
   * Return a human readable representation of the data
   * @return The human readable representation of the data
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

 protected:

  uint8_t m_afield;

};

}
#endif 
