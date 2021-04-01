#ifndef RD53A_DATA_H
#define RD53A_DATA_H

#include "RD53Emulator/Frame.h"

namespace RD53A{

/**
 * This class represents an RD53A data frame, which can contain 
 * one (Type 1) or two (Type 2) Hit or Header sub-frames.
 * These types match the aurora code header, 0b01 for Type 1,
 * and 0b10 for Type 2.
 * The data frame format for each type is the following:
 *
 * Type 1 (0b01)
 *
 * | Bit   | 63 | 56 | 55 | 48 | 47 | 40 | 39 | 32 | 31 | 24 | 23 | 16 | 15 | 8  | 7  | 0  |
 * | ----  | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -  | -  | -  |
 * | Rel   | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  |
 * | Value | Hit or Header (position 2)     ||||||||  Hit or Header (position 1)    ||||||||
 * | Byte  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
 *
 * Type 2 (0b10)
 *
 * | Bit   | 63 | 56 | 55 | 48 | 47 | 40 | 39 | 32 | 31 | 24 | 23 | 16 | 15 | 8  | 7  | 0  |
 * | ----  | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -  | -  | -  |
 * | Rel   | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  | 7  | 0  |
 * | Value | 0x1E   || 0x04   || 0x0000         |||| Hit or Header (position 1)     ||||||||
 * | Byte  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
 *
 * Unfortunately, in software it is impossible to unequivocally
 * decode a Type 1 with a Hit in position 2, as it will also
 * coincide with possible encodings of an Register.
 * Thus, this implementation will not implement the encoding
 * and decoding of a Type 1 with a hit in position 2.
 *
 * The header format is the following
 *
 * | Bit   | 31 | 26 | 24 | 23 | 20 | 19 | 16 | 15 | 14 | 8 | 7 | 0 |
 * | ----  | -- | -- | -- | -- | -- | -- | -- | -- | -- | - | - | - |
 * | Rel   | 7  | 1  | 0  | 7  | 4  | 3  | 0  | 7  | 6  | 0 | 7 | 0 |
 * | Value | 1       | Trig ID    ||| Trig Tag   ||| BCID        ||||
 * | Size  | 7      || 5          ||| 5          ||| 15          ||||
 * | Byte  | 1          |||  2             |||| 3         ||| 4    ||
 *
 * The hit format is the following
 *
 * | Bit   | 31 | 26   | 25 | 24 | 23 | 20 | 19 | 16      | 15 | 12 | 11 | 8 | 7 | 4 | 3 | 0 |
 * | ----  | -- | --   | -- | -- | -- | -- | -- | --      | -- | -- | -- | - | - | - | - | - |
 * | Rel   | 7  | 2    | 1  | 0  | 7  | 4  |  3 |  0      | 7  | 4  | 3  | 0 | 7 | 4 | 3 | 0 |
 * | Value | Core Col || Core row       |||| Core region || Tot1   || Tot2  || Tot3 || Tot4 ||
 * | Size  | 6        || 6              |||| 4           || 4      || 4     || 4    || 4    ||
 * | Byte  | 1                ||||  2                  |||| 3             |||| 4          ||||
 *
 *
 * If the byte-stream encoded by the Decoder will not use aurora codes,
 * it will be impossible to distinguish a Hit in bits 63-32 of a Type 1 
 * data frame, because of the matching valid addresses in a Register.
 * 
 *
 * @brief RD53A DataFrame
 * @author Carlos.Solans@cern.ch
 * @author Kaan.Yuksel.Oyulmaz@cern.ch
 * @author Enrico.Junior.Schioppa@cern.ch
 * @date March 2020
 **/

class DataFrame: public Frame{
  
 public:
  
