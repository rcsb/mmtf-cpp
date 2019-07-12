
#ifndef _INCLUDED_custom_zone_hpp
#define _INCLUDED_custom_zone_hpp

#include <pybind11/stl_bind.h>

void
msgpack_sd_binder(pybind11::handle &M)
{ // mmtf::StructureData file:mmtf/structure_data.hpp line:157
	pybind11::class_<mmtf::StructureData, std::shared_ptr<mmtf::StructureData>> cl(M, "StructureData", "Top level MMTF data container.\n\n Default values (mmtf::isDefaultValue, mmtf::setDefaultValue) are set in\n constructor and can be used to check if value was never set (only relevant\n for optional values):\n - default for vectors and strings: empty\n - default for numeric types (incl. char): max. value of that type\n - default for numXX = 0\n\n https://github.com/rcsb/mmtf/blob/HEAD/spec.md#fields");
	pybind11::handle cl_type = cl;

	cl.def( pybind11::init( [](){ return new mmtf::StructureData(); } ) );
	cl.def( pybind11::init( [](mmtf::StructureData const &o){ return new mmtf::StructureData(o); } ) );
	cl.def_readwrite("mmtfVersion", &mmtf::StructureData::mmtfVersion);
	cl.def_readwrite("mmtfProducer", &mmtf::StructureData::mmtfProducer);
	cl.def_readwrite("unitCell", &mmtf::StructureData::unitCell);
	cl.def_readwrite("spaceGroup", &mmtf::StructureData::spaceGroup);
	cl.def_readwrite("structureId", &mmtf::StructureData::structureId);
	cl.def_readwrite("title", &mmtf::StructureData::title);
	cl.def_readwrite("depositionDate", &mmtf::StructureData::depositionDate);
	cl.def_readwrite("releaseDate", &mmtf::StructureData::releaseDate);
	cl.def_readwrite("ncsOperatorList", &mmtf::StructureData::ncsOperatorList);
	cl.def_readwrite("bioAssemblyList", &mmtf::StructureData::bioAssemblyList);
	cl.def_readwrite("entityList", &mmtf::StructureData::entityList);
	cl.def_readwrite("experimentalMethods", &mmtf::StructureData::experimentalMethods);
	cl.def_readwrite("resolution", &mmtf::StructureData::resolution);
	cl.def_readwrite("rFree", &mmtf::StructureData::rFree);
	cl.def_readwrite("rWork", &mmtf::StructureData::rWork);
	cl.def_readwrite("numBonds", &mmtf::StructureData::numBonds);
	cl.def_readwrite("numAtoms", &mmtf::StructureData::numAtoms);
	cl.def_readwrite("numGroups", &mmtf::StructureData::numGroups);
	cl.def_readwrite("numChains", &mmtf::StructureData::numChains);
	cl.def_readwrite("numModels", &mmtf::StructureData::numModels);
	cl.def_readwrite("groupList", &mmtf::StructureData::groupList);
	cl.def_readwrite("bondAtomList", &mmtf::StructureData::bondAtomList);
	cl.def_readwrite("bondOrderList", &mmtf::StructureData::bondOrderList);
	cl.def_readwrite("bondResonanceList", &mmtf::StructureData::bondResonanceList);
	cl.def_readwrite("xCoordList", &mmtf::StructureData::xCoordList);
	cl.def_readwrite("yCoordList", &mmtf::StructureData::yCoordList);
	cl.def_readwrite("zCoordList", &mmtf::StructureData::zCoordList);
	cl.def_readwrite("bFactorList", &mmtf::StructureData::bFactorList);
	cl.def_readwrite("atomIdList", &mmtf::StructureData::atomIdList);
	cl.def_readwrite("altLocList", &mmtf::StructureData::altLocList);
	cl.def_readwrite("occupancyList", &mmtf::StructureData::occupancyList);
	cl.def_readwrite("groupIdList", &mmtf::StructureData::groupIdList);
	cl.def_readwrite("groupTypeList", &mmtf::StructureData::groupTypeList);
	cl.def_readwrite("secStructList", &mmtf::StructureData::secStructList);
	cl.def_readwrite("insCodeList", &mmtf::StructureData::insCodeList);
	cl.def_readwrite("sequenceIndexList", &mmtf::StructureData::sequenceIndexList);
	cl.def_readwrite("chainIdList", &mmtf::StructureData::chainIdList);
	cl.def_readwrite("chainNameList", &mmtf::StructureData::chainNameList);
	cl.def_readwrite("groupsPerChain", &mmtf::StructureData::groupsPerChain);
	cl.def_readwrite("chainsPerModel", &mmtf::StructureData::chainsPerModel);
	cl.def_readonly("msgpack_zone", &mmtf::StructureData::msgpack_zone);
	cl.def_readwrite("bondProperties", &mmtf::StructureData::bondProperties);
	cl.def_readwrite("atomProperties", &mmtf::StructureData::atomProperties);
	cl.def_readwrite("groupProperties", &mmtf::StructureData::groupProperties);
	cl.def_readwrite("chainProperties", &mmtf::StructureData::chainProperties);
	cl.def_readwrite("modelProperties", &mmtf::StructureData::modelProperties);
	cl.def_readwrite("extraProperties", &mmtf::StructureData::extraProperties);
	cl.def("assign", (struct mmtf::StructureData & (mmtf::StructureData::*)(const struct mmtf::StructureData &)) &mmtf::StructureData::operator=, "Overload for assignment operator.\n\nC++: mmtf::StructureData::operator=(const struct mmtf::StructureData &) --> struct mmtf::StructureData &", pybind11::return_value_policy::automatic, pybind11::arg("f"));
	cl.def("hasConsistentData", [](mmtf::StructureData const &o) -> bool { return o.hasConsistentData(); }, "");
	cl.def("hasConsistentData", [](mmtf::StructureData const &o, bool const & a0) -> bool { return o.hasConsistentData(a0); }, "", pybind11::arg("verbose"));
	cl.def("hasConsistentData", (bool (mmtf::StructureData::*)(bool, unsigned int) const) &mmtf::StructureData::hasConsistentData, "Check consistency of structural data.\n \n\n                 Print first error encountered (if any)\n \n\n   Max allowed chain name length\n \n\n True if all required fields are set and vector sizes and indices\n         are consistent.\n\nC++: mmtf::StructureData::hasConsistentData(bool, unsigned int) const --> bool", pybind11::arg("verbose"), pybind11::arg("chain_name_max_length"));
	cl.def("print", [](mmtf::StructureData &o) -> std::string { return o.print(); }, "");
	cl.def("print", (std::string (mmtf::StructureData::*)(std::string)) &mmtf::StructureData::print, "Read out the contents of mmtf::StructureData in a PDB-like fashion\n Columns are in order:\n ATOM/HETATM AtomId Element AtomName AltLoc GroupId GroupType\n InsCode ChainName x y z B-factor Occupancy Charge\n \n\n what to split columns with\n\nC++: mmtf::StructureData::print(std::string) --> std::string", pybind11::arg("delim"));
	cl.def("__eq__", (bool (mmtf::StructureData::*)(const struct mmtf::StructureData &) const) &mmtf::StructureData::operator==, "Compare two StructureData classes for equality\n \n\n What to compare to\n\nC++: mmtf::StructureData::operator==(const struct mmtf::StructureData &) const --> bool", pybind11::arg("c"));
	cl.def("__ne__", (bool (mmtf::StructureData::*)(const struct mmtf::StructureData &) const) &mmtf::StructureData::operator!=, "Compare two StructureData classes for inequality\n \n\n What to compare to\n\nC++: mmtf::StructureData::operator!=(const struct mmtf::StructureData &) const --> bool", pybind11::arg("c"));

	// templated if you need more -- add more here!
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const int &)) &mmtf::StructureData::add_prop<int>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const int &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::string &)) &mmtf::StructureData::add_prop<std::string>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::string &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const float &)) &mmtf::StructureData::add_prop<float>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const float &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const double &)) &mmtf::StructureData::add_prop<double>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const double &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::vector<char> &)) &mmtf::StructureData::add_prop<std::vector<char>>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::vector<char> &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::vector<int> &)) &mmtf::StructureData::add_prop<std::vector<int>>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::vector<int> &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::vector<std::vector<int>> &)) &mmtf::StructureData::add_prop<std::vector<std::vector<int>>>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::vector<std::vector<int>> &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::vector<float> &)) &mmtf::StructureData::add_prop<std::vector<float>>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::vector<float> &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
	cl.def("add_prop", (void (mmtf::StructureData::*)(const std::string &, const std::string &, const std::vector<std::vector<float>> &)) &mmtf::StructureData::add_prop<std::vector<std::vector<float>>>, "C++: mmtf::StructureData::add_prop(const std::string &, const std::string &, const std::vector<std::vector<float>> &) --> void", pybind11::arg("prefix"), pybind11::arg("key"), pybind11::arg("val"));
}


#endif // _INCLUDED_std_binders_hpp_
