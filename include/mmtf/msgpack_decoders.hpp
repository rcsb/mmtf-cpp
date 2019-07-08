// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code are: Gabriel Studer, Gerardo Tauriello, and
// Daniel Farrell.
// 
// Based on mmtf_c developed by Julien Ferte (http://www.julienferte.com/),
// Anthony Bradley, Thomas Holder with contributions from Yana Valasatava,
// Gazal Kalyan, Alexander Rose.
//
// *************************************************************************

#ifndef MMTF_MSGPACK_DECODERS_H
#define MMTF_MSGPACK_DECODERS_H

#include "structure_data.hpp"
#include "map_decoder.hpp"
#include "errors.hpp"

#include <msgpack.hpp>

// here we specialize msgpack-c functionality so we can use obj.convert(..)

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

// custom specialization for chars stored as strings
template <>
struct convert<char> {
    const msgpack::object& operator()(const msgpack::object& obj, 
                                      char& c) const {
        // extract string
        std::string temp;
        obj.convert(temp);
        // ensure length
        if (temp.size() != 1) {
            throw mmtf::DecodeError("Observed single letter string not being "
                                    "of length one!");
        }
        c = temp[0];
        return obj;
    }
};

template <>
struct convert<mmtf::GroupType> {
    const msgpack::object& operator()(const msgpack::object& obj, 
                                      mmtf::GroupType& group) const {

        mmtf::MapDecoder md(obj);
        md.decode("formalChargeList", true, group.formalChargeList);
        md.decode("atomNameList", true, group.atomNameList);
        md.decode("elementList", true, group.elementList);
        md.decode("bondAtomList", false, group.bondAtomList);
        md.decode("bondOrderList", false, group.bondOrderList);
        md.decode("bondResonanceList", false, group.bondResonanceList);
        md.decode("groupName", true, group.groupName);
        md.decode("singleLetterCode", true, group.singleLetterCode);
        md.decode("chemCompType", true, group.chemCompType);
        md.checkExtraKeys();
        return obj;
    }
};

template <>
struct convert<mmtf::Entity> {
    const msgpack::object& operator()(const msgpack::object& obj, 
                                      mmtf::Entity& entity) const {

        mmtf::MapDecoder md(obj);
        md.decode("chainIndexList", true, entity.chainIndexList);
        md.decode("description", true, entity.description);
        md.decode("type", true, entity.type);
        md.decode("sequence", true, entity.sequence);
        md.checkExtraKeys();
        return obj;
    }
};

template <>
struct convert<mmtf::Transform> {
    const msgpack::object& operator()(const msgpack::object& obj, 
                                      mmtf::Transform& transform) const {

        mmtf::MapDecoder md(obj);
        md.decode("chainIndexList", true, transform.chainIndexList);
        md.decode("matrix", true, transform.matrix);
        md.checkExtraKeys();
        return obj;
    }
};

template <>
struct convert<mmtf::BioAssembly> {
    const msgpack::object& operator()(const msgpack::object& obj, 
                                      mmtf::BioAssembly& assembly) const {

        mmtf::MapDecoder md(obj);
        md.decode("transformList", true, assembly.transformList);
        md.decode("name", true, assembly.name);
        md.checkExtraKeys();
        return obj;
    }
};

}}} // msgpack::VERSION::adaptor namespace

#endif