  /**
   * Format values
   **/
  static const uint32_t UNKNOWN=0; /**< Format unknown **/
  static const uint32_t HIT_HIT=1; /**< Hit followed by a hit **/
  static const uint32_t HIT_HDR=2; /**< Hit followed by a header **/
  static const uint32_t HDR_HIT=3; /**< Header followed by a hit **/
  static const uint32_t HDR_HDR=4; /**< Header followed by a header **/
  static const uint32_t SYN_HDR=5; /**< Sync followed by a header **/
  static const uint32_t SYN_HIT=6; /**< Sync followed by a hit **/
  
  /**
   * Create an empty data frame
   **/
  DataFrame();

  /**
   * Delete the data frame
   **/
  ~DataFrame();

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
   * Get the type of command
   * @return the type of the command
   **/
  uint32_t GetType();

  /**
   * Get the format of the Frame. Possible values are:
   *   - HIT_HIT: Hit followed by a hit
   *   - HIT_HDR: Hit followed by a header
   *   - HDR_HIT: Header followed by a hit
   *   - HDR_HDR: Header followed by a header
   *   - SYN_HDR: Sync followed by a header
   *   - SYN_HIT: Sync followed by a hit
   *
   * @return the format of the Frame from the list of possible values
   **/
  uint32_t GetFormat();

  /**
   * Set the format of the Frame. Possible values are:
   *   - HIT_HIT: Hit followed by a hit
   *   - HIT_HDR: Hit followed by a header
   *   - HDR_HIT: Header followed by a hit
   *   - HDR_HDR: Header followed by a header
   *   - SYN_HDR: Sync followed by a header
   *   - SYN_HIT: Sync followed by a hit
   *
   * @param format The format of the Frame from the list of possible values
   **/
  void SetFormat(uint32_t format);

  /**
   * Add a hit to the Frame in a given position.
   * @param pos position of the hit in the Frame (0 or 1)
   * @param core_col core column
   * @param core_row core row
   * @param core_region region in core
   * @param tot1 the tot value for pixel 1
   * @param tot2 the tot value for pixel 2
   * @param tot3 the tot value for pixel 3
   * @param tot4 the tot value for pixel 4
   **/
  void SetHit(uint32_t pos, uint32_t core_col, uint32_t core_row, uint32_t core_region, uint32_t tot1, uint32_t tot2, uint32_t tot3, uint32_t tot4);

  /**
   * Add a hit to the Frame in a given position.
   * @param pos position of the hit in the Frame (0 or 1)
   * @param quad_col quad column (0 to 99)
   * @param row row (0 to 191)
   * @param tot array of tot values for the 4 pixels
   **/
  void SetHit(uint32_t pos, uint32_t quad_col, uint32_t row, uint32_t * tot);

  /**
   * Create an event header at a given position in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param triggerID the Trigger ID
   * @param triggerTag the Trigger Tag
   * @param BCID the Bunch Crossing ID
   **/
  void SetHeader(uint32_t pos, uint32_t triggerID, uint32_t triggerTag, uint32_t BCID);

  /**
   * Set the Trigger ID of a header in the Frame
   * @param pos position of the header in the Frame (0 or 1)
   * @param triggerID the Trigger ID
   **/
  void SetTID(uint32_t pos, uint32_t triggerID);
  
  /**
   * Set the Trigger Tag of a header in the Frame
   * @param pos position of the header in the Frame (0 or 1)
   * @param triggerTag the Trigger Tag
   **/
  void SetTTag(uint32_t pos, uint32_t triggerTag);
  
  /**
   * Set the BCID tag in the header
   * @param pos position of the header in the Frame (0 or 1)
   * @param BCID the BCID
   **/
  void SetBCID(uint32_t pos, uint32_t BCID);

  /**
   * Set the core column of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param core_col the core column (0 to 49)
   **/
  void SetCoreCol (uint32_t pos, uint32_t core_col);
  
  /**
   * Set the core row of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param core_row the core column (0 to 23)
   **/
  void SetCoreRow (uint32_t pos, uint32_t core_row);
  
  /**
   * Set the core region of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param core_reg the core row (0 to 15)
   **/
  void SetCoreReg (uint32_t pos, uint32_t core_reg);

