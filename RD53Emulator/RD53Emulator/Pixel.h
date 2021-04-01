#ifndef RD53A_PIXEL_H
#define RD53A_PIXEL_H

#include <stdint.h>
#include <string>

namespace RD53A{

/**
 * A representation of the in Pixel in the RD53A.
 *
 * | Bit | Name     | Definition                            |
 * | --- | ----     | ----------                            |
 * |  0  | Enable   | Enable the output of the Pixel (mask) |
 * |  1  | Inject   | Use the small capacitor for injection |
 * |  2  | Hitbus   | Connect the Pixel to the hit bus OR   |
 * | 3:6 | TDAC     | Pixel threshold setting (LSB first)   |
 * |  7  | Gain     | Use the large capacitor for injection |
 *
 * @brief RD53A Pixel
 * @author Carlos.Solans@cern.ch
 * @date August 2020
 **/
class Pixel{

public:

  static const uint32_t Sync = 0;
  static const uint32_t Lin = 1;
  static const uint32_t Diff = 2;

  static const uint32_t Output = 0;
  static const uint32_t Enable = 0;
  static const uint32_t Inject = 1;
  static const uint32_t Hitbus = 2;
  static const uint32_t TDAC = 3;
  static const uint32_t TDAC_0 = 3;
  static const uint32_t TDAC_1 = 4;
  static const uint32_t TDAC_2 = 5;
  static const uint32_t TDAC_3 = 6;
  static const uint32_t Gain = 7;

  /**
   * Create a new Pixel with default empty configuration
   **/
  Pixel(uint8_t type=Pixel::Sync);

  /**
   * Empty destructor
   **/
  ~Pixel();

  /**
   * Set the Pixel type (Synchronous, Linear, or Differential)
   * @param value The Pixel type (Pixel::Sync, Pixel::Lin, Pixel::Diff)
   **/
  void SetType(uint8_t value);

  /**
   * Get the Pixel type (Synchronous, Linear, or Differential)
   * @return The Pixel type (Pixel::Sync, Pixel::Lin, Pixel::Diff)
   **/
  uint8_t GetType();

  /**
   * Set the 8 Pixel bits at the same time
   * @param value The Pixel bits
   **/
  void SetValue(uint32_t value);

  /**
   * Get the 8 Pixel bits at the same time
   * @return The 8 Pixel bits
   **/
  uint32_t GetValue();

  /**
   * Set a Pixel register (group of bits) given a name.
   * @param name The name of the register (Output, Hitbus, Inject, TDAC, Gain)
   * @param value The value of the register
   **/
  void SetValue(std::string name, uint32_t value);

  /**
   * Set a Pixel register (group of bits) given a name.
   * @param name The name of the register (Output, Hitbus, Inject, TDAC, Gain)
   * @return The value of the register
   **/
  uint32_t GetValue(std::string name);

  /**
   * Set the output bit (Pixel::Enable) for the given Pixel
   * @param enable Enable the Pixel output if true
   */
  void SetEnable(bool enable);

  /**
   * Get the output bit (Pixel::Enable) for the given Pixel
   * @return True if the Pixel output is enabled
   */
  bool GetEnable();

  /**
   * Set the inject bit (Pixel::Inject) for the given Pixel
   * @param enable Enable the Pixel injection if true
   */
  void SetInject(bool enable);

  /**
   * Get the inject bit (Pixel::Inject) for the given Pixel
   * @return True if the Pixel injection is enabled
   */
  bool GetInject();

  /**
   * Get the hitbus bit (Pixel::Hitbus) for the given Pixel
   * @param enable Enable the Pixel hitbus if true
   */
  void SetHitbus(bool enable);

  /**
   * Get the hitbus bit (Pixel::Hitbus) for the given Pixel
   * @return True if the Pixel hitbus is enabled
   */
  bool GetHitbus();

  /**
   * Set the in pixel threshold (Pixel::TDAC) for the given Pixel
   * @param tdac The pixel threshold
   */
  void SetTDAC(uint32_t tdac);

  /**
   * Get the in pixel threshold (Pixel::TDAC) for the given Pixel
   * @return The pixel threshold
   */
  uint32_t GetTDAC();

  /**
   * Set the pixel gain (Pixel::Gain) for the given Pixel
   * @param gain Pixel gain
   */
  void SetGain(bool gain);

  /**
   * Set the pixel gain (Pixel::Gain) for the given Pixel
   * @return The pixel gain
   */
  bool GetGain();

private:

  uint8_t m_type;
  uint8_t m_data;

};

}

#endif
