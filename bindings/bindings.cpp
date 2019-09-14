
//#ifndef MMTF_CPP_PYBIND_BINDINGS_HH
//#define MMTF_CPP_PYBIND_BINDINGS_HH

#include <mmtf.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <sstream>

namespace py = pybind11;


template< typename T >
py::array
array1d_from_vector(std::vector<T> & m) {
	if (m.empty()) return py::array_t<T>();
	std::vector<T>* ptr = new std::vector<T>(std::move(m));
	auto capsule = py::capsule(ptr, [](void* p) { delete reinterpret_cast<std::vector<T>*>(p); });
	return py::array_t<T>(ptr->size(),  // shape of array
												ptr->data(),  // c-style contiguous strides for Sequence
												capsule       // numpy array references this parent
	);
}


template< typename T >
py::array
array2d_from_vector(std::vector<std::vector<T>> & m) {
	if (m.empty()) return py::array_t<T>();
	std::vector<std::vector<T>>* ptr = new std::vector<std::vector<T>>(std::move(m));
	auto capsule = py::capsule(ptr, [](void* p) { delete reinterpret_cast<std::vector<std::vector<T>>*>(p); });
	return py::array_t<T>({ptr->size(), ptr->at(0).size()},  // shape of array
												{ptr->size()*ptr->at(0).size()*sizeof(T)},  // c-style contiguous strides for Sequence
												capsule       // numpy array references this parent
	);
}

py::bytes
raw_properties(mmtf::StructureData const & sd) {
	std::stringstream bytes;
	std::map< std::string, std::map< std::string, msgpack::object > > objs({
			{"bondProperties", sd.bondProperties },
			{"atomProperties", sd.atomProperties },
			{"groupProperties", sd.groupProperties },
			{"chainProperties", sd.chainProperties },
			{"modelProperties", sd.modelProperties },
			{"extraProperties", sd.extraProperties }});
	msgpack::pack(bytes, objs);
	return py::bytes(bytes.str().data());
}

py::dict
dump_transform(mmtf::Transform & trans) {
	py::dict d;
	d["chainIndexList"] = array1d_from_vector(trans.chainIndexList);
	std::vector<float> matrix(std::begin(trans.matrix), std::end(trans.matrix));
	d["matrix"] = array1d_from_vector(matrix);
	return d;
}



py::dict
dump_bio_assembly(mmtf::BioAssembly & ba) {
	py::dict d;
	py::list l;
	for (mmtf::Transform & trans : ba.transformList) {
		l.append(dump_transform(trans));
	}
	d["transformList"] = l;
	d["name"] = py::str(ba.name);
	return d;
}


py::list
dump_bio_assembly_list(mmtf::StructureData & sd) {
	py::list bal;
	for (mmtf::BioAssembly & cba : sd.bioAssemblyList) {
		bal.append(dump_bio_assembly(cba));
	}
	return bal;
}


py::dict
dump_group(mmtf::GroupType & gt) {
	py::dict d;
	d["formalChargeList"] = array1d_from_vector(gt.formalChargeList);
	py::list anl;
	for (std::string & s: gt.atomNameList) anl.append(s);
	d["atomNameList"] = anl;
	py::list el;
	for (std::string & s: gt.atomNameList) el.append(s);
	d["elementList"] = el;
	d["bondAtomList"] = array1d_from_vector(gt.bondAtomList);
	d["bondOrderList"] = array1d_from_vector(gt.bondOrderList);
	d["bondResonanceList"] = array1d_from_vector(gt.bondResonanceList);
	d["groupName"] = gt.groupName;
	d["singleLetterCode"] = std::string(1, gt.singleLetterCode);
	d["chemCompType"] = gt.chemCompType;
	return d;
}


py::list
dump_group_list(std::vector<mmtf::GroupType> & gtl) {
	py::list gl;
	for (mmtf::GroupType & gt : gtl) {
		gl.append(dump_group(gt));
	}
	return gl;
}


py::list
dump_entity_list(std::vector<mmtf::Entity> & cpp_el) {
	py::list el;
	for (mmtf::Entity & e : cpp_el) {
		py::dict d;
		d["chainIndexList"] = array1d_from_vector(e.chainIndexList);
		d["description"] = e.description;
		d["type"] = e.type;
		d["sequence"] = e.sequence;
		el.append(d);
	}
	return el;
}


