#ifndef RD53A_DECODER_H
#define RD53A_DECODER_H

#include "RD53Emulator/Frame.h"
#include "RD53Emulator/RegisterFrame.h"
#include "RD53Emulator/BlankFrame.h"
#include "RD53Emulator/DataFrame.h"

#include <cstdint>
#include <vector>

namespace RD53A{

/**
 * This class is designed to encode RD53A specific commands
 * into a byte stream, and decode a byte stream into commands,
 * through the Decoder::Encode and Decoder::Decode.
 *
 * RD53A specific frames (RegisterFrame, Blank, Data) can be
 * added to the byte stream by Decoder::AddFrame.
 * Commands added to the decoder will be owned by the decoder,
 * and should not be deleted by the user. 
 * The sequence of frames in the byte stream is preserved. 
 *
 * The byte stream is accessible through Decoder::GetBytes.
 * The resulting pointer cannot be deleted. 
 * Similarly, a byte stream can be decoded by the Decoder::SetBytes.
 * The frames are available from Decoder::GetFrames.
 *
 * @verbatim
 
   Decoder decoder;
   decoder.AddFrame(new Register());
   decoder.AddFrame(new Blank());
   decoder.AddFrame(new Data());
 
   //Encode into bytes
   decoder.Encode();
   uint8_t * bytes = decoder.GetBytes();
   uin32_t length = decoder.GetLength();

   //Decode into commands
   decoder.SetBytes(bytes, length);
   decoder.Decode();
   vector<Frames*> frames = decoder.GetFrames();

   @endverbatim
 *
 * @brief RD53A Frame encoder/decoder
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Decoder{
    
 public:

  /**
   * Initialize the decoder
   **/
  Decoder();
  
  /**
   * Delete the frames in memory
   **/
  ~Decoder();
  
  /**
   * Add bytes to the already existing byte array
   * @param bytes byte array
   * @param pos starting index of the byte array
   * @param len number of bytes to add
   **/
  void AddBytes(uint8_t *bytes, uint32_t pos, uint32_t len);

  /**
   * Replace the bytes of the byte array.
   * @param bytes byte array
   * @param len number of bytes to add
   **/
  void SetBytes(uint8_t *bytes, uint32_t len);
  
  /**
   * Add a cframe to the end of the frame list
   * @param frame RD53A specific frame
   **/
  void AddFrame(Frame *frame);
  
  /**
   * Clear the byte array
   **/
  void ClearBytes();
  
  /**
   * Clear the frame list by deleting each
   * object in the list.
   **/
  void ClearFrames();
  
  /**
   * Clear the byte array and the command list
   **/
  void Clear();
  
  /**
   * Get a string representation of the bytes. 
   * @return a string in hexadecimal
   **/
  std::string GetByteString();

  /**
   * Get the byte array pointer. 
   * This pointer cannot be deleted by the user.
   * @return the byte array as a pointer
   **/
  uint8_t * GetBytes();
  
  /**
   * @return the size of the byte array
   **/
  uint32_t GetLength();

  /**
   * Encode the frames into a byte array
   **/
  void Encode();

  /**
   * Decode the byte array into frames
   **/
  void Decode(const bool verbose = false);
  
  /**
   * Get the list of frames
   * @return vector of RD53AFrames pointers
   **/
  std::vector<Frame*> & GetFrames();
  
 private:
  
  std::vector<Frame*> m_frames;
  std::vector<uint8_t> m_bytes;
  //uint8_t * m_bytes;
  uint32_t m_length;
  
  DataFrame * m_fD;
  RegisterFrame * m_fR;
  BlankFrame * m_fB;
};

}

#endif
