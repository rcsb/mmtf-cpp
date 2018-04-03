// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The author of this code is: Daniel Farrell
// 
// Based on mmtf_python, adapted to c++ standards 2018
//
// *************************************************************************


#ifndef MMTF_ENCODER_H
#define MMTF_ENCODER_H

#include "structure_data.hpp"
#include "errors.hpp"
#include "object_encoders.hpp"
#include <string>
#include <math.h>

namespace mmtf {

// byteorder functions
#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif


/**
 * @brief Encode an MMTF data structure into a stream.
 * @param[in] data          MMTF data structure to be stored
 * @param[in] stream        Stream to encode to
 * @param[in] coord_divider               Divisor for coordinates
 * @param[in] occupancy_b_factor_divider  Divisor for occupancy and b-factor
 * @param[in] chain_name_max_length       Max. length for chain name strings
 * @throw mmtf::EncodeError if an error occured
 * Common settings for the divisors are the default values for a loss-less
 * encoding and both set to 10 for a lossy variant.
 */
template <typename Stream>
inline void encodeToStream(const StructureData& data, Stream& stream,
    int32_t coord_divider = 1000, int32_t occupancy_b_factor_divider = 100,
    int32_t chain_name_max_length = 4);

/**
 * @brief Encode an MMTF data structure into a file.
 * @param[in] data          MMTF data structure to be stored
 * @param[in] filename      Path to file to load
 * @param[in] coord_divider               Divisor for coordinates
 * @param[in] occupancy_b_factor_divider  Divisor for occupancy and b-factor
 * @param[in] chain_name_max_length       Max. length for chain name strings
 * @throw mmtf::EncodeError if an error occured
 * Common settings for the divisors are the default values for a loss-less
 * encoding and both set to 10 for a lossy variant.
 */
inline void encodeToFile(const StructureData& struct_data,
    const std::string& filename, int32_t coord_divider = 1000,
    int32_t occupancy_b_factor_divider = 100, int32_t chain_name_max_length  = 4);

/**
 * @brief Convert floats to ints via multiplier.
 * @param[in] vec_in        vector of floats
 * @param[in] multiplier    multiply float vector by this
 * @return vec_out          vec_in * multiplier and rounded
 */
inline std::vector<int32_t> convertFloatsToInts(std::vector<float> const & vec_in,
                                            int multiplier);

/**
 * @brief mmtf delta encode a vector of ints.
 * @param[in] vec_in        vector of ints
 * @return vec_out          delta encoded int vector
 */
inline std::vector<int32_t> deltaEncode(std::vector<int32_t> const & vec_in);


/**
 * @brief mmtf run length encode a vector of ints.
 * @param[in] vec_in        vector of ints
 * @return vec_out          run length encoded int vector
 */
inline std::vector<int32_t> runLengthEncode(std::vector<int32_t> const & vec_in );

/**
 * @brief mmtf recursive index encode a vector of ints.
 * @param[in] vec_in        vector of ints
 * @param[in] max           maximum value of signed 16bit int
 * @param[in] min           minimum value of signed 16bit int
 * @return vec_out          recursive index encoded vector
 */
inline std::vector<int32_t> recursiveIndexEncode(std::vector<int32_t> const & vec_in,
                                             int max=32767, int min=-32768);

/**
 * @brief mmtf convert char to int
 * @param[in] vec_in        vector of chars
 * @return vec_out          vector of ints
 */
inline std::vector<int32_t> convertCharsToInts(std::vector<char> const & vec_in);


/**
 * @brief Add mmtf header to a stream
 * @param[in] ss            stringstream to add a header to
 * @param[in] array_size    size of array you're adding
 * @param[in] codec         the codec type number you're using to encode
 * @param[in] param         the param for the codec you're using (default 0)
 */
inline void add_header(std::stringstream & ss, uint32_t array_size, uint32_t codec, uint32_t param=0);

/**
 * @brief Convert stringstream to CharVector
 * @param[in] ss            ss to convert
 * @return                  converted ss
 */
inline std::vector<char> stringstreamToCharVector(std::stringstream & ss);

/** encode 8 bit int to bytes encoding (type 2)
 * @param[in] vec_in        vector of ints to encode
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeInt8ToByte(std::vector<int8_t> vec_in);

/** encode 4 bytes to int encoding (type 4)
 * @param[in] vec_in        vector of ints to encode
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeFourByteInt(std::vector<int32_t> vec_in);

/** encode string vector encoding (type 5)
 * @param[in] in_sv         vector of strings
 * @param[in] CHAIN_LEN     maximum length of string
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeStringVector(std::vector<std::string> in_sv, int32_t CHAIN_LEN);


/** encode Run Length Char encoding (type 6)
 * @param[in] in_cv         vector for chars
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeRunLengthChar(std::vector<char> in_cv);


/** encode Run Length Delta Int encoding (type 8)
 * @param[in] int_vec       vector of ints
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeRunLengthDeltaInt(std::vector<int32_t> int_vec);

/** encode Run Length Float encoding (type 9)
 * @param[in] floats_in     vector of ints
 * @param[in] multiplier    float multiplier
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeRunLengthFloat(std::vector<float> floats_in, int32_t multiplier);

/** encode Delta Recursive Float encoding (type 10)
 * @param[in] floats_in     vector of ints
 * @param[in] multiplier    float multiplier
 * @return cv               char vector of encoded bytes
 */
inline std::vector<char> encodeDeltaRecursiveFloat(std::vector<float> floats_in, int32_t multiplier);


// *************************************************************************
// IMPLEMENTATION
// *************************************************************************

inline std::vector<int32_t> convertFloatsToInts(std::vector<float> const & vec_in,
                                            int multiplier) {
  std::vector<int32_t> vec_out;
  for (size_t i=0; i<vec_in.size(); ++i) {
    vec_out.push_back(round(vec_in[i]*multiplier));
  }
  return vec_out;
}


inline std::vector<int32_t> deltaEncode(std::vector<int32_t> const & vec_in) {
  std::vector<int32_t> vec_out;
  if (vec_in.size() == 0) return vec_out;
  vec_out.push_back(vec_in[0]);
  for (int32_t i=1; i< (int)vec_in.size(); ++i) {
    vec_out.push_back(vec_in[i]-vec_in[i-1]); 
  }
  return vec_out;
}


inline std::vector<int32_t> runLengthEncode(std::vector<int32_t> const & vec_in ) {
  std::vector<int32_t> ret;
  if (vec_in.size()==0) return ret;
  int32_t curr = vec_in[0];
  ret.push_back(curr);
  int32_t counter = 1;
  for (int32_t i = 1; i < (int)vec_in.size(); ++i) {
    if ( vec_in[i] == curr ) {
      ++counter;
    } else {
      ret.push_back(counter);
      ret.push_back(vec_in[i]);
      curr = vec_in[i];
      counter = 1;
    }
  }
  ret.push_back(counter);
  return ret;
}


inline std::vector<int32_t> recursiveIndexEncode(
    std::vector<int32_t> const & vec_in,
    int max /* =32767 */, int min /*=-32768 */) {
  std::vector<int32_t> vec_out;
  for (int32_t i=0; i< (int)vec_in.size(); ++i) {
    int32_t x = vec_in[i];
    if ( x >= 0 ) {
      while (x >= max) {
        vec_out.push_back(max);
        x -= max;
      }
    } else {
      while (x <= min) {
        vec_out.push_back(min);
        x += std::abs(min);
      }
    }
    vec_out.push_back(x);
  }
  return vec_out;
}


inline std::vector<int32_t> convertCharsToInts(std::vector<char> const & vec_in) {
  std::vector<int32_t> vec_out;
  for (size_t i=0; i<vec_in.size(); ++i) {
    vec_out.push_back((int)vec_in[i]);
  }
  return vec_out;
}

inline void add_header(std::stringstream & ss, uint32_t array_size, uint32_t codec, uint32_t param /* =0 */) { 
    uint32_t be_codec = htobe32(codec);
    uint32_t be_array_size = htobe32(array_size);
    uint32_t be_param = htobe32(param);
    ss.write(reinterpret_cast< char * >(&be_codec), sizeof(be_codec));
    ss.write(reinterpret_cast< char * >(&be_array_size), sizeof(be_array_size));
    ss.write(reinterpret_cast< char * >(&be_param), sizeof(be_param));
}


inline std::vector<char> stringstreamToCharVector(std::stringstream & ss) {
  std::string s = ss.str();
  std::vector<char> ret(s.begin(), s.end());
  return ret;
}


inline std::vector<char> encodeInt8ToByte(std::vector<int8_t> vec_in) {
  std::stringstream ss;
  add_header(ss, vec_in.size(), 2, 0);
  for (size_t i=0; i<vec_in.size(); ++i) {
    ss.write(reinterpret_cast< char * >(&vec_in[i]), sizeof(vec_in[i]));
  }
  return stringstreamToCharVector(ss);
}


inline std::vector<char> encodeFourByteInt(std::vector<int32_t> vec_in) {
  std::stringstream ss;
  add_header(ss, vec_in.size(), 4, 0);
  for (size_t i=0; i<vec_in.size(); ++i) {
    int32_t be_x = htobe32(vec_in[i]);
    ss.write(reinterpret_cast< char * >(&be_x), sizeof(be_x));
  }
  return stringstreamToCharVector(ss);
}


inline std::vector<char> encodeStringVector(std::vector<std::string> in_sv, int32_t CHAIN_LEN) {
  char NULL_BYTE = 0x00;
  std::stringstream ss;
  add_header(ss, in_sv.size(), 5, CHAIN_LEN);
  std::vector<char> char_vec;
  for (size_t i=0; i<in_sv.size(); ++i) {
    char_vec.insert(char_vec.end(), in_sv[i].begin(), in_sv[i].end());
    for (size_t j=0; j<CHAIN_LEN-in_sv[i].size(); ++j) {
      char_vec.push_back(NULL_BYTE);
    }
  }
  for (size_t i=0; i<char_vec.size(); ++i) {
    ss.write(reinterpret_cast< char * >(&char_vec[i]), sizeof(char_vec[i]));
  }
  return stringstreamToCharVector(ss);
}


inline std::vector<char> encodeRunLengthChar(std::vector<char> in_cv) {
  std::stringstream ss;
  add_header(ss, in_cv.size(), 6, 0);
  std::vector<int32_t> int_vec;
  int_vec = convertCharsToInts(in_cv);
  int_vec = runLengthEncode(int_vec);
  for (size_t i=0; i<int_vec.size(); ++i) {
    int32_t temp = htobe32(int_vec[i]);
    ss.write(reinterpret_cast< char * >(&temp), sizeof(temp));
  }
  return stringstreamToCharVector(ss);
}


inline std::vector<char> encodeRunLengthDeltaInt(std::vector<int32_t> int_vec) {
  std::stringstream ss;
  add_header(ss, int_vec.size(), 8, 0);
  int_vec = deltaEncode(int_vec);
  int_vec = runLengthEncode(int_vec);
  for (size_t i=0; i<int_vec.size(); ++i) {
    int32_t temp = htobe32(int_vec[i]);
    ss.write(reinterpret_cast< char * >(&temp), sizeof(temp));
  }
  return stringstreamToCharVector(ss);
}

inline std::vector<char> encodeRunLengthFloat(std::vector<float> floats_in, int32_t multiplier) {
  std::stringstream ss;
  add_header(ss, floats_in.size(), 9, multiplier);
  std::vector<int32_t> int_vec = convertFloatsToInts(floats_in, multiplier);
  int_vec = runLengthEncode(int_vec);
  for (size_t i=0; i<int_vec.size(); ++i) {
    int32_t temp = htobe32(int_vec[i]);
    ss.write(reinterpret_cast< char * >(&temp), sizeof(temp));
  } 
  return stringstreamToCharVector(ss);
}

  

inline std::vector<char> encodeDeltaRecursiveFloat(std::vector<float> floats_in, int32_t multiplier) {
  std::stringstream ss;
  add_header(ss, floats_in.size(), 10, multiplier);
  std::vector<int32_t> int_vec = convertFloatsToInts(floats_in, multiplier);
  int_vec = deltaEncode(int_vec);
  int_vec = recursiveIndexEncode(int_vec);
  for (size_t i=0; i<int_vec.size(); ++i) {
    int16_t temp = htobe16(int_vec[i]);
    ss.write(reinterpret_cast< char * >(&temp), sizeof(temp));
  }
  return stringstreamToCharVector(ss);
}


inline void encodeToFile(const StructureData& struct_data,
    const std::string& filename, int32_t coord_divider,
    int32_t occupancy_b_factor_divider, int32_t chain_name_max_length) {
    // encode to a file
    std::ofstream ofs(filename.c_str(), std::ios::binary | std::ios::out );
    if ( !ofs ) {
        throw EncodeError("Could not open >" + filename + "< for writing, exiting.");
    }
    encodeToStream(struct_data, ofs);
}


template <typename Stream>
inline void encodeToStream(const StructureData& data, Stream& stream,
    int32_t coord_divider, int32_t occupancy_b_factor_divider,
    int32_t chain_name_max_length) {
  if (!data.hasConsistentData(4,true)) {
    throw mmtf::EncodeError("mmtf EncoderError, StructureData does not have Consistent data... exiting!");
  }


  std::map<std::string, msgpack::object> data_map;
  msgpack::zone m_zone;
  // std::string
  data_map["mmtfVersion"] = msgpack::object(data.mmtfVersion, m_zone);
  data_map["mmtfProducer"] = msgpack::object(data.mmtfProducer, m_zone);
  if (!mmtf::isDefaultValue(data.spaceGroup)) {
    data_map["spaceGroup"] = msgpack::object(data.spaceGroup, m_zone);
  }
  if (!mmtf::isDefaultValue(data.structureId)) {
    data_map["structureId"] = msgpack::object(data.structureId, m_zone);
  }
  if (!mmtf::isDefaultValue(data.title)) {
    data_map["title"] = msgpack::object(data.title, m_zone);
  }
  if (!mmtf::isDefaultValue(data.depositionDate)) {
    data_map["depositionDate"] = msgpack::object(data.depositionDate, m_zone);
  }
  if (!mmtf::isDefaultValue(data.releaseDate)) {
    data_map["releaseDate"] = msgpack::object(data.releaseDate, m_zone);
  }
  // std::vector<std::string>
  data_map["chainIdList"] = msgpack::object(encodeStringVector(data.chainIdList, chain_name_max_length), m_zone);
  if (!mmtf::isDefaultValue(data.chainNameList)) {
    data_map["chainNameList"] = msgpack::object(encodeStringVector(data.chainNameList, chain_name_max_length), m_zone);
  }
  if (!mmtf::isDefaultValue(data.experimentalMethods)) {
    data_map["experimentalMethods"] =
      msgpack::object(data.experimentalMethods, m_zone);
  }
  // std::vector<char>
  if (!mmtf::isDefaultValue(data.altLocList)) {
    data_map["altLocList"] =
      msgpack::object(encodeRunLengthChar(data.altLocList), m_zone);
  }
  if (!mmtf::isDefaultValue(data.insCodeList)) {
    data_map["insCodeList"] = msgpack::object(encodeRunLengthChar(data.insCodeList), m_zone);
  }
  // std::vector<int8_t>
  if (!mmtf::isDefaultValue(data.bondOrderList)) {
    data_map["bondOrderList"] =
      msgpack::object(encodeInt8ToByte(data.bondOrderList), m_zone);
  }
  if (!mmtf::isDefaultValue(data.secStructList)) {
    data_map["secStructList"] =
      msgpack::object(encodeInt8ToByte(data.secStructList), m_zone);
  }
  // int32_t
  data_map["numBonds"] = msgpack::object(data.numBonds, m_zone);
  data_map["numAtoms"] = msgpack::object(data.numAtoms, m_zone);
  data_map["numGroups"] = msgpack::object(data.numGroups, m_zone);
  data_map["numChains"] = msgpack::object(data.numChains, m_zone);
  data_map["numModels"] = msgpack::object(data.numModels, m_zone);
  // std::vector<int32_t>
  data_map["groupTypeList"] = msgpack::object(encodeFourByteInt(data.groupTypeList), m_zone);
  data_map["groupIdList"] = msgpack::object(encodeRunLengthDeltaInt(data.groupIdList), m_zone);
  data_map["groupsPerChain"] = msgpack::object(data.groupsPerChain, m_zone);
  data_map["chainsPerModel"] = msgpack::object(data.chainsPerModel, m_zone);
  if (!mmtf::isDefaultValue(data.bondAtomList)) {
    data_map["bondAtomList"] = msgpack::object(encodeFourByteInt(data.bondAtomList), m_zone);
  }
  if (!mmtf::isDefaultValue(data.atomIdList)) {
    data_map["atomIdList"] = msgpack::object(encodeRunLengthDeltaInt(data.atomIdList), m_zone);
  }
  if (!mmtf::isDefaultValue(data.sequenceIndexList)) {
    data_map["sequenceIndexList"] = msgpack::object(encodeRunLengthDeltaInt(data.sequenceIndexList), m_zone);
  }
  // float
  if (!mmtf::isDefaultValue(data.resolution)) {
    data_map["resolution"] = msgpack::object(data.resolution, m_zone);
  }
  if (!mmtf::isDefaultValue(data.rFree)) {
    data_map["rFree"] = msgpack::object(data.rFree, m_zone);
  }
  if (!mmtf::isDefaultValue(data.rWork)) {
    data_map["rWork"] = msgpack::object(data.rWork, m_zone);
  }
  // std::vector<float>
  data_map["xCoordList"] = msgpack::object(encodeDeltaRecursiveFloat(data.xCoordList, coord_divider), m_zone);
  data_map["yCoordList"] = msgpack::object( encodeDeltaRecursiveFloat(data.yCoordList, coord_divider), m_zone);
  data_map["zCoordList"] = msgpack::object( encodeDeltaRecursiveFloat(data.zCoordList, coord_divider), m_zone);
  if (!mmtf::isDefaultValue(data.bFactorList)) {
    data_map["bFactorList"] = msgpack::object(encodeDeltaRecursiveFloat(data.bFactorList, occupancy_b_factor_divider), m_zone);
  }
  if (!mmtf::isDefaultValue(data.occupancyList)) {
    data_map["occupancyList"] = msgpack::object(encodeRunLengthFloat(data.occupancyList, occupancy_b_factor_divider), m_zone);
  }
  if (!mmtf::isDefaultValue(data.unitCell)) {
      data_map["unitCell"] = msgpack::object(data.unitCell, m_zone);
  }
  // std::vector<GroupType>
  data_map["groupList"] = msgpack::object(data.groupList, m_zone);
  // std::vector<BioAssembly>
  if (!mmtf::isDefaultValue(data.bioAssemblyList)) {
    data_map["bioAssemblyList"] = msgpack::object(data.bioAssemblyList, m_zone);
  }
  // std::vector<Entity>
  if (!mmtf::isDefaultValue(data.entityList)) {
    data_map["entityList"] = msgpack::object(data.entityList, m_zone);
  }
  // std::vector<std::vector<float>>
  if (!mmtf::isDefaultValue(data.ncsOperatorList)) {
    data_map["ncsOperatorList"] = msgpack::object(data.ncsOperatorList, m_zone);
  }
  msgpack::pack(stream, data_map);
}


} // mmtf namespace

#endif
