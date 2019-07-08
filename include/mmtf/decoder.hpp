// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code are: Gabriel Studer, Gerardo Tauriello
// 
// Based on mmtf_c developed by Julien Ferte (http://www.julienferte.com/),
// Anthony Bradley, Thomas Holder with contributions from Yana Valasatava,
// Gazal Kalyan, Alexander Rose.
//
// *************************************************************************

#ifndef MMTF_DECODER_H
#define MMTF_DECODER_H

#include "structure_data.hpp"
#include "errors.hpp"
#include "msgpack_decoders.hpp"
#include "map_decoder.hpp"

#include <msgpack.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace mmtf {

/**
 * @brief Decode an MMTF data structure from a mapDecoder.
 * @param[out] data   MMTF data structure to be filled
 * @param[in]  mapDecoder MapDecoder holding raw mmtf data
 * @throw mmtf::DecodeError if an error occured
 */
inline void decodeFromMapDecoder(StructureData& data, MapDecoder& mapDecoder);

/**
 * @brief Decode an MMTF data structure from a byte buffer.
 * @param[out] data   MMTF data structure to be filled
 * @param[in]  buffer File contents
 * @param[in]  size   Size of buffer
 * @throw mmtf::DecodeError if an error occured
 */
inline void decodeFromBuffer(StructureData& data, const char* buffer,
                             size_t size);

/**
 * @brief Decode an MMTF data structure from a stream.
 *
 * Note that the full stream is read until the end before decoding it!
 *
 * @param[out] data   MMTF data structure to be filled
 * @param[in]  stream Any stream type compatible to std::ostream
 * @throw mmtf::DecodeError if an error occured
 */
template <typename Stream>
inline void decodeFromStream(StructureData& data, Stream& stream);

/**
 * @brief Decode an MMTF data structure from an existing file.
 * @param[out] data     MMTF data structure to be filled
 * @param[in]  filename Path to file to load
 * @throw mmtf::DecodeError if an error occured
 */
inline void decodeFromFile(StructureData& data, const std::string& filename);

/**
 * @brief Get a mapDecoder for un-decoded MMTF data
 * @param[out] mapDecoder  MapDecoder to hold raw mmtf data
 * @param[in]  buffer      File contents
 * @param[in]  size        Size of buffer
 * @throw mmtf::DecodeError if an error occured
 */
inline void mapDecoderFromBuffer(MapDecoder& mapDecoder, const char* buffer,
                                 std::size_t size);

/**
 * @brief Get a mapDecoder into an un-decoded MMTF data
 * @param[out] mapDecoder  MapDecoder to hold raw mmtf data
 * @param[in]  stream      Stream that holds mmtf data
 * @throw mmtf::DecodeError if an error occured
 */
template <typename Stream>
inline void mapDecoderFromStream(MapDecoder& mapDecoder, Stream& stream);

/**
 * @brief Get a mapDecoder into an un-decoded MMTF data
 * @param[out] mapDecoder  MapDecoder to hold raw mmtf data
 * @param[in]  filename    Stream that holds mmtf data
 * @throw mmtf::DecodeError if an error occured
 */
inline void mapDecoderFromFile(MapDecoder& mapDecoder,
                               const std::string& filename);

// *************************************************************************
// IMPLEMENTATION
// *************************************************************************