PYBIND11_MODULE(example, m) {
	// new stuff here
	py::class_<mmtf::StructureData>(m, "CPPStructureData")
		.def( pybind11::init( [](){ return new mmtf::StructureData(); } ) )
		.def( pybind11::init( [](mmtf::StructureData const &o){ return new mmtf::StructureData(o); } ) )
		.def_readwrite("mmtfVersion", &mmtf::StructureData::mmtfVersion)
		.def_readwrite("mmtfProducer", &mmtf::StructureData::mmtfProducer)
		.def("unitCell", [](mmtf::StructureData &m){return array1d_from_vector(m.unitCell);})
		.def_readwrite("spaceGroup", &mmtf::StructureData::spaceGroup)
		.def_readwrite("structureId", &mmtf::StructureData::structureId)
		.def_readwrite("title", &mmtf::StructureData::title)
		.def_readwrite("depositionDate", &mmtf::StructureData::depositionDate)
		.def_readwrite("releaseDate", &mmtf::StructureData::releaseDate)
		.def("ncsOperatorList", [](mmtf::StructureData &m){return array2d_from_vector(m.ncsOperatorList);})
		.def("bioAssemblyList", [](mmtf::StructureData &m){return dump_bio_assembly_list(m);})
		.def("entityList", [](mmtf::StructureData &m){return dump_entity_list(m.entityList);})
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
		.def("unitCell", [](mmtf::StructureData &m){return array1d_from_vector(m.unitCell);})
		.def("bondAtomList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondAtomList);})
		.def("bondOrderList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondOrderList);})
		.def("bondResonanceList", [](mmtf::StructureData &m){return array1d_from_vector(m.bondResonanceList);})
		.def("xCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.xCoordList);})
		.def("yCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.yCoordList);})
		.def("zCoordList", [](mmtf::StructureData &m){return array1d_from_vector(m.zCoordList);})
		.def("bFactorList", [](mmtf::StructureData &m){return array1d_from_vector(m.bFactorList);})
		.def("atomIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.atomIdList);})
		.def_readwrite("altLocList", &mmtf::StructureData::altLocList)
		.def("occupancyList", [](mmtf::StructureData &m){return array1d_from_vector(m.occupancyList);})
		.def("groupIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupIdList);})
		.def("groupTypeList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupTypeList);})
		.def("secStructList", [](mmtf::StructureData &m){return array1d_from_vector(m.secStructList);})
		.def_readwrite("insCodeList", &mmtf::StructureData::insCodeList)
		.def("sequenceIndexList", [](mmtf::StructureData &m){return array1d_from_vector(m.sequenceIndexList);})
		.def_readwrite("chainIdList", &mmtf::StructureData::chainIdList)
		.def_readwrite("chainNameList", &mmtf::StructureData::chainNameList)
		.def("groupsPerChain", [](mmtf::StructureData &m){return array1d_from_vector(m.groupsPerChain);})
		.def("chainsPerModel", [](mmtf::StructureData &m){return array1d_from_vector(m.chainsPerModel);})
		.def("raw_properties", [](mmtf::StructureData const &m){return raw_properties(m);});
	m.def("decodeFromFile", &mmtf::decodeFromFile, "decode a mmtf::StructureData from a file");

	py::class_<mmtf::BioAssembly>(m, "CPPBioAssembly")
		.def( pybind11::init( [](){ return new mmtf::BioAssembly(); } ) )
		.def( pybind11::init( [](mmtf::BioAssembly const &o){ return new mmtf::BioAssembly(o); } ) );
		// TODO ^^ insert transformlist
	py::class_<mmtf::Transform>(m, "CPPTransform")
		.def( pybind11::init( [](){ return new mmtf::Transform(); } ) )
		.def( pybind11::init( [](mmtf::Transform const &o){ return new mmtf::Transform(o); } ) );

		/// TODO finish ^^
	py::class_<mmtf::GroupType>(m, "CPPGroupType")
		.def( pybind11::init( [](){ return new mmtf::GroupType(); } ) )
		.def( pybind11::init( [](mmtf::GroupType const &o){ return new mmtf::GroupType(o); } ) );
		/// TODO finish ^^
	py::class_<mmtf::Entity>(m, "CPPEntity")
		.def( pybind11::init( [](){ return new mmtf::Entity(); } ) )
		.def( pybind11::init( [](mmtf::Entity const &o){ return new mmtf::Entity(o); } ) );
}


//#endif
