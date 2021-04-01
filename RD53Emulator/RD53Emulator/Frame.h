#ifndef RD53A_FRAME_H
#define RD53A_FRAME_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace RD53A{

/**
 * This class represents a generic RD53A output Frame.
 * The specific RD53A frames (RegisterFrame, DataFrame, BlankFrame)
 * extend this class.
 *
 * This class defines the methods to decode a byte array (Frame::UnPack)
 * and to encode it (Frame::Pack).
 * Frame::ToString returns a human readable string of the Frame
 *
 * @brief Abstract RD53A Frame
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Frame{
  
 public:

  static const uint32_t UNKNOWN=0;  /**< Unknown type of Frame **/
  static const uint32_t DATA=1;     /**< Type of the DataFrame **/
  static const uint32_t REGISTER=2; /**< Type of the RegisterFrame **/
  static const uint32_t BLANK=3;    /**< Type of the BlankFrame **/

  /**
   * Empty constructor
   **/
  Frame();

  /**
   * Virtual destructor
   **/
  virtual ~Frame();

  /**
   * Return a human readable representation of the frame
   * @return The human readable representation of the frame
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
   * Get the aurora header
   * @return the aurora header in byte form
   **/
  uint8_t GetAuroraHeader();

  /**
   * Get the type of the command
   * @return the command type
   **/
  virtual uint32_t GetType()=0;

 protected:

  uint32_t m_aheader;
  
};

}

#endif 
