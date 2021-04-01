#ifndef RD53A_ENCODER_H
#define RD53A_ENCODER_H

#include "RD53Emulator/Command.h"
#include "RD53Emulator/ECR.h"
#include "RD53Emulator/BCR.h"
#include "RD53Emulator/Cal.h"
#include "RD53Emulator/Noop.h"
#include "RD53Emulator/Sync.h"
#include "RD53Emulator/Pulse.h"
#include "RD53Emulator/WrReg.h"
#include "RD53Emulator/RdReg.h"
#include "RD53Emulator/Trigger.h"

#include <cstdint>
#include <vector>

namespace RD53A{

/**
 * This class is designed to encode RD53A specific commands
 * into a byte stream, and decode a byte stream into commands,
 * through the Encoder::Encode and Encoder::Decode.
 *
 * RD53A specific commands (ECR, BCR, Pulse, Cal...) can be
 * added to the byte stream by Encoder::AddCommand.
 * Commands added to the encoder will be owned by the encoder,
 * and should be deleted by the user. 
 * The sequence of commands in the byte stream is preserved. 
 *
 * The byte stream is accessible through Encoder::GetBytes.
 * The resulting pointer cannot be deleted. 
 * Similarly, a byte stream can be decoded by the Encoder::SetBytes.
 * The commands are available from Encoder::GetCommands.
 *
 * Sync commands should be sent whenever there is no other command to send
 * and recommended every 32 frames. By default the transmission should always
 * begin with a Sync frame.
 *
 * The decoded commands are executed 1 BC after the end of the last frame of the command data.
 * Trigger, Cal, and Pulse commands have a delay and duration.
 * The trigger command sends 1 to 4 pulses in 4 consecutive BCs, and thus is finished before
 * a new command can be completely received (since 1 frame is 4 beam clock cycles).
 * The Cal and Global pulse commands can occupy their respective output lines
 * (CAL_edge, CAL_aux, and Global_pulse) for many clock cycles.
 * A new trigger or global pulse command should not be sent before such command is complete.
 *
 *
 * @verbatim
 
 Encoder encoder;
 encoder.AddCommand(new ECR());
 encoder.AddCommand(new Pulse());
 encoder.AddCommand(new Trigger());
 
 //Encode into bytes
 encoder.Encode();
 uint8_t * bytes = encoder.GetBytes();
 uin32_t length = encoder.GetLength();

 //Decode into commands
 encoder.SetBytes(bytes, length);
 encoder.Decode();
 vector<Command*> cmds = encoder.GetCommands();

   @endverbatim
 *
 * @brief RD53A Command encoder/decoder
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Encoder{
    
 public:

  /**
   * Initialize the RD53A data symbol dictionaries
   **/
  Encoder();
  
  /**
   * Delete the commands in memory
   **/
  ~Encoder();
  
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
   * Add a command to the end of the command list
   * @param cmd RD53A specific command
   **/
  void AddCommand(Command *cmd);
  
  /**
   * Clear the byte array
   **/
  void ClearBytes();
  
  /**
   * Clear the command list by deleting each
   * object in the list.
   **/
  void ClearCommands();
  
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
   * @return the byte array
   **/
  uint8_t * GetBytes();
  
  /**
   * @return the size of the byte array
   **/
  uint32_t GetLength();

  /**
   * Encode the commands into a byte array
   **/
  void Encode();

  /**
   * Decode the byte array into commands
   **/
  void Decode();
  
  /**
   * Get the list of commands
   * @return vector of RD53ACommand pointers
   **/
  std::vector<Command*> & GetCommands();
  
 private:
  
  std::vector<Command*> m_cmds;
  std::vector<uint8_t> m_bytes;
  uint32_t m_length;
  
  ECR *m_ecr;
  BCR *m_bcr;
  Cal *m_cal;
  Noop *m_noop;
  Sync *m_sync;
  Pulse *m_pulse;
  WrReg *m_wrreg;
  RdReg *m_rdreg;
  Trigger *m_trig;
};

}
#endif
