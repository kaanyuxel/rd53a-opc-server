#ifndef RD53A_RUNNUMBER_H
#define RD53A_RUNNUMBER_H

#include <string>
#include <cstdlib>

namespace RD53A{

/**
 * The Run Number uses a file to store the current run number.
 * Once the Run Number has been used by a scan, the value in the file
 * should be updated.
 *
 * @brief A tool to handle run numbers
 * @author Carlos.Solans@cern.ch
 * @date November 2019
 */

class RunNumber{

 public:

  /**
   * Create a new RunNumber object
   */
  RunNumber();

  /**
   * Delete the RunNumber
   */
  ~RunNumber();

  /**
   * Get the next run number from the file system.
   * If the update flag is set, the value from the file system will be updated with the new value.
   * Else the value in memory will be returned.
   * @param update Force reading the value from the file system
   * @return The next run number to be used
   */
  uint32_t GetNextRunNumber(bool update=false);

  /**
   * Get the current run number.
   * If the update flag is set, the value will be read from the file system.
   * Else the in memory value will be returned.
   * @param update Force reading the value from the file system
   * @return The current run number to be used
   */
  uint32_t GetRunNumber(bool update=false);

  /**
   * Force the current run number
   */
  void SetRunNumber(uint32_t runnumber);

  /**
   * Get the next run number as a string. See RunCounter::GetNextRunNumber().
   * @param numdigits Number of characters in the string filled with zeroes
   * @param update Force reading the value from the file system
   * @return The next run number to be used
   */
  std::string GetNextRunNumberString(uint32_t numdigits, bool update=false);

  /**
   * Get the current run number as a string. See RunCounter::GetRunNumber().
   * @param numdigits Number of characters in the string filled with zeroes
   * @param update Force reading the value from the file system
   * @return The current run number to be used
   */
  std::string GetRunNumberString(uint32_t numdigits, bool update=false);

 private:

  uint32_t m_RunNumber;

};

}

#endif
