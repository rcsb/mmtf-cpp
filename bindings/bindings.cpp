
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


// This destroys the original data
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


// This destroys the original data
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


py::list
dump_bio_assembly_list(mmtf::StructureData & sd) {
	py::object py_ba_class = py::module::import("mmtf_t").attr("BioAssembly");
	py::object py_t_class = py::module::import("mmtf_t").attr("Transform");
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


std::vector<mmtf::Transform>
make_transformList(py::list & l) {
	std::vector<mmtf::Transform> tl;
	for (auto & trans : l) {
		mmtf::Transform t;
		t.chainIndexList = trans.attr("chainIndexList").cast<std::vector<int>>();
		py::list pymatrix(trans.attr("matrix"));
		std::size_t count(0);
		for (auto & x : pymatrix) {
			t.matrix[count] = x.cast<float>();
			++count;
		}
		tl.push_back(t);
	}
	return tl;
}


void
set_bioAssemblyList(py::list & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::BioAssembly> bioAs;
	for (auto & py_bioAssembly : obj ) {
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
set_entityList(py::list & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::Entity> entities;
	for (auto & py_entity : obj ) {
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
set_groupList(py::list & obj, mmtf::StructureData & sd) {
	std::vector<mmtf::GroupType> groups;
	for (auto & py_group : obj ) {
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


py::list
dump_group_list(std::vector<mmtf::GroupType> & gtl) {
	py::object py_gt_class = py::module::import("mmtf_t").attr("GroupType");
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


py::list
dump_entity_list(std::vector<mmtf::Entity> & cpp_el) {
	py::object entity = py::module::import("mmtf_t").attr("Entity");
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


PYBIND11_MODULE(example, m) {
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
		.def("ncsOperatorList", [](mmtf::StructureData &m){return array2d_from_vector(m.ncsOperatorList);})
		.def_readwrite("ncsOperatorList_io", &mmtf::StructureData::ncsOperatorList)
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
		.def_readwrite("altLocList", &mmtf::StructureData::altLocList)
		.def("occupancyList", [](mmtf::StructureData &m){return array1d_from_vector(m.occupancyList);})
		.def_readwrite("occupancyList_io", &mmtf::StructureData::occupancyList)
		.def("groupIdList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupIdList);})
		.def_readwrite("groupIdList_io", &mmtf::StructureData::groupIdList)
		.def("groupTypeList", [](mmtf::StructureData &m){return array1d_from_vector(m.groupTypeList);})
		.def_readwrite("groupTypeList_io", &mmtf::StructureData::groupTypeList)
		.def("secStructList", [](mmtf::StructureData &m){return array1d_from_vector(m.secStructList);})
		.def_readwrite("secStructList_io", &mmtf::StructureData::secStructList)
		.def_readwrite("insCodeList", &mmtf::StructureData::insCodeList)
		.def("sequenceIndexList", [](mmtf::StructureData &m){return array1d_from_vector(m.sequenceIndexList);})
		.def_readwrite("sequenceIndexList_io", &mmtf::StructureData::sequenceIndexList)
		.def_readwrite("chainIdList", &mmtf::StructureData::chainIdList)
		.def_readwrite("chainNameList", &mmtf::StructureData::chainNameList)
		.def("groupsPerChain", [](mmtf::StructureData &m){return array1d_from_vector(m.groupsPerChain);})
		.def_readwrite("groupsPerChain_io", &mmtf::StructureData::groupsPerChain)
		.def("chainsPerModel", [](mmtf::StructureData &m){return array1d_from_vector(m.chainsPerModel);})
		.def_readwrite("chainsPerModel_io", &mmtf::StructureData::chainsPerModel)
		.def("raw_properties", [](mmtf::StructureData const &m){return raw_properties(m);});
 
	m.def("set_bioAssemblyList", [](py::list &i, mmtf::StructureData & sd){return set_bioAssemblyList(i, sd);});
	m.def("set_entityList", [](py::list &i, mmtf::StructureData & sd){return set_entityList(i, sd);});
	m.def("set_groupList", [](py::list &i, mmtf::StructureData & sd){return set_groupList(i, sd);});
	m.def("decodeFromFile", &mmtf::decodeFromFile, "decode a mmtf::StructureData from a file");
	m.def("decodeFromBuffer", &mmtf::decodeFromBuffer, "decode a mmtf::StructureData from bytes");
	m.def("encodeToFile", [](mmtf::StructureData const &m, std::string const & fn){mmtf::encodeToFile(m, fn);});
	m.def("encodeToStream", [](mmtf::StructureData const &m){
			std::stringstream ss;
			mmtf::encodeToStream(m, ss);
			return py::bytes(ss.str());
			});
}

//#endif
