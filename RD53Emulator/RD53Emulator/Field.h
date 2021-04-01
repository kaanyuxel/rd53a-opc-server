#ifndef RD53A_FIELD_H
#define RD53A_FIELD_H

#include <cstdint>
#include <vector>

#include "RD53Emulator/Register.h"

namespace RD53A{

/**
 * A Field contains the relevant Configuration bits for a particular setting.
 *
 * @brief RD53A Configuration Field
 * @author Carlos.Solans@cern.ch
 * @date May 2020
 **/

class Field{
	
public:
    
  /**
   * Construct a Field given a pointer to the Configuration register.
   * @param data pointer to the Configuration register
   * @param start First bit of the data
   * @param len Length of the Field in bits
   * @param defval Default value for the Field
   * @param reversed Flip the bit order of the Field if true
   **/
  Field(Register * data, uint32_t start, uint32_t len, uint32_t defval=0, bool reversed=false);

  /**
   * Empty Destructor
   **/
  ~Field();

  /**
   * Set the value of the Field. Extra bits will be truncated.
   * @param value New value of the Field
   **/
  void SetValue(uint32_t value);

  /**
   * Get the value of the Field
   * @return The value of the Field
   **/
  uint32_t GetValue();
  
private:

  Register * m_data;
  uint32_t m_start;
  uint32_t m_len;
  uint32_t m_defval;
  uint32_t m_mask;
  uint32_t m_smask;
  bool m_reversed;

  static std::vector<uint32_t> m_masks;
    
  uint32_t Reverse(uint32_t value, uint32_t sz);

};

}

#endif
