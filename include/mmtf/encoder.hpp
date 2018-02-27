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
 * @brief Encode an MMTF data structure into a file.
 * @param[in] data          MMTF data structure to be stored
 * @param[in] filename      Path to file to load
 * @throw mmtf::EncodeError if an error occured
 */
inline void encodeToFile(const StructureData& data,
                         const std::string& filename);

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
inline std::vector<char> encodeFourByteToInt(std::vector<int32_t> vec_in);

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
  for (int i=0; i<vec_in.size(); ++i) {
    vec_out.push_back(round(vec_in[i]*multiplier));
  }
  return vec_out;
}


inline std::vector<int32_t> deltaEncode(std::vector<int32_t> const & vec_in) {
  std::vector<int32_t> vec_out;
  vec_out.push_back(vec_in[0]);
  for (int32_t i=1; i<vec_in.size(); ++i) {
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
  for (int32_t i = 1; i < vec_in.size(); ++i) {
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
  for (int32_t i=0; i<vec_in.size(); ++i) {
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
  for (int i=0; i<vec_in.size(); ++i) {
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
  for (int i=0; i<vec_in.size(); ++i) {
    ss.write(reinterpret_cast< char * >(&vec_in[i]), sizeof(vec_in[i]));
  }
  return stringstreamToCharVector(ss);
}


inline std::vector<char> encodeFourByteToInt(std::vector<int32_t> vec_in) {
  std::stringstream ss;
  add_header(ss, vec_in.size(), 4, 0);
  for (int i=0; i<vec_in.size(); ++i) {
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
  for (int i=0; i<in_sv.size(); ++i) {
    char_vec.insert(char_vec.end(), in_sv[i].begin(), in_sv[i].end());
    for (int j=0; j<CHAIN_LEN-in_sv[i].size(); ++j) {
      char_vec.push_back(NULL_BYTE);
    }
  }
  for (int i=0; i<char_vec.size(); ++i) {
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
  for (int i=0; i<int_vec.size(); ++i) {
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
  for (int i=0; i<int_vec.size(); ++i) {
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
  for (int i=0; i<int_vec.size(); ++i) {
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
  for (int i=0; i<int_vec.size(); ++i) {
    int16_t temp = htobe16(int_vec[i]);
    ss.write(reinterpret_cast< char * >(&temp), sizeof(temp));
  }
  return stringstreamToCharVector(ss);
}


inline void encodeToFile(StructureData& struct_data, const std::string& filename) {
    // encode to a file
    std::ofstream ofs(filename.c_str(), std::ios::binary | std::ios::out );
    if ( !ofs ) {
        throw EncodeError("Could not open >" + filename + "< for writing, exiting.");
    }

    std::map<std::string, msgpack::object> data_map;
    msgpack::zone m_zone;
    //inline std::vector< uint32_t > encodeArray(std::vector<T> input_array, uint32_t codec, uint32_t param ) {
    // std::string
    data_map["mmtfVersion"] = msgpack::object(struct_data.mmtfVersion, m_zone);
    data_map["mmtfProducer"] = msgpack::object(struct_data.mmtfProducer, m_zone);
    data_map["spaceGroup"] = msgpack::object(struct_data.spaceGroup, m_zone);
    data_map["structureId"] = msgpack::object(struct_data.structureId, m_zone);
    data_map["title"] = msgpack::object(struct_data.title, m_zone);
    data_map["depositionDate"] = msgpack::object(struct_data.depositionDate, m_zone);
    data_map["releaseDate"] = msgpack::object(struct_data.releaseDate, m_zone);
    // std::vector<std::string>
    data_map["experimentalMethods"] = msgpack::object(struct_data.experimentalMethods, m_zone);
    data_map["chainIdList"] = msgpack::object(encodeStringVector(struct_data.chainIdList, 4), m_zone);
    data_map["chainNameList"] = msgpack::object(encodeStringVector(struct_data.chainNameList, 4), m_zone);
    // std::vector<char>
    data_map["altLocList"] = msgpack::object(encodeRunLengthChar(struct_data.altLocList), m_zone);
    data_map["insCodeList"] = msgpack::object(encodeRunLengthChar(struct_data.insCodeList), m_zone);
    // std::vector<int8_t>
    data_map["bondOrderList"] = msgpack::object(encodeInt8ToByte(struct_data.bondOrderList), m_zone);
    data_map["secStructList"] = msgpack::object(encodeInt8ToByte(struct_data.secStructList), m_zone);
    // int32_t
    data_map["numBonds"] = msgpack::object(struct_data.numBonds, m_zone);
    data_map["numAtoms"] = msgpack::object(struct_data.numAtoms, m_zone);
    data_map["numGroups"] = msgpack::object(struct_data.numGroups, m_zone);
    data_map["numChains"] = msgpack::object(struct_data.numChains, m_zone);
    data_map["numModels"] = msgpack::object(struct_data.numModels, m_zone);
    // std::vector<int32_t>
    data_map["bondAtomList"] = msgpack::object(encodeFourByteToInt(struct_data.bondAtomList), m_zone);
    data_map["groupTypeList"] = msgpack::object(encodeFourByteToInt(struct_data.groupTypeList), m_zone);
    data_map["groupIdList"] = msgpack::object(encodeRunLengthDeltaInt(struct_data.groupIdList), m_zone);
    data_map["atomIdList"] = msgpack::object(encodeRunLengthDeltaInt(struct_data.atomIdList), m_zone);
    data_map["sequenceIndexList"] = msgpack::object(encodeRunLengthDeltaInt(struct_data.sequenceIndexList), m_zone);
    data_map["groupsPerChain"] = msgpack::object(struct_data.groupsPerChain, m_zone);
    data_map["chainsPerModel"] = msgpack::object(struct_data.chainsPerModel, m_zone);
    // float
    data_map["unitCell"] = msgpack::object(struct_data.unitCell, m_zone);
    data_map["resolution"] = msgpack::object(struct_data.resolution, m_zone);
    data_map["rFree"] = msgpack::object(struct_data.rFree, m_zone);
    data_map["rWork"] = msgpack::object(struct_data.rWork, m_zone);
    // std::vector<float>
    data_map["xCoordList"] = msgpack::object(encodeDeltaRecursiveFloat(struct_data.xCoordList, 1000), m_zone);
    data_map["yCoordList"] = msgpack::object(encodeDeltaRecursiveFloat(struct_data.yCoordList, 1000), m_zone);
    data_map["zCoordList"] = msgpack::object(encodeDeltaRecursiveFloat(struct_data.zCoordList, 1000), m_zone);
    data_map["bFactorList"] = msgpack::object(encodeDeltaRecursiveFloat(struct_data.bFactorList, 100), m_zone);
    data_map["occupancyList"] = msgpack::object(encodeRunLengthFloat(struct_data.occupancyList, 100), m_zone);
    // std::vector<GroupType>
    data_map["groupList"] = msgpack::object(struct_data.groupList, m_zone);
    // std::vector<BioAssembly>
    data_map["bioAssemblyList"] = msgpack::object(struct_data.bioAssemblyList, m_zone);
    // std::vector<Entity>
    data_map["entityList"] = msgpack::object(struct_data.entityList, m_zone);
    // std::vector<std::vector<float>>
    data_map["ncsOperatorList"] = msgpack::object(struct_data.ncsOperatorList, m_zone);
    msgpack::pack(ofs, data_map);
}


} // mmtf namespace

#endif
