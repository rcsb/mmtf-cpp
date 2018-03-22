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

TEST_CASE("Test RunLengthFloat enc/dec") {
	std::vector<char> encoded_data;
	// h1
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x09);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x64);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x64);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<float> decoded_input;
	bd.decode(decoded_input);

	std::vector<float> decoded_data;
	decoded_data.push_back(1.00f);
	decoded_data.push_back(1.00f);
	decoded_data.push_back(1.00f);

	std::vector<char> encoded_output = mmtf::encodeRunLengthFloat(decoded_data, 100);

	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(almost_equal_vector(decoded_data, decoded_input));
}

TEST_CASE("Test RunLengthDeltaInt enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x08);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x07);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x01);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x07);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int32_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int32_t> decoded_data;
	decoded_data.push_back(1);
	decoded_data.push_back(2);
	decoded_data.push_back(3);
	decoded_data.push_back(4);
	decoded_data.push_back(5);
	decoded_data.push_back(6);
	decoded_data.push_back(7);

	std::vector<char> encoded_output = mmtf::encodeRunLengthDeltaInt(decoded_data);

	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test RunLengthChar enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x06);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x41);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<char> decoded_input;
	bd.decode(decoded_input);

	std::vector<char> decoded_data;
	decoded_data.push_back('A');
	decoded_data.push_back('A');
	decoded_data.push_back('A');
	decoded_data.push_back('A');

	std::vector<char> encoded_output = mmtf::encodeRunLengthChar(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}

    //def test_enc_str(self):
    //    test_data = b'B\x00\x00\x00A\x00\x00\x00C\x00\x00\x00A\x00\x00\x00A\x00\x00\x00A\x00\x00\x00'
    //    output_data =  numpy.array(["B","A","C","A","A","A"])
    //    run_all(self, test_data, output_data, 0, 5)


TEST_CASE("Test encodeStringVector enc/dec") {
//inline std::vector<char> encodeStringVector(std::vector<std::string> in_sv, int32_t CHAIN_LEN) {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x05);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x06);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// data
	encoded_data.push_back('B');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('C');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<std::string> decoded_input;
	bd.decode(decoded_input);

	std::vector<std::string> decoded_data;
	decoded_data.push_back("B");
	decoded_data.push_back("A");
	decoded_data.push_back("C");
	decoded_data.push_back("A");
	decoded_data.push_back("A");
	decoded_data.push_back("A");

	std::vector<char> encoded_output = mmtf::encodeStringVector(decoded_data, 4);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}