inline void decodeFromMapDecoder(StructureData& data, MapDecoder& md) {
    md.decode("mmtfVersion", true, data.mmtfVersion);

    // check if version is compatible before continuing
    if (!mmtf::isVersionSupported(data.mmtfVersion)) {
        throw mmtf::DecodeError("Unsupported MMTF version "
                                + data.mmtfVersion);
    }

    md.decode("mmtfProducer", true, data.mmtfProducer);
    md.decode("unitCell", false, data.unitCell);
    md.decode("spaceGroup", false, data.spaceGroup);
    md.decode("structureId", false, data.structureId);
    md.decode("title", false, data.title);
    md.decode("depositionDate", false, data.depositionDate);
    md.decode("releaseDate", false, data.releaseDate);
    md.decode("ncsOperatorList", false, data.ncsOperatorList);
    md.decode("bioAssemblyList", false, data.bioAssemblyList);
    md.decode("entityList", false, data.entityList);
    md.decode("experimentalMethods", false, data.experimentalMethods);
    md.decode("resolution", false, data.resolution);
    md.decode("rFree", false, data.rFree);
    md.decode("rWork", false, data.rWork);
    md.decode("numBonds", true, data.numBonds);
    md.decode("numAtoms", true, data.numAtoms);
    md.decode("numGroups", true, data.numGroups);
    md.decode("numChains", true, data.numChains);
    md.decode("numModels", true, data.numModels);
    md.decode("groupList", true, data.groupList);
    md.decode("bondAtomList", false, data.bondAtomList);
    md.decode("bondOrderList", false, data.bondOrderList);
    md.decode("bondResonanceList", false, data.bondResonanceList);
    md.decode("xCoordList", true, data.xCoordList);
    md.decode("yCoordList", true, data.yCoordList);
    md.decode("zCoordList", true, data.zCoordList);
    md.decode("bFactorList", false, data.bFactorList);
    md.decode("atomIdList", false, data.atomIdList);
    md.decode("altLocList", false, data.altLocList);
    md.decode("occupancyList", false, data.occupancyList);
    md.decode("groupIdList", true, data.groupIdList);
    md.decode("groupTypeList", true, data.groupTypeList);
    md.decode("secStructList", false, data.secStructList);
    md.decode("insCodeList", false, data.insCodeList);
    md.decode("sequenceIndexList", false, data.sequenceIndexList);
    md.decode("chainIdList", true, data.chainIdList);
    md.decode("chainNameList", false, data.chainNameList);
    md.decode("groupsPerChain", true, data.groupsPerChain);
    md.decode("chainsPerModel", true, data.chainsPerModel);
    // extraProperties (application specific stuff)
    // Perform expensive copy if exists.
    // Implement outside accessor if speed is necessary
    md.copy_decode("bondProperties", false, data.bondProperties,
                   data.msgpack_zone);
    md.copy_decode("atomProperties", false, data.atomProperties,
                   data.msgpack_zone);
    md.copy_decode("groupProperties", false, data.groupProperties,
                   data.msgpack_zone);
    md.copy_decode("chainProperties", false, data.chainProperties,
                   data.msgpack_zone);
    md.copy_decode("modelProperties", false, data.modelProperties,
                   data.msgpack_zone);
    md.copy_decode("extraProperties", false, data.extraProperties,
                   data.msgpack_zone);
    md.checkExtraKeys();
}

inline void decodeFromBuffer(StructureData& data, const char* buffer,
                             size_t size) {
    MapDecoder md;
    mapDecoderFromBuffer(md, buffer, size);
    decodeFromMapDecoder(data, md);
}

template <typename Stream>
inline void decodeFromStream(StructureData& data, Stream& stream) {
    MapDecoder md;
    mapDecoderFromStream(md, stream);
    decodeFromMapDecoder(data, md);
}

inline void decodeFromFile(StructureData& data, const std::string& filename) {
    MapDecoder md;
    mapDecoderFromFile(md, filename);
    decodeFromMapDecoder(data, md);
}

inline void mapDecoderFromBuffer(MapDecoder& mapDecoder, const char* buffer,
                                 std::size_t size) {
    mapDecoder.initFromBuffer(buffer, size);
}

template <typename Stream>
inline void mapDecoderFromStream(MapDecoder& mapDecoder, Stream& stream) {
    // parse straight into string buffer
    std::string buffer;
    stream.seekg(0, std::ios::end);
    buffer.resize(stream.tellg());
    stream.seekg(0, std::ios::beg);
    if (!buffer.empty()) stream.read(&buffer[0], buffer.size());
    mapDecoderFromBuffer(mapDecoder, buffer.data(), buffer.size());
}

inline void mapDecoderFromFile(MapDecoder& mapDecoder,
                               const std::string& filename) {
    // read file as binary
    std::ifstream ifs(filename.c_str(), std::ifstream::in | std::ios::binary);
    if (!ifs.is_open()) {
        throw DecodeError("Could not open file: " + filename);
    }
    mapDecoderFromStream(mapDecoder, ifs);
}

} // mmtf namespace

#endif
