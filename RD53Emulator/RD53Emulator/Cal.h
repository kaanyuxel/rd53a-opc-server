#ifndef RD53A_Cal_H
#define RD53A_Cal_H

#include "RD53Emulator/Command.h"

namespace RD53A{

/**
 * Send a calibration command to a given chip ID.
 * The symbol identifier is 0x69.
 * This command controls the generation of 2 signals
 * CAL_EDGE (delay: 3 bit, mode: 1 bit, width: 6 bit),
 * and CAL_AUX (delay: 5 bit, mode: 1 bit).
 *
 * Command encoding uses 20 bits:
 * 
 * Bits    | Description
 * ------- | -------
 * [19:16] | ChipId
 * [15]    | CAL_EDGE mode (1 bit) (pulse=1, step=0)
 * [14:12] | CAL_EDGE delay (3 bit) in units of bunch crossing (25 ns)
 * [11:6]  | CAL_EDGE width (6 bit) in units of 160 MHz clock (6.25 ns)
 * [5]     | CAL_AUX mode (1 bit)
 * [4:0]   | CAL_AUX delay (5 bit) in units of 160 MHz clock (6.25 ns)
 *
 * Split in 4 5-bit fields it looks like the following:
 *
 *
 * | Bit   | 19 | 16 | 15 | 14 | 12 | 11 | 10 | 9  | 6  | 5  | 4  | 0  |
 * | ----  | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
 * | Rel   | 4  | 1  | 0  | 4  | 2  | 1  | 0  | 4  | 1  | 0  | 4  | 7  |
 * | Value | ChipID || EM | EDelay || Edge width     |||| AM | ADelay ||
 * | Field | 1          ||| 2          ||| 3          ||| 4          |||
 *
 * The CAL_EDGE mode (Cal::GetEdgeMode, Cal::SetEdgeMode) selects between step (mode=0) or pulse (mode=1)
 * for the generation of a rising edge after a CAL_EDGE delay (Cal::GetEdgeDelay, Cal::SetEdgeDelay),
 * for a given duration (width) (Cal::GetEdgeWidth, Cal::SetEdgeWidth),
 * unless the duration is zero, in which case no action is taken.
 * It the CAL_edge was high upon reception, command will be ignored.
 * A falling edge is generated after delay plus duration only in pulse mode.
 *
 * The CAL_AUX signal is switched to the values specified by CAL_AUX mode (Cal::GetAuxMode, Cal::SetAuxMode)
 * after the specified CAL_AUX delay (Cal::GetAuxDelay, Cal::SetAuxDelay).
 *
 * @brief RD53A Calibration Injection
 * @author Carlos.Solans@cern.ch
 * @date March 2020
 **/

class Cal: public Command{
  
 public:
  
  /**
   * Create a Cal
   **/
  Cal();
  
  /**
   * Create a Cal with settings
   * @param chipid Chip ID [0:3] + bcast [4]
   * @param edge_mode the mode for the CAL_EDGE
   * @param edge_delay the delay for the CAL_EDGE
   * @param edge_width the width for the CAL_EDGE
   * @param aux_mode the mode for the CAL_AUX
   * @param aux_delay the delay for the CAL_AUX
   **/
  Cal(uint32_t chipid, uint32_t edge_mode, uint32_t edge_delay, uint32_t edge_width, uint32_t aux_mode, uint32_t aux_delay);

  /**
   * Create a new Cal from another one
   * @param copy A Cal to be copied
   **/
  Cal(Cal * copy);

  /**
   * Empty destructor just for completeness.
   **/
  ~Cal();
  
  /**
   * Return a clone of this Cal
   * @return A clone of this Cal
   **/
  Cal * Clone();

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

  /**
   * Set the chipid for the pulse
   * @param chipid Chip ID [0:3] + bcast [4]
   **/
  void SetChipId(uint32_t chipid);

  /**
   * Get the chipid for the pulse
   * @return chipid Chip ID [0:3] + bcast [4]
   **/
  uint32_t GetChipId();

  /**
   * Set the CAL_EDGE mode (1 bit)
   * @param edge_mode the mode for the CAL_EDGE
   **/
  void SetEdgeMode(uint32_t edge_mode);

  /**
   * Get the CAL_EDGE mode (1 bit)
   * @return the mode for the CAL_EDGE
   **/
  uint32_t GetEdgeMode();
  
  /**
   * Set the CAL_EDGE delay (3 bits)
   * @param edge_delay the delay for the CAL_EDGE
   **/
  void SetEdgeDelay(uint32_t edge_delay);
  
  /**
   * Get the CAL_EDGE delay (3 bits)
   * @return the delay for the CAL_EDGE
   **/
  uint32_t GetEdgeDelay();
  
  /**
   * Set the CAL_EDGE width (6 bits)
   * @param edge_width the width for the CAL_EDGE
   **/
  void SetEdgeWidth(uint32_t edge_width);
  
  /**
   * Get the CAL_EDGE width (6 bits)
   * @return the width for the CAL_EDGE
   **/
  uint32_t GetEdgeWidth();
  
  /**
   * Set the CAL_AUX mode (1 bit)
   * @param aux_mode the mode for the CAL_AUX
   **/
  void SetAuxMode(uint32_t aux_mode);
  
  /**
   * Get the CAL_AUX mode (1 bit)
   * @return the mode for the CAL_AUX
   **/
  uint32_t GetAuxMode();
  
  /**
   * Set the CAL_AUX delay (5 bits)
   * @param aux_delay the delay for the CAL_AUX
   **/
  void SetAuxDelay(uint32_t aux_delay);
  
  /**
   * Get the CAL_AUX delay (5 bits)
   * @return the delay for the CAL_AUX
   **/
  uint32_t GetAuxDelay();

 private:

  uint32_t m_symbol;
  uint32_t m_chipid;
  uint32_t m_edge_mode;
  uint32_t m_edge_width;
  uint32_t m_edge_delay;
  uint32_t m_aux_mode;
  uint32_t m_aux_delay;
  
};

}

#endif
