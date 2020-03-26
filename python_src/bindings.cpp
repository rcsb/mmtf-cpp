
#include <mmtf.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <sstream>

#include <msgpack.hpp>

namespace py = pybind11;

/// CPP -> PY FUNCTIONS

/* Notes
 * We destory original data because it is much faster to apply move 
 * than it is to copy the data.
 */

// This destroys the original data
template< typename T >
py::array
array1d_from_vector(std::vector<T> & m) {
	if (m.empty()) return py::array_t<T>();
	std::vector<T>* ptr = new std::vector<T>(std::move(m));
	auto capsule = py::capsule(ptr, [](void* p) {
			delete reinterpret_cast<std::vector<T>*>(p);
	});
	return py::array_t<T>(
			ptr->size(),  // shape of array
			ptr->data(),  // c-style contiguous strides for Sequence
			capsule       // numpy array references this parent
	);
}


template<>
py::array
array1d_from_vector(std::vector<char> & m) {
	//if (m.empty()) return py::array_t<char>();
	std::vector<char>* ptr = new std::vector<char>(std::move(m));
	auto capsule = py::capsule(ptr, [](void* p) {
			delete reinterpret_cast<std::vector<char>*>(p);
	});
	return py::array(
			py::dtype("<S1"),
			{ptr->size()},  // shape of array
			{},
			ptr->data(),  // c-style contiguous strides for Sequence
			capsule       // numpy array references this parent
	);
}

template< >
py::array
array1d_from_vector(std::vector<std::string> & m) {
	return py::array(py::cast(std::move(m)));
}

template <typename T>
std::vector<T>
flatten2D(std::vector<std::vector<T>> const & v) {
	std::size_t total_size = 0;
	for (auto const & x : v)
			total_size += x.size();
	std::vector<T> result;
	result.reserve(total_size);
	for (auto const & subv : v)
			result.insert(result.end(), subv.begin(), subv.end());
	return result;
}


// would be nice if this was faster
template< typename T >
py::array
array2D_from_vector(std::vector<std::vector<T>> const & m) {
	if (m.empty()) return py::array_t<T>();
	std::vector<T>* ptr = new std::vector<T>(flatten2D(m));
	auto capsule = py::capsule(ptr, [](void* p) {
			delete reinterpret_cast<std::vector<T>*>(p);
	});
	return py::array_t<T>(
			{m.size(), m.at(0).size()},           // shape of array
			{m.at(0).size()*sizeof(T), sizeof(T)},   // c-style contiguous strides
			ptr->data(),
			capsule);
}

// This destroys the original data
py::list
dump_bio_assembly_list(mmtf::StructureData & sd) {
	py::object py_ba_class = py::module::import("mmtf_cppy").attr("BioAssembly");
	py::object py_t_class = py::module::import("mmtf_cppy").attr("Transform");
	py::list bal;
	for (mmtf::BioAssembly & cba : sd.bioAssemblyList) {
		py::list transform_list;
		for (mmtf::Transform & trans : cba.transformList) {
			std::vector<float> matrix(std::begin(trans.matrix), std::end(trans.matrix));
			transform_list.append(
					py_t_class(
						array1d_from_vector(trans.chainIndexList),
						array1d_from_vector(matrix)
					)
			);
		}
		bal.append(
				py_ba_class(
					transform_list,
					py::str(cba.name)
				)
		);
	}
	return bal;
}

// This destroys the original data
py::list
dump_entity_list(std::vector<mmtf::Entity> & cpp_el) {
	py::object entity = py::module::import("mmtf_cppy").attr("Entity");
	py::list el;
	for (mmtf::Entity & e : cpp_el) {
		el.append(
				entity(
					array1d_from_vector(e.chainIndexList),
					e.description,
					e.type,
					e.sequence)
		);
	}
	return el;
}

py::bytes
raw_properties(mmtf::StructureData const & sd) {
	std::stringstream bytes;
	std::map< std::string, std::map< std::string, msgpack::object > > objs({
			{"bondProperties", sd.bondProperties},
			{"atomProperties", sd.atomProperties},
			{"groupProperties", sd.groupProperties},
			{"chainProperties", sd.chainProperties},
			{"modelProperties", sd.modelProperties},
			{"extraProperties", sd.extraProperties}});
	msgpack::pack(bytes, objs);
	return py::bytes(bytes.str());
}


std::vector<mmtf::Transform>
make_transformList(py::list const & l) {
	std::vector<mmtf::Transform> tl;
	for (auto const & trans : l) {
		mmtf::Transform t;
		t.chainIndexList = trans.attr("chainIndexList").cast<std::vector<int>>();
		py::list pymatrix(trans.attr("matrix"));
		std::size_t count(0);
		for (auto const & x : pymatrix) {
			t.matrix[count] = x.cast<float>();
			++count;
		}
		tl.push_back(t);
	}
	return tl;
}