  /**
   * Set the TOT 1 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param tot the 4-bit TOT value (0 to 15)
   **/
  void SetTOT1 (uint32_t pos, uint32_t tot);

  /**
   * Set the TOT 2 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param tot the 4-bit TOT value (0 to 15)
   **/
  void SetTOT2 (uint32_t pos, uint32_t tot);

  /**
   * Set the TOT 3 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param tot the 4-bit TOT value (0 to 15)
   **/
  void SetTOT3 (uint32_t pos, uint32_t tot);

  /**
   * Set the TOT 4 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param tot the 4-bit TOT value (0 to 15)
   **/
  void SetTOT4 (uint32_t pos, uint32_t tot);

  /**
   * Set the TOT with given idx (0,1,2,3) of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param idx TOT index (0,1,2,3)
   * @param tot the 4-bit TOT value (0 to 15)
   **/
  void SetTOT(uint32_t pos, uint32_t idx, uint32_t tot);

  /**
   * Get the Trigger ID of a header in the Frame
   * @param pos position of the header in the Frame (0 or 1)
   * @return the Trigger ID
   **/
  uint32_t GetTID(uint32_t pos);
  
  /**
   * Get the Trigger Tag of a header in the Frame
   * @param pos position of the header in the Frame (0 or 1)
   * @return the Trigger Tag
   **/
  uint32_t GetTTag(uint32_t pos);
  
  /**
   * Get the BCID tag in the header
   * @param pos position of the header in the Frame (0 or 1)
   * @return the BCID
   **/
  uint32_t GetBCID(uint32_t pos);

  /**
   * Get the core column of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the core column (0 to 49)
   **/
  uint32_t GetCoreCol(uint32_t pos);

  /**
   * Get the core row of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the core row (0 to 23)
   **/
  uint32_t GetCoreRow(uint32_t pos);

  /**
   * Get the core region of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the core column (0 to 15)
   **/
  uint32_t GetCoreReg(uint32_t pos);

  /**
   * Get the TOT 1 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the 4-bit TOT value (0 to 15)
   **/
  uint32_t GetTOT1(uint32_t pos);
  
  /**
   * Get the TOT 2 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the 4-bit TOT value (0 to 15)
   **/
  uint32_t GetTOT2(uint32_t pos);
  
  /**
   * Get the TOT 3 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the 4-bit TOT value (0 to 15)
   **/
  uint32_t GetTOT3(uint32_t pos);
  
  /**
   * Get the TOT 4 of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the 4-bit TOT value (0 to 15)
   **/
  uint32_t GetTOT4(uint32_t pos);
  
  /**
   * Get the TOT with given idx (0,1,2,3) of a hit in the Frame
   * @param pos position of the hit in the Frame (0 or 1)
   * @param idx TOT index (0,1,2,3)
   * @return the 4-bit TOT value (0 to 15)
   **/
  uint32_t GetTOT(uint32_t pos, uint32_t idx);

  /**
   * Get the column of the first TOT of a hit in the Frame
   * by merging the core column, and core region values
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the core column (0 to 399)
   **/
  uint32_t GetCol(uint32_t pos);

  /**
   * Get the row of a hit in the Frame
   * by merging the core row, and core region values
   * @param pos position of the hit in the Frame (0 or 1)
   * @return the row (0 to 191)
   **/
  uint32_t GetRow(uint32_t pos);

 private:
 
  uint32_t m_aheader;

  uint32_t m_format;
  
  //Hit variables
  uint32_t m_ccol[2];
  uint32_t m_crow[2];
  uint32_t m_creg[2];
  uint32_t m_tot1[2];
  uint32_t m_tot2[2];
  uint32_t m_tot3[2];
  uint32_t m_tot4[2];

  //Header variables
  uint32_t m_TID[2];
  uint32_t m_TTag[2];
  uint32_t m_BCID[2];

};

}

#endif 
