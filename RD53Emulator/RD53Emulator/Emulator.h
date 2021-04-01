#ifndef RD53A_EMULATOR_H
#define RD53A_EMULATOR_H

#include "RD53Emulator/Decoder.h"
#include "RD53Emulator/Encoder.h"
#include "RD53Emulator/Configuration.h"
#include "RD53Emulator/Matrix.h"
#include "RD53Emulator/Command.h"
#include <queue>
#include <vector>
#include <mutex>
#include <random>

namespace RD53A{

/**
 * This class emulates an RD53A pixel detector that is identified by a chip ID
 * by makes use of a Command Encoder, a Frame Decoder, and a Field Configuration.
 *
 * A byte stream can be passed to the Emulator (Emulator::HandleCommand),
 * that will be decoded into a list of commands, and added to a queue.
 * If the chip ID of the Command does not match the one for the Emulator,
 * the Command will be ignored. Messages with chip ID >7 will be always interpreted.
 *
 * Commands are processed (Emulator::ProcessQueue) from the queue starting
 * from the oldest one received. After this the output data will be available
 * (Emulator::GetBytes, Emulator::GetLength) as a byte stream.
 * Since the RD53A has no internal self-trigger mechanism, no output data
 * will be available if no Trigger Command is received.
 *
 * By default the emulator output will contain both the service and the data frames.
 * It is possible to configure it to output only one of them in order to reproduce
 * the expected behaviour of the FELIX e-links.
 *
 *
 * @verbatim

   uint8_t * in_bytes =...
   uint32_t in_length =...

   Emulator emu(0);

   emu.HandleCommand(in_bytes,in_length);
   emu.ProcessQueue();

   uint8_t * out_bytes = emu.GetBytes();
   uin32_t out_length = emu.GetLength();

   @endverbatim
 *
 * The RD53A has a buffer for 32 triggers, identified by the Tag and the corresponding BCID.
 * A Trigger command with more than one BCID, will occupy more than one position in the buffer.
 * Notably the command with a trigger in the second position will correspond to BCID +1.
 * The 5-bit Tag provided by the trigger command is prepended to 2 bit trigger position.
 * Thus the output data Tag is 7-bit wide.
 *
 * The Trigger commands should be synchronous with the DAQ, and all the other commands should
 * fill the spaces between the Trigger commands.
 * ECR and BCR are always processed.
 * The rest of the commands are only processed if the chip ID in the command matches that of
 * the chip wire-bonds. Up to 8 chips can be addressed with the 3-bit chip id.
 * Not processed commands are skipped.
 *
 * The Cal Command generates a pulse synchronized with the 160 MHz clock,
 * of a given width, at a given delay after reception, and an auxiliary edge
 * at a different delay,
 * that is distributed to the matrix after predefined delay (Configuration::FineDelay).
 *
 * Sync pattern should be sent at power up, and every 32 frames.
 * A sequence of commands should start with a Sync frame.
 * The number of Sync symbols required to lock the transmission are 2N_lock.
 * If the number of Sync symbols drops below N_unlock, the transmission will be unlocked.
 *
 * The Command Encoder does not decode any commands until the lock has been reached.
 * All Commands are decoded upon reception, and only those targeted to the chip will produce an output.
 * Error handling is described below:
 *
 * Frame received   | Frame Expected | Error/Action
 * --------------   | -------------- | ------------
 * invalid, data    | data           | Aborted command
 * data, invalid    | data           | Aborted command
 * invalid, invalid | data           | Aborted command
 * invalid, data    | not data       | Lost trigger
 * invalid, invalid | not data       | Corrupted frame
 * invalid, sync    | any            | Corrupted sync
 * sync, invalid    | any            | Corrupted sync
 * invalid, command | any            | Execute with warning
 * command, invalid | any            | Execute with warning
 * trigger, invalid | any            | Lost Tag
 * command, command | data           | Ignored command
 *
 * Chip Configuration is addressed by WrReg commands.
 * Register 0 (Configuration::PIX_PORTAL) is a virtual register to read and write
 * Pixel data. Each Pixel holds 8 or fewer bits of local configuration, and is addressed
 * as half of the 16-bit register value.
 * Multiple Pixels can be addressed at a time by the broadcast (Configuration::B_MASK),
 * and the mode (Configuration::PIX_MODE), column (Configuration::REGION_COL)
 * and row (Configuration::REGION_ROW).
 *
 * The pixel data is written into or retrieved form global register 0 with normal write and read register commands.
 * This is a virtual register acting as a portal to whatever region(s) is(are) pointed to be column and row config registers.
 *
 * @brief RD53A Emulator
 * @author Carlos.Solans@cern.ch
 * @author Enrico.Junior.Schioppa@cern.ch
 * @author Alessandra.Palazzo@cern.ch
 * @date April 2020
 **/

class Emulator {

