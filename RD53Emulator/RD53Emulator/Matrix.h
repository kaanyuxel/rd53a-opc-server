#ifndef RD53A_MATRIX_H
#define RD53A_MATRIX_H

#include "RD53Emulator/Pixel.h"
#include <cstdlib>
#include <vector>

namespace RD53A{

/**
 * Matrix is a representation of the RD53A pixel matrix.
 * Each of the 192 x 400 pixels (RD53A::Pixel) in the matrix
 * can be addressed by their column and row (Matrix::GetPixel)
 * or alternatively by one of the following methods:
 *
 *  * 16-bit address: Matrix::GetQuad(uint32_t)
 *  * 8-bit double column and 9-bit row: Matrix::GetPair(uint32_t, uint32_t)
 *  * 6-bit core column, 6-bit core row, 4-bit core region, 1-bit pixel pair: Matrix::GetPair(uint32_t, uint32_t, uint32_t, uint32_t)
 *
 * The matrix is divided into core regions of 8x8 pixels identified by
 * a 6-bit core column, and a 6-bit core row.
 * There are 16 regions inside each core identified by a 4-bit core region.
 * An additional 1-bit region pair is required to identify the pair inside the region,
 * since the configuration of the pixels is written in pairs (two pixels at a time).
 * Each pair of pixels is identified by the following:
 *
 *  * 6-bit core column
 *  * 6-bit core row
 *  * 4-bit core region (region in core)
 *  * 1-bit region pair (pair in region)
 *
 * According to the version 3.51 of the RD53A manual,
 * the regions inside a 4-bit core region are numbered
 * from 0 to 15 from left to right (increasing column number)
 * and top to bottom (decreasing row number).
 * This needs to be checked.
 *
 *  |   |  0 |  1 |
 *  | - |  - |  - |
 *  | 7 |  0 |  1 |
 *  | 6 |  2 |  3 |
 *  | 5 |  4 |  5 |
 *  | 4 |  6 |  7 |
 *  | 3 |  8 |  9 |
 *  | 2 | 10 | 11 |
 *  | 1 | 12 | 13 |
 *  | 0 | 14 | 15 |
 *
 *
 * Each pixel pair can be addressed by its double column and row, that can be computed
 * from the core column, core row, and core region, and region pair like the following:
 *
 * Double Column:
 *
 * | Bit  | 7 | 2     | 1              | 0           |
 * | ---  | - | -     | -              | -           |
 * | Desc | core col || core region[0] | region pair |
 * | Size | 6        || 1              | 1           |
 *
 * Region Row
 *
 * | Bit  | 8 | 3     | 2              | 0 |
 * | ---  | - | -     | -              | - |
 * | Desc | core row || core region[3:1]  ||
 * | Size | 6        || 3                 ||
 *
 * @brief RD53A Matrix
 * @author Carlos.Solans@cern.ch
 * @date September 2020
 */
class Matrix{

public:

  /**
   * Build a new Matrix containing 192 x 400 Pixel objects
   */
  Matrix();

  /**
   * Delete the objects in the Matrix
   */
  ~Matrix();

  /**
   * Update the bits of 4 pixels in a core region given the 16-bit address
   * @param address The 16-bit address
   * @param value The 32-bit blob of the 4 pixels in the core region
   */
  void SetQuad(uint32_t address, uint32_t value);

  /**
   * Update the bits of a pair of pixels given their double column and row
   * @param double_col 8-bit double column address (core_col, core_region[0], region_pair)
   * @param row 9-bit row address (core_row, core_region[3:1])
   * @param value The 16-bit value
   */
  void SetPair(uint32_t double_col, uint32_t row, uint32_t value);

  /**
   * Update the bits of a pair of pixels given their core column, core row, core region, and region pair
   * @param core_col The 6-bit core column
   * @param core_row The 6-bit core row
   * @param core_region The 4-bit region in the core
   * @param region_pair The 1-bit pair in the region
   * @param value The 16-bit value
   */
  void SetPair(uint32_t core_col, uint32_t core_row, uint32_t core_region, uint32_t region_pair, uint32_t value);

  /**
   * Update the bits of 4 pixels in a core region given the 16-bit address
   * @param address The 16-bit address
   * @return The 32-bit blob of the 4 pixels addressed
   */
  uint32_t GetQuad(uint32_t address);

  /**
   * Get the bits of a pair of pixels given their double column and row
   * @param double_col 8-bit composite (core_col, core_region[0], region_pair)
   * @param row 9-bit composite (core_row, core_region[3:1])
   * @return value The 16-bit value
   */
  uint32_t GetPair(uint32_t double_col, uint32_t row);

  /**
   * Get the bits of a pair of pixels given their core column, core row, core region, and region pair
   * Update the pixel bits given the 14-bit address
   * @param core_col The 6-bit core column
   * @param core_row The 6-bit core row
   * @param core_region The 4-bit region in the core
   * @param region_pair The 1-bit pair in the region
   * @return The 16-bit value
   */
  uint32_t GetPair(uint32_t core_col, uint32_t core_row, uint32_t core_region, uint32_t region_pair);

  /**
   * Get the Pixel pointer given the pixel column and row
   * @param col The pixel column
   * @param row The pixel row
   * @return A pointer to the Pixel object
   */
  Pixel * GetPixel(uint32_t col, uint32_t row);

private:

  std::vector<std::vector<Pixel*> > m_pixels;

};

}

#endif
