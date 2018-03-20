#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <mmtf.hpp>


// we never use anything less than 0.xxx, this is more than enough
// precision for the PDB. Perhaps more investigation will be necessary
// in the future.
template <typename T>
bool almost_equal(const T& a, const T& b) {
	return std::abs(a - b) < 0.00001;
}


template <typename T>
bool almost_equal_vector(const T& a, const T& b) {
	for (std::size_t i=0; i<=a.size(); ++i) {
		if (!almost_equal(a[i], b[i])) return false;
	}
	return true;
}


// Tests for structure_data.hpp
TEST_CASE("Test for equality of StructureData") {
	mmtf::StructureData sd1, sd2;
	REQUIRE( sd1 == sd2 );
}

TEST_CASE("Test DeltaRecursiveFloat enc/dec") {
	std::vector<char> encoded_data;
	// h1
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x0a);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	encoded_data.push_back(0xe8);
	// data
	encoded_data.push_back(0x7f);
	encoded_data.push_back(0xff);
	encoded_data.push_back(0x44);
	encoded_data.push_back(0xab);
	encoded_data.push_back(0x01);
	encoded_data.push_back(0x8f);
	encoded_data.push_back(0xff);
	encoded_data.push_back(0xca);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<float> decoded_input;
	bd.decode(decoded_input);

	std::vector<float> decoded_data;
	decoded_data.push_back(50.346f);
	decoded_data.push_back(50.745f);
	decoded_data.push_back(50.691f);

	std::vector<char> encoded_output = mmtf::encodeDeltaRecursiveFloat(decoded_data, 1000);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(almost_equal_vector(decoded_data, decoded_input));
}