 public:

  static const uint32_t OUTPUT_ALL=0; /**< Output data and service frames. Default. */
  static const uint32_t OUTPUT_DATA=1; /**< Output data frames only */
  static const uint32_t OUTPUT_SERVICE=2; /**< Output service frames only */

  /**
   * Create a new Emulator containing a Command Encoder, and a Data Decoder.
   */
  Emulator(uint32_t chipid=0,uint32_t mode=0);

  /**
   * Delete the Emulator
   */
  ~Emulator();

  /**
   * Get the ID for this emulated front-end.
   * @return The ID for this emulated front-end from 0 to 7.
   **/
  uint32_t GetChipID();

  /**
   * Set the ID for this emulated front-end from 0 to 7.
   * @return The ID for this emulated front-end from 0 to 7.
   **/
  void SetChipID(uint32_t chipid);

  /**
   * Process byte stream of commands in the emulator.
   * The command will be first decoded by the Encoder,
   * and then interpreted by the emulator.
   * Commands are then placed in a command queue.
   * If the chip ID of the commands does not match the one
   * for this emulator, the command will be ignored.
   * Messages with ChipID>7 will be always interpreted.
   * @param recv_data Byte stream of commands.
   * @param recv_size Size of the byte stream.
   **/
  void HandleCommand(uint8_t *recv_data, uint32_t recv_size);

  /**
   * Process commands pending queue starting from the oldest received.
   * If the emulator is configured in auto-trigger mode through a configuration command.
   * It will result in the generation of data events.
   **/
  void ProcessQueue();
  
  /**
   * Enable the verbose mode
   * @param lvl Enable verbose mode if true
   **/
  void SetVerbose(int lvl);
  
  /**
   * Enable initialization with random thresholds
   * @param enable Enable random thresholds mode if true
   **/
  void SetRandomThresholds(bool enable);
  
  /**
   * Enable emulation of pixel electronic noise
   * @param enable Enable pixel noise if true
   **/
  void SetPixelNoise(bool enable);
  
  /**
   * Clear the emulator input and output queues.
   **/
  void Clear();

  /**
   * Initializes the thresholds to the values given as global configurations. If threshold randomization is enabled, it is applied here.
   **/
  void InitThresholds();
  
  /**
   * Creates a random ADC data between 0 and 5000
   * @return the ADC data
   **/
  uint32_t CreateRandomADCData();

  /**
   * Get the output byte stream from the emulator.
   * @return Byte stream output from the emulator.
   **/
  uint8_t * GetBytes();
  
  /**
   * Get the length of the output byte stream from the emulator.
   * @return Length of the byte stream output from the emulator.
   **/
  uint32_t  GetLength();
  
  /**
   * Set the output mode of the Emulator options are
   *  * OUTPUT_ALL: Data and service frames
   *  * OUTPUT_DATA: Only data frames
   *  * OUTPUT_SERVICE: Only service frames
   * @param mode The output mode (OUTPUT_ALL, OUTPUT_DATA, OUTPUT_SERVICE)
   */
  void SetOutputMode(uint32_t mode);

  private:
  
  int m_verbose;  
  bool m_randomThresholds;  
  bool m_pixelNoise;
  std::queue<uint32_t> m_read_reqs;
  std::deque<Command*> m_cmds;
  uint32_t m_register_index;
  Decoder *m_decoder;
  Encoder *m_encoder;
  std::mutex m_read_mutex;
  std::mutex m_reg_mutex;
  Configuration * m_config;
  Matrix * m_matrix;
  uint32_t m_outmode;
  uint32_t m_chipid;
  double ** m_thresholds;
  bool m_isInitialized;
  uint32_t m_ndf;
  uint32_t m_th_syn;
  uint32_t m_th_lin;
  uint32_t m_th_diff;
  double m_sigmaNoiseDistribution;
  uint32_t GetNextRegister();
  void AddServiceFrame();
  std::default_random_engine m_generator;
  std::normal_distribution<double> m_pixelNoiseDistribution;
};

}

#endif
