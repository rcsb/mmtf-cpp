#ifdef __EMSCRIPTEN__
#define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_RUNNER
#else
#define CATCH_CONFIG_MAIN
#endif

#include "catch.hpp"

#include <mmtf.hpp>


// NOTE!!! Margin is set to 0.00001
// If we ever get more specific data this may need to be altered!
template <typename T>
bool approx_equal_vector(const T& a, const T& b, float eps = 0.00001) {
	if (a.size() != b.size()) return false;
	for (std::size_t i=0; i < a.size(); ++i) {
		if (a[i] != Approx(b[i]).margin(eps)) return false;
	}
	return true;
}


// Tests for structure_data.hpp
TEST_CASE("Test round trip StructureData working") {
	std::vector<std::string> works;
	works.push_back("../mmtf_spec/test-suite/mmtf/173D.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1AA6.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1AUY.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1BNA.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1CAG.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1HTQ.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1IGT.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1L2Q.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1LPV.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1MSH.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1O2F.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1R9V.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1SKM.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/3NJW.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/3ZYB.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4CK4.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4CUP.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4OPJ.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4P3R.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4V5A.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4Y60.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/5EMG.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/5ESW.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-all0.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-numChains1.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-numModels1.mmtf");
	for (size_t i=0; i<works.size(); ++i) {
		mmtf::StructureData sd1, sd2;
		mmtf::decodeFromFile(sd1, works[i]);
		mmtf::encodeToFile(sd1, "test_mmtf.mmtf");
		mmtf::decodeFromFile(sd2, "test_mmtf.mmtf");
		REQUIRE( sd1 == sd2 );
	}
}


TEST_CASE("Test round trip StructureData not working - EncodeError") {
	std::string basic = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, basic);
	SECTION("Alter xCoordList") {
		sd.xCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter yCoordList") {
		sd.yCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter zCoordList") {
		sd.zCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter bFactorList") {
		sd.bFactorList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter numAtoms") {
		sd.numAtoms = 20;
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter chainIdList") {
		sd.chainIdList.push_back("xsz");
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("chainIdList one too large") {
		sd.chainIdList[0] = "IheartMMTF";
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter single groupList entry") {
		sd.groupList[0].formalChargeList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
		sd.groupList[0].atomNameList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
		sd.groupList[0].elementList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter groupTypeList") {
		sd.groupTypeList[0] = 100;
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}

}


TEST_CASE("Test StructureData DecodeError") {
	std::vector<std::string> doesnt_work;
	doesnt_work.push_back("../mmtf_spec/test-suite/mmtf/empty-mmtfVersion99999999.mmtf");
	for (size_t i=0; i<doesnt_work.size(); ++i) {
		mmtf::StructureData sd;
		REQUIRE_THROWS_AS(mmtf::decodeFromFile(sd, doesnt_work[i]), mmtf::DecodeError);
	}
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
	REQUIRE(approx_equal_vector(decoded_data, decoded_input));
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
	REQUIRE(approx_equal_vector(decoded_data, decoded_input));
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


TEST_CASE("Test encodeStringVector enc/dec") {
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

	int chain_name_max_length = 4;
	std::vector<char> encoded_output = mmtf::encodeStringVector(decoded_data, chain_name_max_length);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test int 8 enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x05);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x07);
	encoded_data.push_back(0x06);
	encoded_data.push_back(0x06);
	encoded_data.push_back(0x07);
	encoded_data.push_back(0x07);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int8_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int8_t> decoded_data;
	decoded_data.push_back(7);
	decoded_data.push_back(6);
	decoded_data.push_back(6);
	decoded_data.push_back(7);
	decoded_data.push_back(7);

	std::vector<char> encoded_output = mmtf::encodeInt8ToByte(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test FourByteInt enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
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
	encoded_data.push_back(0x01);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x01);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int32_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int32_t> decoded_data;
	decoded_data.push_back(1);
	decoded_data.push_back(131073);
	decoded_data.push_back(0);
	decoded_data.push_back(2);

	std::vector<char> encoded_output = mmtf::encodeFourByteInt(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}

TEST_CASE("Test bondOrderList vs bondAtomList") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	SECTION("Deleting all bondOrderLists") {
		for (auto & group : sd.groupList) {
			group.bondOrderList.clear();
		}
		sd.bondOrderList.clear();
		REQUIRE(sd.hasConsistentData(true) == true);
	}
	SECTION("altering group bondOrderLists") {
		sd.groupList[0].bondOrderList.push_back(1);
		REQUIRE(sd.hasConsistentData(true) == false);
	}
	SECTION("altering sd bondOrderLists") {
		sd.bondOrderList.push_back(1);
		REQUIRE(sd.hasConsistentData(true) == false);
	}
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

int main(int argc, char* argv[]) {
    // Give node.js an access to the root filesystem
    EM_ASM(
        FS.mkdir('root');
        FS.mount(NODEFS, { root: '/' }, 'root');
        FS.chdir('root/' + process.cwd() + '/../');
    );

    return Catch::Session().run(argc, argv);
}
#endif
