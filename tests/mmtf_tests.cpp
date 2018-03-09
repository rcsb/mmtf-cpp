#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <mmtf.hpp>

// Tests for structure_data.hpp
TEST_CASE("Test for equality of StructureData") {
	mmtf::StructureData sd1, sd2;
	REQUIRE( sd1 == sd2 );
}

TEST_CASE("Test DeltaRecursiveFloat enc/dec") {
	//const char* encoded_data = '\x7f\xffD\xab\x01\x8f\xff\xca';
	char encoded_data[] = "\x7f\xffD\xab\x01\x8f\xff\xca";
	//const char* encoded_data = u"\x007f\x0ffD\x00ab\x0001\x008f\x00ff\x00ca";
	//const char* encoded_data = "\u7f\uffD\uab\u01\u8f\uff\uca";
	//const char* encoded_data = "0x7f0xffD0xab0x010x8f0xff0xca";
	std::vector<float> decoded_data;
	decoded_data.push_back(50.346);
	decoded_data.push_back(50.745);
	decoded_data.push_back(50.691);
	std::cout << "encoded: " << std::string(encoded_data) << std::endl;
//std::cout << "decoded: " << mmtf::encodeDeltaRecursiveFloat(decoded_data, 1000) << std::endl;
	std::vector<char> encode_output = mmtf::encodeDeltaRecursiveFloat(decoded_data, 1000);
	std::string encode_output_str(encode_output.begin(),encode_output.end());
	std::cout << "encoded: " << encode_output_str << std::endl;
	REQUIRE(std::string(encoded_data) == encode_output_str);
}
