// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code are: Gerardo Tauriello
// 
// Based on mmtf_c developed by Julien Ferte (http://www.julienferte.com/),
// Anthony Bradley, Thomas Holder with contributions from Yana Valasatava,
// Gazal Kalyan, Alexander Rose.
//
// *************************************************************************

#ifndef MMTF_ENCODER_H
#define MMTF_ENCODER_H

#include "structure_data.hpp"
#include <string>

namespace mmtf {

/**
 * @brief Encode an MMTF data structure into a file.
 * @param[in] data                        MMTF data structure to be stored
 * @param[in] filename                    Path to file to load
 * @param[in] coord_divider               Divisor for coordinates
 * @param[in] occupancy_b_factor_divider  Divisor for occupancy and b-factor
 * Common settings for the divisors are the default values for a loss-less
 * encoding and both set to 10 for a lossy variant.
 * @throw mmtf::EncodeError if an error occured
 */
inline void encodeToFile(const StructureData& data,
                         const std::string& filename,
                         int coord_divider = 1000,
                         int occupancy_b_factor_divider = 100);

// *************************************************************************
// IMPLEMENTATION
// *************************************************************************

// TODO

} // mmtf namespace

#endif