void
set_bioAssemblyList(py::list const & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::BioAssembly> bioAs;
	for (auto const & py_bioAssembly : obj ) {
		mmtf::BioAssembly bioA;
		bioA.name = py::str(py_bioAssembly.attr("name"));
		py::list py_transform_list(py_bioAssembly.attr("transformList"));
		std::vector<mmtf::Transform> transform_list = make_transformList(py_transform_list);
		bioA.transformList = transform_list;
		bioAs.push_back(bioA);
	}
	sd.bioAssemblyList = bioAs;
}


void
set_entityList(py::list const & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::Entity> entities;
	for (auto const & py_entity : obj ) {
		mmtf::Entity entity;
		entity.chainIndexList = py_entity.attr("chainIndexList").cast<std::vector<int32_t>>();
		entity.description = py_entity.attr("description").cast<std::string>();
		entity.type = py_entity.attr("type").cast<std::string>();
		entity.sequence = py_entity.attr("sequence").cast<std::string>();
		entities.push_back(entity);
	}
	sd.entityList = entities;
}


void
set_groupList(py::list const & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::GroupType> groups;
	for (auto const & py_group : obj ) {
		mmtf::GroupType group;
		group.formalChargeList = py_group.attr("formalChargeList").cast<std::vector<int32_t>>();
		group.atomNameList = py_group.attr("atomNameList").cast<std::vector<std::string>>();
		group.elementList = py_group.attr("elementList").cast<std::vector<std::string>>();
		group.bondAtomList = py_group.attr("bondAtomList").cast<std::vector<int32_t>>();
		group.bondOrderList = py_group.attr("bondOrderList").cast<std::vector<int8_t>>();
		group.bondResonanceList = py_group.attr("bondResonanceList").cast<std::vector<int8_t>>();
		group.groupName = py_group.attr("groupName").cast<std::string>();
		group.singleLetterCode = py_group.attr("singleLetterCode").cast<char>();
		group.chemCompType = py_group.attr("chemCompType").cast<std::string>();
		groups.push_back(group);
	}
	sd.groupList = groups;
}


// This destroys the original data
py::list
dump_group_list(std::vector<mmtf::GroupType> & gtl) {
	py::object py_gt_class = py::module::import("mmtf_cppy").attr("GroupType");
	py::list gl;
	for (mmtf::GroupType & gt : gtl) {
		gl.append(
				py_gt_class(
					array1d_from_vector(gt.formalChargeList),
					gt.atomNameList,
					gt.elementList,
					array1d_from_vector(gt.bondAtomList),
					array1d_from_vector(gt.bondOrderList),
					array1d_from_vector(gt.bondResonanceList),
					gt.groupName,
					std::string(1, gt.singleLetterCode),
					gt.chemCompType
				)
		);
	}
	return gl;
}

template< typename T>
std::vector<T>
py_array_to_vector(py::array_t<T, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<T> vec_array(array_in.size());
	std::memcpy(vec_array.data(), array_in.data(), array_in.size()*sizeof(T));
	return vec_array;
}

template<>
std::vector<char>
py_array_to_vector(py::array_t<char, py::array::c_style | py::array::forcecast> const & array_in) {
	std::string tmpstr(array_in.data(), array_in.size());
	std::vector<char> vec_array(tmpstr.begin(), tmpstr.end());
	return vec_array;
}

/* This isn't really necessary, but lets make the interface anyway
 */
py::bytes
py_encodeInt8ToByte(py::array_t<int8_t, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<char> cpp_vec(mmtf::encodeInt8ToByte(py_array_to_vector<int8_t>(array_in)));
	return py::bytes(std::string(cpp_vec.begin(), cpp_vec.end()));
}

py::bytes
py_encodeFourByteInt(py::array_t<int32_t, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<int32_t> const cpp_vec(py_array_to_vector<int32_t>(array_in));
	std::vector<char> encoded(mmtf::encodeFourByteInt(cpp_vec));
	return py::bytes(encoded.data(), encoded.size());
}

py::bytes
py_encodeRunLengthChar(py::array_t<char, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<char> const cpp_vec(py_array_to_vector<char>(array_in));
	std::vector<char> encoded(mmtf::encodeRunLengthChar(cpp_vec));
	return py::bytes(encoded.data(), encoded.size());
}

