#ifndef RD53A_TOOLS_H
#define RD53A_TOOLS_H

#include <cstdint>

namespace RD53A{

  /**
   * @brief RD53A Tools
   * @author Carlos.Solans@cern.ch
   * @author Enrico.Junior.Schioppa@cern.ch
   * @date November 2020
   */

  class Tools{

  public:

    /**
     * Default constructor
     **/
    Tools();

    /**
     * Destructor
     **/
    ~Tools();

    /**
     * Convert injection charge from electrons to vcal DAC units.
     * @param charge The charge to convert (electrons).
     * @return The Vcal value in DAC units
     */
    static uint32_t injToVcal(double charge);

    /**
     * Convert injection charge from vcal DAC units to electrons.
     * @param vcal The Vcal value in DAC units
     * @return The charge in electrons.
     */
    static double injToCharge(double vcal);

    /**
     * Convert threshold charge from electrons to vcal DAC units.
     * @param charge The charge to convert (electrons).
     * @param ccol The core column to select between front-end flavor (syn, diff or lin)
     * @return The Vth value in DAC units
     */
    static uint32_t thrToVth(double charge, uint32_t ccol);

    /**
     * Convert threshold charge from vcal DAC units to electrons.
     * @param vth The Vth value in DAC units
     * @param ccol The core column to select between front-end flavor (syn, diff or lin)
     * @return The charge in electrons.
     */
    static double thrToCharge(double vth, uint32_t ccol);

    /**
     * Assigns the threshold calibration parameters according to the front-end flavor
     * @param par Pointer of doubles to store the parameters
     * @param nPar Number of parameters
     * @param ccol The core column to select between front-end flavor (syn, diff or lin)
     * @return The charge in electrons.
     */
    static void getThCalibrationParameters(double *par, unsigned int nPar, uint32_t ccol);

    /**
     * Convert injected charge to ToT value.
     * @param DAC The VFF_DIFF, KRUM_CURR_LIN or IBIAS_KRUM_SYNC parameters in DAC units, according to the front-end flavor
     * @param charge Difference between the injected charge and the threshold in electrons
     * @param ccol feFlavor sync, diff or lin
     * @return The ToT value in bc units. 
     */
    static uint32_t chargeToToT(double DAC, double charge, uint32_t ccol); 

    /**
     * Assigns the ToT calibration parameters according to the front-end flavor
     * @param par Pointer of doubles to store the parameters
     * @param nPar Number of parameters
     * @param ccol The core column to select between front-end flavor (syn, diff or lin)
     */
    static void getToTCalibrationParameters(double *par, unsigned int nPar, uint32_t ccol); 

  };

}

#endif
