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

#include <stdexcept>

namespace mmtf {

/**
 * @brief Exception thrown when failing during decoding.
 */
class DecodeError: public std::runtime_error {
public:
    DecodeError(const std::string& m): std::runtime_error(m) { }
};

/**
 * @brief Exception thrown when failing during encoding.
 */
class EncodeError: public std::runtime_error {
public:
    EncodeError(const std::string& m): std::runtime_error(m) { }
};

} // mmtf namespace

#endif