py::bytes
py_encodeRunLengthDeltaInt(py::array_t<int32_t, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<int32_t> const cpp_vec(py_array_to_vector<int32_t>(array_in));
	std::vector<char> encoded(mmtf::encodeRunLengthDeltaInt(cpp_vec));
	return py::bytes(encoded.data(), encoded.size());
}

py::bytes
py_encodeDeltaRecursiveFloat(py::array_t<float, py::array::c_style | py::array::forcecast> const & array_in, int32_t const multiplier = 1000) {
	std::vector<float> const cpp_vec(py_array_to_vector<float>(array_in));
	std::vector<char> encoded(mmtf::encodeDeltaRecursiveFloat(cpp_vec, multiplier));
	return py::bytes(encoded.data(), encoded.size());
}

py::bytes
py_encodeRunLengthFloat(py::array_t<float, py::array::c_style | py::array::forcecast> const & array_in, int32_t const multiplier = 1000) {
	std::vector<float> const cpp_vec(py_array_to_vector<float>(array_in));
	std::vector<char> encoded(mmtf::encodeRunLengthFloat(cpp_vec, multiplier));
	return py::bytes(encoded.data(), encoded.size());
}



py::bytes
py_encodeRunLengthInt8(py::array_t<int8_t, py::array::c_style | py::array::forcecast> const & array_in) {
	std::vector<int8_t> const cpp_vec(py_array_to_vector<int8_t>(array_in));
	std::vector<char> encoded(mmtf::encodeRunLengthInt8(cpp_vec));
	return py::bytes(encoded.data(), encoded.size());
}

// TODO pyarray POD types to numpy array? seems hard
//py::bytes
//py_encodeStringVector4(py::array_t<std::array<char, 4>> const & array_in) {
//	using np_str_t = std::array<char, 4>;
//	pybind11::array_t<np_str_t> cstring_array(vector.size());
//	const char * data = 
//	np_str_t* array_of_cstr_ptr = reinterpret_cast<np_str_t*>(cstring_array.request().ptr);
//
//
///* 	std::vector<std::string> tobuild; */
///* 	std::vector<std::string> const cpp_vec(py_array_to_vector<std::array[char, max_string_size]>(array_in)); */
///* 	std::vector<char> encoded(mmtf::encodeStringVector(cpp_vec, max_string_size)); */
///* 	return py::bytes(encoded.data(), encoded.size()); */
//}


std::vector<std::string>
char_vector_to_string_vector(std::vector<char> const & cvec) {
	std::vector<std::string> ret(cvec.size());
	for (std::size_t i=0; i<cvec.size(); ++i) {
		ret[i] = std::string(1, cvec[i]);
	}
	return ret;
}

void
set_properties(mmtf::StructureData & sd, py::bytes const & bytes_in) {
	std::size_t offset(0);
	std::string const xbits(bytes_in);
	msgpack::object tmp_object(msgpack::unpack(sd.msgpack_zone, xbits.data(), xbits.size(), offset));
	std::map< std::string, std::map< std::string, msgpack::object> > tmp_target;
	tmp_object.convert(tmp_target);
	sd.bondProperties = tmp_target["bondProperties"];
	sd.atomProperties = tmp_target["atomProperties"];
	sd.groupProperties = tmp_target["groupProperties"];
	sd.chainProperties = tmp_target["chainProperties"];
	sd.modelProperties = tmp_target["modelProperties"];
	sd.extraProperties = tmp_target["extraProperties"];
}


py::array
binary_decode_int32(py::bytes const & bytes) {
	using namespace pybind11::literals;
	std::string const tmpstr(bytes);
	std::vector<int32_t> tmp;
	mmtf::BinaryDecoder bd(tmpstr);
	bd.decode(tmp);
	return array1d_from_vector<int32_t>(tmp);
}

py::array
binary_decode_int16(py::bytes const & bytes) {
	using namespace pybind11::literals;
	std::string const tmpstr(bytes);
	std::vector<int16_t> tmp;
	mmtf::BinaryDecoder bd(tmpstr);
	bd.decode(tmp);
	return array1d_from_vector<int16_t>(tmp);
}

py::array
binary_decode_int8(py::bytes const & bytes) {
	using namespace pybind11::literals;
	std::string const tmpstr(bytes);
	std::vector<int8_t> tmp;
	mmtf::BinaryDecoder bd(tmpstr);
	bd.decode(tmp);
	return array1d_from_vector<int8_t>(tmp);
}


py::array
binary_decode_char(py::bytes const & bytes) {
	using namespace pybind11::literals;
	std::string const tmpstr(bytes);
	std::vector<char> tmp;
	mmtf::BinaryDecoder bd(tmpstr);
	bd.decode(tmp);
	return array1d_from_vector<char>(tmp);
}

