#ifndef RD53A_HIT_H
#define RD53A_HIT_H

#include <cstdint>
#include <string>

namespace RD53A{

/**
 * A Hit is a class containing an event number, a Trigger ID (TID),
 * a Trigger Tag (TTag), a BCID, a pixel column, a pixel row and a TOT value.
 * During a scan, a Hit is created by the FrontEnd::HandleData method from the
 * information contained in the DataFrame.
 *
 * One DataFrame can contain information to create up to 8 Hit objects.
 * if the type is DataFrame::HIT_HIT, and all TOTs are active.
 *
 * The event number is not provided by the data format.
 *
 * @verbatim

 DataFrame data;

 Hit hit(data.GetTID(0),data.GetTTag(0),data.GetL1ID(0));
 hit.Set(data.GetCol(1),data.GetRow(1),data.GetTOT1(1));

   @endverbatim
 *
 * @brief RD53A Hit
 * @author Carlos.Solans@cern.ch
 * @date September 2020
 **/

class Hit{

public:

  /**
   * Create a new Hit
   */
  Hit();

  /**
   * Delete the Hit
   */
  ~Hit();

  /**
   * Create a Hit with L1ID, BCID
   * @param trigger_id The Trigger ID
   * @param trigger_tag The Trigger Tag
   * @param bcid The BCID counter
   */
  Hit(uint32_t trigger_id, uint32_t trigger_tag, uint32_t bcid);

  /**
   * Clone the hit
   * @return a Hit pointer cloned from this one
   */
  Hit * Clone();

  /**
   * Update the Hit with Trigger ID, Trigger Tag, and BCID
   * @param trigger_id The Trigger ID
   * @param trigger_tag The Trigger Tag
   * @param bcid The BCID counter
   */
  void Update(uint32_t trigger_id, uint32_t trigger_tag, uint32_t bcid);

  /**
   * Set the col, row and tot at once
   * @param col the column number
   * @param row the row number
   * @param tot the TOT
   */
  void Set(uint32_t col, uint32_t row, uint32_t tot);

  /**
   * Get the Trigger ID of this hit
   * @return The Trigger ID of this hit
   */
  uint32_t GetTID();

  /**
   * Get the Trigger Tag of this hit
   * @return The Trigger Tag of this hit
   */
  uint32_t GetTTag();

  /**
   * Get the BCID of this hit
   * @return The BCID of this hit
   */
  uint32_t GetBCID();

  /**
   * Get the column of this hit
   * @return The column of this hit
   */
  uint32_t GetCol();

  /**
   * Get the row of this hit
   * @return The row of this hit
   */
  uint32_t GetRow();

  /**
   * Get the time over threshold of this hit
   * @return The time over threshold of this hit
   */
  uint32_t GetTOT();

  /**
   * Get the event number counter
   * @return The event number counter
   */
  uint32_t GetEvNum();

  /**
   * Get the Trigger ID of this hit
   * @param tid The Trigger ID of this hit
   */
  void SetTID(uint32_t tid);

  /**
   * Get the Trigger Tag of this hit
   * @param tid The Trigger Tag of this hit
   */
  void SetTTag(uint32_t tid);

  /**
   * Set the BCID of this hit
   * @param bcid The BCID of this hit
   */
  void SetBCID(uint32_t bcid);

  /**
   * Set the column of this hit
   * @param col The column of this hit
   */
  void SetCol(uint32_t col);

  /**
   * Set the row of this hit
   * @param row The row of this hit
   */
  void SetRow(uint32_t row);

  /**
   * Set the time over threshold of this hit
   * @param tot The time over threshold of this hit
   */
  void SetTOT(uint32_t tot);

  /**
   * Set the event number counter
   * @param evnum The event number counter
   */
  void SetEvNum(uint32_t evnum);

  /**
   * Return the string representation of this hit
   * @return The human readable representation of this hit
   */
  std::string ToString();

private:

  uint32_t m_col;
  uint32_t m_row;
  uint32_t m_tot;
  uint32_t m_tid;
  uint32_t m_ttag;
  uint32_t m_bcid;
  uint32_t m_evnum;

};

}

#endif
