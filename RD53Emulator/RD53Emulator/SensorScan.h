#ifndef RD53A_SENSORSCAN_H
#define RD53A_SENSORSCAN_H

#include "RD53Emulator/Handler.h"

namespace RD53A{

/**
 * Readout the temperature and radiation sensors in a loop
 *
 * @brief RD53A SensorScan
 * @author Carlos.Solans@cern.ch
 * @author Kaan.Yuksel.Oyulmaz@cern.ch
 * @date March 2021
 */

class SensorScan: public Handler{

public:

  /**
   * Create the scan
   */
  SensorScan();

  /**
   * Delete the Handler
   */
  virtual ~SensorScan();

  /**
   * Run over the temperature and radiation sensors and save histos
   */
  virtual void Run();

  /**
   * Loop over the temperature and radiation sensors
   */
  virtual void Loop();

  /**
   * Enable the verbose mode
   * @param enable Enable verbose mode if true
   **/
  virtual void SetVerbose(bool enable);

private:

  bool m_continue;
  bool m_verbose;

};

}

#endif