py::array
binary_decode_float(py::bytes const & bytes) {
	using namespace pybind11::literals;
	std::string const tmpstr(bytes);
	std::vector<float> tmp;
	mmtf::BinaryDecoder bd(tmpstr);
	bd.decode(tmp);
	return array1d_from_vector<float>(tmp);
}


PYBIND11_MODULE(mmtf_bindings, m) {
	m.def("decode_int32", &binary_decode_int32, "decode array[int32_t]");
	m.def("decode_int16", &binary_decode_int16, "decode array[int16_t]");
	m.def("decode_int8", &binary_decode_int8, "decode array[int8_t]");
	m.def("decode_char", &binary_decode_char, "decode array[char]");
	m.def("decode_float", &binary_decode_float, "decode array[float]");
	// new stuff here
	py::class_<mmtf::StructureData>(m, "CPPStructureData")
		.def( pybind11::init( [](){ return new mmtf::StructureData(); } ) )
		.def( pybind11::init( [](mmtf::StructureData const &o){ return new mmtf::StructureData(o); } ) )
		.def_readwrite("mmtfVersion", &mmtf::StructureData::mmtfVersion)
		.def_readwrite("mmtfProducer", &mmtf::StructureData::mmtfProducer)
		.def("unitCell", [](mmtf::StructureData &m){return array1d_from_vector(m.unitCell);})
		.def_readwrite("unitCell_io", &mmtf::StructureData::unitCell)
		.def_readwrite("spaceGroup", &mmtf::StructureData::spaceGroup)
		.def_readwrite("structureId", &mmtf::StructureData::structureId)
		.def_readwrite("title", &mmtf::StructureData::title)
		.def_readwrite("depositionDate", &mmtf::StructureData::depositionDate)
		.def_readwrite("releaseDate", &mmtf::StructureData::releaseDate)
		.def("ncsOperatorList", [](mmtf::StructureData &m){return array2D_from_vector<float>(m.ncsOperatorList);})
		.def_readwrite("ncsOperatorList_io", &mmtf::StructureData::ncsOperatorList, py::return_value_policy::move)
		.def("bioAssemblyList", [](mmtf::StructureData &m){return dump_bio_assembly_list(m);})
		.def_readwrite("bioAssemblyList_io", &mmtf::StructureData::bioAssemblyList)
		.def("entityList", [](mmtf::StructureData &m){return dump_entity_list(m.entityList);})
		.def_readwrite("entityList_io", &mmtf::StructureData::entityList)
		.def_readwrite("experimentalMethods", &mmtf::StructureData::experimentalMethods)
		.def_readwrite("resolution", &mmtf::StructureData::resolution)
		.def_readwrite("rFree", &mmtf::StructureData::rFree)
		.def_readwrite("rWork", &mmtf::StructureData::rWork)
		.def_readwrite("numBonds", &mmtf::StructureData::numBonds)
		.def_readwrite("numAtoms", &mmtf::StructureData::numAtoms)
		.def_readwrite("numGroups", &mmtf::StructureData::numGroups)
		.def_readwrite("numChains", &mmtf::StructureData::numChains)
		.def_readwrite("numModels", &mmtf::StructureData::numModels)
		.def("groupList", [](mmtf::StructureData &m){return dump_group_list(m.groupList);})
		.def_readwrite("groupList_io", &mmtf::StructureData::groupList)
		.def("unitCell", [](mmtf::StructureData &m){return array1d_from_vector(m.unitCell);})
		.def_readwrite("unitCell_io", &mmtf::StructureData::unitCell)
		.def("bondAtomList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondAtomList);})
		.def_readwrite("bondAtomList_io", &mmtf::StructureData::bondAtomList)
		.def("bondOrderList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondOrderList);})
		.def_readwrite("bondOrderList_io", &mmtf::StructureData::bondOrderList)
		.def("bondResonanceList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondResonanceList);})
		.def_readwrite("bondResonanceList_io", &mmtf::StructureData::bondResonanceList)
		.def("xCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.xCoordList);})
		.def_readwrite("xCoordList_io", &mmtf::StructureData::xCoordList)
		.def("yCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.yCoordList);})
		.def_readwrite("yCoordList_io", &mmtf::StructureData::yCoordList)
		.def("zCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.zCoordList);})
		.def_readwrite("zCoordList_io", &mmtf::StructureData::zCoordList)
		.def("bFactorList", [](mmtf::StructureData &m){return array1d_from_vector(m.bFactorList);})
		.def_readwrite("bFactorList_io", &mmtf::StructureData::bFactorList)
		.def("atomIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.atomIdList);})
		.def_readwrite("atomIdList_io", &mmtf::StructureData::atomIdList)
		.def("altLocList", [](mmtf::StructureData &m) {
			return array1d_from_vector<char>(m.altLocList);
			/* std::vector<std::string> tmp(char_vector_to_string_vector(m.altLocList)); */
			/* return array1d_from_vector(tmp); */
		})
		.def("set_altLocList", [](mmtf::StructureData &m, py::array_t<int8_t> const & st) {
				m.altLocList = std::vector<char>(st.data(), st.data()+st.size());
		})
		.def("occupancyList", [](mmtf::StructureData &m){return array1d_from_vector(m.occupancyList);})
		.def_readwrite("occupancyList_io", &mmtf::StructureData::occupancyList)
		.def("groupIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupIdList);})
		.def_readwrite("groupIdList_io", &mmtf::StructureData::groupIdList)
		.def("groupTypeList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupTypeList);})
		.def_readwrite("groupTypeList_io", &mmtf::StructureData::groupTypeList)
		.def("secStructList", [](mmtf::StructureData &m){return array1d_from_vector(m.secStructList);})
		.def_readwrite("secStructList_io", &mmtf::StructureData::secStructList)
		.def("insCodeList", [](mmtf::StructureData &m) {
			std::vector<std::string> tmp(char_vector_to_string_vector(m.insCodeList));
			return array1d_from_vector(tmp);
		})
		.def("set_insCodeList", [](mmtf::StructureData &m, py::array_t<char> const & st) {
			m.insCodeList = std::vector<char>(st.data(), st.data()+st.size());
		})
		.def("sequenceIndexList", [](mmtf::StructureData &m){return array1d_from_vector(m.sequenceIndexList);})
		.def_readwrite("sequenceIndexList_io", &mmtf::StructureData::sequenceIndexList)
		.def("chainIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.chainIdList);})
		.def_readwrite("chainIdList_io", &mmtf::StructureData::chainIdList)
		.def("chainNameList", [](mmtf::StructureData &m){return array1d_from_vector(m.chainNameList);})
		.def_readwrite("chainNameList_io", &mmtf::StructureData::chainNameList)
		.def("groupsPerChain", [](mmtf::StructureData &m){return array1d_from_vector(m.groupsPerChain);})
		.def_readwrite("groupsPerChain_io", &mmtf::StructureData::groupsPerChain)
		.def("chainsPerModel", [](mmtf::StructureData &m){return array1d_from_vector(m.chainsPerModel);})
		.def_readwrite("chainsPerModel_io", &mmtf::StructureData::chainsPerModel)
		.def("set_properties", [](mmtf::StructureData & sd, py::bytes const & bytes_in){set_properties(sd, bytes_in);})
		.def("raw_properties", [](mmtf::StructureData const &m){return raw_properties(m);});
 
	// I think it would be ideal to not pass in the sd, but it is still very
	// fast this way.
	m.def("set_bioAssemblyList", [](py::list const & i, mmtf::StructureData & sd){return set_bioAssemblyList(i, sd);});
	m.def("set_entityList", [](py::list const & i, mmtf::StructureData & sd){return set_entityList(i, sd);});
	m.def("set_groupList", [](py::list const & i, mmtf::StructureData & sd){return set_groupList(i, sd);});
	m.def("decodeFromFile", &mmtf::decodeFromFile, "decode a mmtf::StructureData from a file");
	m.def("decodeFromBuffer", &mmtf::decodeFromBuffer, "decode a mmtf::StructureData from bytes");
	m.def("encodeToFile", [](mmtf::StructureData const &m, std::string const & fn){mmtf::encodeToFile(m, fn);});
	m.def("encodeToStream", [](mmtf::StructureData const &m){
		std::stringstream ss;
		mmtf::encodeToStream(m, ss);
		return py::bytes(ss.str());
	});
	// encoders
	m.def("encodeInt8ToByte", &py_encodeInt8ToByte);
	m.def("encodeFourByteInt", &py_encodeFourByteInt);
	m.def("encodeRunLengthChar", &py_encodeRunLengthChar);
	m.def("encodeRunLengthDeltaInt", &py_encodeRunLengthDeltaInt);
	m.def("encodeDeltaRecursiveFloat", &py_encodeDeltaRecursiveFloat);
	m.def("encodeRunLengthFloat", &py_encodeRunLengthFloat);
	m.def("encodeRunLengthInt8", &py_encodeRunLengthInt8);
	//m.def("encodeStringVector", &py_encodeStringVector);
}
