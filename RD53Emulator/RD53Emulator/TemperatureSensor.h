#ifndef RD53A_TEMPERATURESENSOR_H
#define RD53A_TEMPERATURESENSOR_H

#include <cstdint>

namespace RD53A{

	/**
   * This class represents one of the 4 NTC type temperature sensors inside the RD53A.
   *
   * @brief RD53A TemperatureSensor 
   * @author Carlos.Solans@cern.ch
   * @author Kaan.Oyulmaz@cern.ch
   * @date March 2021
   **/
  class TemperatureSensor{

  public:
    /**
     * Default consturctor
     **/
    TemperatureSensor();
    
    /**
     * Destructor
     **/
    ~TemperatureSensor();

    /**
     * Set the value from the ADC value.
     * Requires conversion from ADC to a floating point number
     * @param value the temperature value in ADC counts
     **/
    void SetADC(uint32_t value);

    /**
     * Set the temperature value in degrees celsuis.
     * @param value the temperature value degrees celsius
     **/
    void SetValue(float value);

    /**
     * Get the temperature value as ADC counts
     * @return the temperature value in ADC counts
     **/
    uint32_t GetADC();

    /**
     * Get the temperature value in degrees celsuis.
     * @return the temperature value degrees celsius
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

    /**
     * Set the calibration constant of ADC.
     * @param cal variable is float
     **/
    void SetCalibration(float cal);

    /**
     * Get the calibration constant of ADC.
     * @return cal variable as float
     **/
    float GetCalibration();

    /**
     * Get the calculated temperature.
     * @return calculated temperature value degrees celsius
     **/
    float GetTemperature();

  private:

    uint32_t m_adc;
    uint32_t m_value;
    bool m_power;
    bool m_updated;
    float m_voltage;
    float m_calibration;
    float m_temperature;
    // constants that are used in the calculation of temperature
    float e_charge = 1.6021e-19;
    float k_boltzman = 1.3806e-23;
    float slope_adc = 0.2;
    float offset_adc = 15.5;
  };

}

#endif 
