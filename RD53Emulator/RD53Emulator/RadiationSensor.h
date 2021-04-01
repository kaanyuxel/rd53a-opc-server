#ifndef RD53A_RADIATIONSENSOR_H
#define RD53A_RADIATIONSENSOR_H

#include <cstdint>

namespace RD53A{

	/**
   * This class represents one of the 4 BJT type radiation sensors inside the RD53A.
   *
   * @brief RD53A RadiationSensor 
   * @author Carlos.Solans@cern.ch
   * @author Kaan.Oyulmaz@cern.ch
   * @date March 2021
   **/
  class RadiationSensor{

  public:
    /**
     * Default consturctor
     **/
    RadiationSensor();
    
    /**
     * Destructor
     **/
    ~RadiationSensor();

    /**
     * Set the value from the ADC value.
     * Requires conversion from ADC to a floating point number
     * @param value the radiation value in ADC counts
     **/
    void SetADC(uint32_t value);

    /**
     * Set the radiation value in rads
     * @param value the radiation value in rads
     **/
    void SetValue(float value);

    /**
     * Get the radiation value as ADC counts
     * @return the radiation value in ADC counts
     **/
    uint32_t GetADC();

    /**
     * Get the radiation value in rads.
     * @return the radiation value in rads
     **/
    float GetValue();

    /**
     * Set the temperature sensor power.
     * @param power as on/off
     **/
    void SetPower(bool power);

    /**
     * Get power status of temperature sensor.
     * @return power as on/off
     **/
    bool GetPower();

    /**
     * Set the status of temperature sensor.
     * @param status as boolean
     **/
    void Update(bool status);

    /**
     * Get the status of temperature sensor.
     * @return status as boolean
     **/
    bool isUpdated();

  private:

    uint32_t m_adc;
    uint32_t m_value;
    bool m_power;
    bool m_updated;
    
  };

}

#endif 
