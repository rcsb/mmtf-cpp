// *************************************************************************
// Copyright [2017] [RCSB]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// The authors of this code are: Gabriel Studer, Gerardo Tauriello
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
