#ifndef RD53A_REGISTER_H
#define RD53A_REGISTER_H

#include <cstdint>

namespace RD53A{

/**
 * The FEI4B Configuration is divided into Register objects.
 * A Register holds the configuration of a particular functionality of the FEI4B.
 * The value of a Register can be set (Register::SetValue), and retrieved (Register::GetValue).
 * And it also contains a flag (Register::IsUpdated) to know if the register was updated since last reading.
 * The maximum content of a Register is 16-bits.
 *
 * @brief RD53A Register
 * @author Carlos.Solans@cern.ch
 * @date August 2020
 **/

class Register{

  public:

  /**
   * Create a new Register
   **/
  Register();

  /**
   * Delete the Register
   **/
  ~Register();

  /**
   * Set the new value of the Register
   **/
  void SetValue(uint16_t value);

  /**
   * Get the Value of the Register
   * @return The value of the register
   **/
  uint16_t GetValue();

  /**
   * Mark the Register as being updated
   * @param enable True if the Register has been updated
   **/
  void Update(bool enable=true);

  /**
   * Get if the Register has been updated
   * @return True if it has been updated
   */
  bool IsUpdated();

  private:

  uint16_t m_value;
  bool m_updated;

};

}

#endif
