
from structure_data import StructureData
import os
import mmtf_cpp
from mmtf_cpp import CPPStructureData, decodeFromFile
import time
import pytest
import numpy as np


def test_eq_operator(test_data_dir):
    s1 = StructureData(os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/173D.mmtf"))
    s2 = StructureData(os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/173D.mmtf"))
    s3 = StructureData(os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/1AUY.mmtf"))
    assert s1 == s2
    assert s1 != s3


def test_roundtrip(test_data_dir):
    files = [
        "mmtf_spec/test-suite/mmtf/173D.mmtf",
        "mmtf_spec/test-suite/mmtf/1AA6.mmtf",
        "mmtf_spec/test-suite/mmtf/1AUY.mmtf",
        "mmtf_spec/test-suite/mmtf/1BNA.mmtf",
        "mmtf_spec/test-suite/mmtf/1CAG.mmtf",
        "mmtf_spec/test-suite/mmtf/1HTQ.mmtf",
        "mmtf_spec/test-suite/mmtf/1IGT.mmtf",
        "mmtf_spec/test-suite/mmtf/1L2Q.mmtf",
        "mmtf_spec/test-suite/mmtf/1LPV.mmtf",
        "mmtf_spec/test-suite/mmtf/1MSH.mmtf",
        "mmtf_spec/test-suite/mmtf/1O2F.mmtf",
        "mmtf_spec/test-suite/mmtf/1R9V.mmtf",
        "mmtf_spec/test-suite/mmtf/1SKM.mmtf",
        "mmtf_spec/test-suite/mmtf/3NJW.mmtf",
        "mmtf_spec/test-suite/mmtf/3ZYB.mmtf",
        "mmtf_spec/test-suite/mmtf/4CK4.mmtf",
        "mmtf_spec/test-suite/mmtf/4CUP.mmtf",
        "mmtf_spec/test-suite/mmtf/4OPJ.mmtf",
        "mmtf_spec/test-suite/mmtf/4P3R.mmtf",
        "mmtf_spec/test-suite/mmtf/4V5A.mmtf",
        "mmtf_spec/test-suite/mmtf/4Y60.mmtf",
        "mmtf_spec/test-suite/mmtf/5EMG.mmtf",
        "mmtf_spec/test-suite/mmtf/5ESW.mmtf",
        "mmtf_spec/test-suite/mmtf/empty-all0.mmtf",
        "mmtf_spec/test-suite/mmtf/empty-numChains1.mmtf",
        "mmtf_spec/test-suite/mmtf/empty-numModels1.mmtf",
        "temporary_test_data/all_canoncial.mmtf",
        "temporary_test_data/1PEF_with_resonance.mmtf",
    ]
    test_tmp_mmtf_filename = "test_mmtf.mmtf"
    for filename in files:
        s1 = StructureData(os.path.join(test_data_dir, filename))
        s1.write_to_file(test_tmp_mmtf_filename)
        s2 = StructureData(test_tmp_mmtf_filename)
        s1.check_equals(s2)
        assert s1 == s2


def test_bad_mmtf(test_data_dir):
    doesnt_work = [
    "mmtf_spec/test-suite/mmtf/empty-mmtfVersion99999999.mmtf"
    ]
    for filename in doesnt_work:
        with pytest.raises(RuntimeError) as einfo:
            s1 = StructureData(os.path.join(test_data_dir, filename))


def test_various_throws(test_data_dir):
    working_mmtf_fn = os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/173D.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.xCoordList = np.append(sd.xCoordList, 0.334)
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.yCoordList = np.append(sd.yCoordList, 0.334)
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.zCoordList = np.append(sd.zCoordList, 0.334)
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.bFactorList = np.append(sd.bFactorList, 0.334)
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.numAtoms = 20
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.chainIdList = np.append(sd.chainIdList, "xsz")
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.chainIdList = sd.chainIdList.astype("<U16")  # TODO should be default
    sd.chainIdList[0] = "IheartMMTF"
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.groupList[0].formalChargeList = sd.groupList[0].formalChargeList[:-1]
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.groupList[0].atomNameList = sd.groupList[0].atomNameList[:-1]
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.groupList[0].elementList = sd.groupList[0].elementList[:-1]
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")

    sd = StructureData(working_mmtf_fn)
    sd.groupTypeList[0] = 100
    with pytest.raises(RuntimeError) as einfo:
        sd.write_to_file("wrk.mmtf")


def test_DeltaRecursiveFloat(test_data_dir):
    encoded_data = b'\x00\x00\x00\x0a\x00\x00\x00\x03\x00\x00\x03\xe8\x7f\xffD\xab\x01\x8f\xff\xca'
    decoded_data = np.array([50.346, 50.745, 50.691])
    ret = mmtf_cpp.encodeDeltaRecursiveFloat(decoded_data, 1000)
    assert ret == encoded_data

##TEST_CASE("Test DeltaRecursiveFloat enc/dec") {
##  std::vector<char> encoded_data;
##  // h1
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x0a);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x03);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x03);
##  encoded_data.push_back(0xe8);
##  // data
##  encoded_data.push_back(0x7f);
##  encoded_data.push_back(0xff);
##  encoded_data.push_back(0x44);
##  encoded_data.push_back(0xab);
##  encoded_data.push_back(0x01);
##  encoded_data.push_back(0x8f);
##  encoded_data.push_back(0xff);
##  encoded_data.push_back(0xca);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<float> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<float> decoded_data;
##  decoded_data.push_back(50.346f);
##  decoded_data.push_back(50.745f);
##  decoded_data.push_back(50.691f);
##
##  std::vector<char> encoded_output
##    = mmtf::encodeDeltaRecursiveFloat(decoded_data, 1000);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(approx_equal_vector(decoded_data, decoded_input));
##}
##
##
##TEST_CASE("Test RunLengthFloat enc/dec") {
##  std::vector<char> encoded_data;
##  // h1
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x09);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x03);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x64);
##  // data
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x64);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x03);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<float> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<float> decoded_data;
##  decoded_data.push_back(1.00f);
##  decoded_data.push_back(1.00f);
##  decoded_data.push_back(1.00f);
##
##  std::vector<char> encoded_output = mmtf::encodeRunLengthFloat(decoded_data, 100);
##
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(approx_equal_vector(decoded_data, decoded_input));
##}
##
##
##TEST_CASE("Test RunLengthDeltaInt enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x08);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x07);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  // data
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x01);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x07);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<int32_t> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<int32_t> decoded_data;
##  decoded_data.push_back(1);
##  decoded_data.push_back(2);
##  decoded_data.push_back(3);
##  decoded_data.push_back(4);
##  decoded_data.push_back(5);
##  decoded_data.push_back(6);
##  decoded_data.push_back(7);
##
##  std::vector<char> encoded_output = mmtf::encodeRunLengthDeltaInt(decoded_data);
##
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##
##TEST_CASE("Test RunLengthChar enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x06);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  // data
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x41);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<char> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<char> decoded_data;
##  decoded_data.push_back('A');
##  decoded_data.push_back('A');
##  decoded_data.push_back('A');
##  decoded_data.push_back('A');
##
##  std::vector<char> encoded_output = mmtf::encodeRunLengthChar(decoded_data);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##
##TEST_CASE("Test RunLengthInt8 enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x10);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  // data
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x02);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<int8_t> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<int8_t> decoded_data;
##  decoded_data.push_back(2);
##  decoded_data.push_back(2);
##  decoded_data.push_back(2);
##  decoded_data.push_back(2);
##
##  std::vector<char> encoded_output = mmtf::encodeRunLengthInt8(decoded_data);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##
##TEST_CASE("Test encodeStringVector enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x05);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x06);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##  // data
##  encoded_data.push_back('B');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back('A');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back('C');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back('A');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back('A');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back('A');
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<std::string> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<std::string> decoded_data;
##  decoded_data.push_back("B");
##  decoded_data.push_back("A");
##  decoded_data.push_back("C");
##  decoded_data.push_back("A");
##  decoded_data.push_back("A");
##  decoded_data.push_back("A");
##
##  int chain_name_max_length = 4;
##  std::vector<char> encoded_output = mmtf::encodeStringVector(decoded_data, chain_name_max_length);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##
##TEST_CASE("Test int 8 enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x02);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x05);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  // data
##  encoded_data.push_back(0x07);
##  encoded_data.push_back(0x06);
##  encoded_data.push_back(0x06);
##  encoded_data.push_back(0x07);
##  encoded_data.push_back(0x07);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<int8_t> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<int8_t> decoded_data;
##  decoded_data.push_back(7);
##  decoded_data.push_back(6);
##  decoded_data.push_back(6);
##  decoded_data.push_back(7);
##  decoded_data.push_back(7);
##
##  std::vector<char> encoded_output = mmtf::encodeInt8ToByte(decoded_data);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##
##TEST_CASE("Test FourByteInt enc/dec") {
##  std::vector<char> encoded_data;
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##  // h2
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x04);
##  // h3
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  // data
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x01);
##
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x02);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x01);
##
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x00);
##  encoded_data.push_back(0x02);
##
##  msgpack::zone m_zone;
##  msgpack::object msgp_obj(encoded_data, m_zone);
##
##  mmtf::BinaryDecoder bd(msgp_obj, "a_test");
##  std::vector<int32_t> decoded_input;
##  bd.decode(decoded_input);
##
##  std::vector<int32_t> decoded_data;
##  decoded_data.push_back(1);
##  decoded_data.push_back(131073);
##  decoded_data.push_back(0);
##  decoded_data.push_back(2);
##
##  std::vector<char> encoded_output = mmtf::encodeFourByteInt(decoded_data);
##  REQUIRE(encoded_data == encoded_output);
##  REQUIRE(decoded_data.size() ==  decoded_input.size());
##  REQUIRE(decoded_data == decoded_input);
##}
##
##TEST_CASE("Test bondOrderList vs bondAtomList") {
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##  mmtf::StructureData sd;
##  mmtf::decodeFromFile(sd, working_mmtf);
##  SECTION("Deleting all bondOrderLists") {
##    for (auto & group : sd.groupList) {
##      group.bondOrderList.clear();
##    }
##    sd.bondOrderList.clear();
##    REQUIRE(sd.hasConsistentData());
##    // we write files without those fields (tested below), can we roundtrip?
##    mmtf::StructureData sd2;
##    mmtf::encodeToFile(sd, "test_mmtf_nobondorder.mmtf");
##    mmtf::decodeFromFile(sd2, "test_mmtf_nobondorder.mmtf");
##    REQUIRE( sd == sd2 );
##  }
##  SECTION("altering group bondOrderLists") {
##    sd.groupList[0].bondOrderList.push_back(1);
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("altering sd bondOrderLists") {
##    sd.bondOrderList.push_back(1);
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("not ok to have bond orders without atom list in group") {
##    sd.groupList[0].bondAtomList.clear();
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("not ok to have bond orders without atom list") {
##    sd.bondAtomList.clear();
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##}
##
##
##TEST_CASE("test valid bonds") {
##  std::string working_mmtf = "../temporary_test_data/all_canoncial.mmtf";
##  mmtf::StructureData sd;
##  mmtf::decodeFromFile(sd, working_mmtf);
##  REQUIRE(sd.hasConsistentData());
##
##  SECTION("invalid bond numbers in group") {
##    int8_t original = sd.groupList[0].bondResonanceList[0];
##    sd.groupList[0].bondResonanceList[0] = -3;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.groupList[0].bondResonanceList[0] = 2;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.groupList[0].bondResonanceList[0] = original;
##
##    original = sd.groupList[0].bondOrderList[0];
##    sd.groupList[0].bondOrderList[0] = 0;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.groupList[0].bondOrderList[0] = 5;
##    REQUIRE_FALSE(sd.hasConsistentData());
##
##    sd.groupList[0].bondOrderList[0] = -1;
##    sd.groupList[0].bondResonanceList[0] = -1;
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("invalid bond numbers") {
##    int8_t original = sd.bondResonanceList[0];
##    sd.bondResonanceList[0] = -3;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.bondResonanceList[0] = 2;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.bondResonanceList[0] = original;
##
##    original = sd.bondOrderList[0];
##    sd.bondOrderList[0] = 0;
##    REQUIRE_FALSE(sd.hasConsistentData());
##    sd.bondOrderList[0] = 5;
##    REQUIRE_FALSE(sd.hasConsistentData());
##
##    sd.bondOrderList[0] = -1;
##    sd.bondResonanceList[0] = -1;
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##
##  SECTION("invalid bondResonanceList size in group") {
##    sd.groupList[0].bondResonanceList.push_back(1);
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("invalid bondResonanceList size") {
##    sd.bondResonanceList.push_back(1);
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##
##  SECTION("not ok to have bond resonances without order list in group") {
##    sd.groupList[0].bondOrderList.clear();
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##  SECTION("not ok to have bond resonances without order list") {
##    sd.bondOrderList.clear();
##    REQUIRE_FALSE(sd.hasConsistentData());
##  }
##}
##
##
##TEST_CASE("test group export optional") {
##  std::string working_mmtf = "../temporary_test_data/all_canoncial.mmtf";
##  mmtf::StructureData sd;
##  mmtf::decodeFromFile(sd, working_mmtf);
##  REQUIRE(sd.hasConsistentData());
##
##  mmtf::GroupType & first_group(sd.groupList[0]);
##
##  SECTION("check that optional fields exist fist") {
##    msgpack::zone my_zone;
##    msgpack::object obj(first_group, my_zone);
##    std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
##    REQUIRE(my_map.find("bondOrderList") != my_map.end());
##    REQUIRE(my_map.find("bondResonanceList") != my_map.end());
##    REQUIRE(my_map.find("bondAtomList") != my_map.end());
##  }
##  SECTION("removing group bondOrderLists") {
##    msgpack::zone my_zone;
##    first_group.bondOrderList = std::vector<int8_t>();
##    msgpack::object obj(first_group, my_zone);
##    std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
##    REQUIRE(my_map.find("bondOrderList") == my_map.end());
##  }
##  SECTION("altering group bondResonanceLists") {
##    msgpack::zone my_zone;
##    first_group.bondResonanceList = std::vector<int8_t>();
##    msgpack::object obj(first_group, my_zone);
##    std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
##    REQUIRE(my_map.find("bondResonanceList") == my_map.end());
##  }
##  SECTION("altering group bondAtomLists") {
##    msgpack::zone my_zone;
##    first_group.bondAtomList = std::vector<int32_t>();
##    msgpack::object obj(first_group, my_zone);
##    std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
##    REQUIRE(my_map.find("bondAtomList") == my_map.end());
##  }
##}
##
##// Mainly a compiler check, not useful to actually test
##void
##map_const_sd_helper(mmtf::StructureData const & sd) {
##	std::map< std::string, std::string > map_str_str_out, map_str_str_in;
##	map_str_str_out["test"] = "tset";
##	const mmtf::MapDecoder extraProperties_MD(sd.extraProperties);
##	extraProperties_MD.decode("map_str_str", true, map_str_str_in);
##	REQUIRE(map_str_str_in == map_str_str_out);
##}
##
##TEST_CASE("mapDecoder types") {
##	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##	mmtf::StructureData sd;
##	mmtf::decodeFromFile(sd, working_mmtf);
##
##	std::map< std::string, std::string > map_str_str_out, map_str_str_in;
##	map_str_str_out["test"] = "tset";
##	sd.extraProperties["map_str_str"] = msgpack::object(map_str_str_out, sd.msgpack_zone);
##
##	// Also check to make sure const works
##	const mmtf::MapDecoder extraProperties_MD(sd.extraProperties);
##	extraProperties_MD.decode("map_str_str", true, map_str_str_in);
##	REQUIRE(map_str_str_in == map_str_str_out);
##	map_const_sd_helper(sd);
##}
##
##// test/example of how to use extra data
##TEST_CASE("atomProperties field") {
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##  mmtf::StructureData sd, sd2;
##  mmtf::decodeFromFile(sd, working_mmtf);
##
##  /// Pack
##  // 1. Make your input data
##  std::vector<int32_t> clist_out;
##  std::vector<int32_t> clist_in, clist_in_decoded, nothing;
##  for (int32_t i = 0; i < sd.numAtoms; ++i) {
##    clist_out.push_back(i % 256);
##  }
##  // 2. msgpack zones have weird lifetimes, make sure you use the zone
##  // in StructureData to avoid any weird errors
##  sd.atomProperties["256_atomColorList"]
##    = msgpack::object(clist_out, sd.msgpack_zone);
##  sd.atomProperties["256_atomColorList_encoded"]
##    = msgpack::object(mmtf::encodeRunLengthDeltaInt(clist_out), sd.msgpack_zone);
##
##  mmtf::encodeToFile(sd, "test_atomProperties.mmtf");
##  /// Done Pack
##
##  /// Start Unpack
##  mmtf::decodeFromFile(sd2, "test_atomProperties.mmtf");
##  // Retrieve our 256 color list via convert
##  mmtf::MapDecoder atomProperties_MD(sd2.atomProperties);
##  atomProperties_MD.decode("256_atomColorList", true, clist_in);
##  atomProperties_MD.decode("256_atomColorList_encoded", true, clist_in_decoded);
##
##  /// Done Unpack
##  REQUIRE(clist_out == clist_in);
##  REQUIRE(clist_in == clist_in_decoded);
##
##  // you would catch mmtf::DecodeError if you wanted continue even after using
##  // required=true.
##  REQUIRE_THROWS_AS(atomProperties_MD.decode("NONEXISTANT", true, nothing),
##                    mmtf::DecodeError);
##}
##
##// simple helper adding vector (numbers 0 to num_items-1) of given length both
##// encoded (key "data_encoded") and non-encoded (key "data") to destination_map
##void add_extra_data(std::map<std::string, msgpack::object>& destination_map,
##                    int32_t num_items, mmtf::StructureData& sd) {
##  std::vector<int32_t> v;
##  for (int32_t i = 0; i < num_items; ++i) {
##    v.push_back(i);
##  }
##  destination_map["data"] = msgpack::object(v, sd.msgpack_zone);
##  destination_map["data_encoded"]
##   = msgpack::object(mmtf::encodeRunLengthDeltaInt(v), sd.msgpack_zone);
##}
##// check data stored above
##void check_extra_data(const std::map<std::string, msgpack::object>& sd_map,
##                      int32_t num_items) {
##  std::vector<int32_t> v;
##  for (int32_t i = 0; i < num_items; ++i) {
##    v.push_back(i);
##  }
##  std::vector<int32_t> v_in, v_in_decoded, nothing;
##  mmtf::MapDecoder map_decoder(sd_map);
##  map_decoder.decode("data", true, v_in);
##  map_decoder.decode("data_encoded", true, v_in_decoded);
##  REQUIRE(v_in == v);
##  REQUIRE(v_in_decoded == v);
##  REQUIRE_THROWS_AS(map_decoder.decode("NONEXISTANT", true, nothing),
##                    mmtf::DecodeError);
##}
##
##// test round trips with all extra data set
##TEST_CASE("extra data fields") {
##  // Randomly chosen, small example MMTF
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##  mmtf::StructureData sd, sd2, sd3;
##  mmtf::decodeFromFile(sd, working_mmtf);
##
##  // Add extra data
##  add_extra_data(sd.bondProperties, sd.numBonds, sd);
##  add_extra_data(sd.atomProperties, sd.numAtoms, sd);
##  add_extra_data(sd.groupProperties, sd.numGroups, sd);
##  add_extra_data(sd.chainProperties, sd.numChains, sd);
##  add_extra_data(sd.modelProperties, sd.numModels, sd);
##  add_extra_data(sd.extraProperties, 42, sd);
##
##  // Encode/decode
##  mmtf::encodeToFile(sd, "test_extra_data.mmtf");
##  mmtf::decodeFromFile(sd2, "test_extra_data.mmtf");
##  REQUIRE(sd == sd2);
##  // Check round-trip after reading extra data
##  // (i.e. we can safely read/write unknown extra data)
##  mmtf::encodeToFile(sd2, "test_extra_data2.mmtf");
##  mmtf::decodeFromFile(sd3, "test_extra_data2.mmtf");
##  REQUIRE(sd2 == sd3);
##
##  // Check data
##  check_extra_data(sd.bondProperties, sd.numBonds);
##  check_extra_data(sd.atomProperties, sd.numAtoms);
##  check_extra_data(sd.groupProperties, sd.numGroups);
##  check_extra_data(sd.chainProperties, sd.numChains);
##  check_extra_data(sd.modelProperties, sd.numModels);
##  check_extra_data(sd.extraProperties, 42);
##}
##
##TEST_CASE("Test export_helpers") {
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/3NJW.mmtf";
##  mmtf::StructureData sd;
##  mmtf::decodeFromFile(sd, working_mmtf);
##  size_t numbonds_ref = sd.numBonds;
##  std::vector<int32_t> bonddata;
##
##  // test requirement: groupTypeList has duplicates
##  REQUIRE(sd.groupTypeList.size() != sd.groupList.size());
##
##  // must throw with duplicates
##  REQUIRE_THROWS_AS(mmtf::BondAdder(sd), mmtf::EncodeError);
##
##  // make groupTypeList non-duplicate
##  for (size_t i = 1; i < sd.groupTypeList.size(); ++i) {
##    for (size_t j = 0; j < i; ++j) {
##      if (sd.groupTypeList[i] == sd.groupTypeList[j]) {
##        sd.groupTypeList[i] = sd.groupList.size();
##        sd.groupList.push_back(sd.groupList[sd.groupTypeList[j]]);
##        break;
##      }
##    }
##  }
##
##  // test assert: groupTypeList has no duplicates anymore
##  REQUIRE(sd.groupTypeList.size() == sd.groupList.size());
##
##  // remove group bonds
##  size_t offset = 0;
##  for (auto groupType : sd.groupTypeList) {
##    auto & group = sd.groupList[groupType];
##    for (size_t i = 0; i < group.bondOrderList.size(); ++i) {
##      bonddata.push_back(group.bondAtomList[i * 2] + offset);
##      bonddata.push_back(group.bondAtomList[i * 2 + 1] + offset);
##      bonddata.push_back(group.bondOrderList[i]);
##      --sd.numBonds;
##    }
##    offset += group.atomNameList.size();
##    group.bondAtomList.clear();
##    group.bondOrderList.clear();
##  }
##
##  // remove global bonds
##  for (size_t i = 0; i < sd.bondOrderList.size(); ++i) {
##    bonddata.push_back(sd.bondAtomList[i * 2]);
##    bonddata.push_back(sd.bondAtomList[i * 2 + 1]);
##    bonddata.push_back(sd.bondOrderList[i]);
##    --sd.numBonds;
##  }
##  sd.bondAtomList.clear();
##  sd.bondOrderList.clear();
##
##  // test assert: all bonds have been transferred to `bonddata`
##  REQUIRE(bonddata.size() == numbonds_ref * 3);
##  REQUIRE(sd.numBonds == 0);
##  REQUIRE(sd.hasConsistentData());
##
##  // re-add bonds
##  mmtf::BondAdder bondadder(sd);
##  for (size_t i = 0; i < bonddata.size(); i += 3) {
##    REQUIRE(bondadder(bonddata[i], bonddata[i + 1], bonddata[i + 2]));
##  }
##  REQUIRE(sd.numBonds == numbonds_ref);
##  REQUIRE(sd.hasConsistentData());
##
##  // re-compress groupTypeList
##  mmtf::compressGroupList(sd);
##  REQUIRE(sd.groupTypeList.size() != sd.groupList.size());
##  REQUIRE(sd.hasConsistentData());
##
##  // compare with original data
##  mmtf::StructureData sd_ref;
##  mmtf::decodeFromFile(sd_ref, working_mmtf);
##  REQUIRE(sd_ref.bondAtomList == sd.bondAtomList);
##  REQUIRE(sd_ref.bondOrderList == sd.bondOrderList);
##  REQUIRE(sd_ref.groupTypeList == sd.groupTypeList);
##  REQUIRE(sd_ref.groupList == sd.groupList);
##}
##
##TEST_CASE("Test mapdecoder from raw mmtf") {
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##  mmtf::MapDecoder md;
##  mmtf::mapDecoderFromFile(md, working_mmtf);
##  std::vector<int> bonds;
##  REQUIRE_NOTHROW(md.decode("bondAtomList", true, bonds));
##
##  std::ifstream ifs(working_mmtf.c_str(), std::ifstream::in | std::ios::binary);
##  REQUIRE_NOTHROW(mmtf::mapDecoderFromStream(md, ifs));
##  REQUIRE_NOTHROW(md.decode("bondAtomList", true, bonds));
##}
##
##TEST_CASE("Test various encode and decode options") {
##  // fetch reference data
##  std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
##  mmtf::StructureData sd_ref;
##  mmtf::decodeFromFile(sd_ref, working_mmtf);
##  // write into buffer
##  std::ostringstream buffer;
##  mmtf::encodeToStream(sd_ref, buffer);
##  std::string const buffer_str(buffer.str());
##
##  SECTION("decodeFromBuffer") {
##    mmtf::StructureData sd;
##    mmtf::decodeFromBuffer(sd, buffer_str.data(), buffer_str.size());
##    REQUIRE(sd_ref == sd);
##  }
##
##  SECTION("decodeFromStream") {
##    mmtf::StructureData sd;
##    std::istringstream ibuffer(buffer_str);
##    mmtf::decodeFromStream(sd, ibuffer);
##    REQUIRE(sd_ref == sd);
##    // note: as of PR#31 (July 2019), we could also reread the same buffer
##  }
##
##  SECTION("mapDecoderFromBuffer") {
##    mmtf::MapDecoder md;
##    mmtf::mapDecoderFromBuffer(md, buffer_str.data(), buffer_str.size());
##    std::vector<int> bondAtomList;
##    REQUIRE_NOTHROW(md.decode("bondAtomList", true, bondAtomList));
##    REQUIRE(sd_ref.bondAtomList == bondAtomList);
##    mmtf::StructureData sd;
##    mmtf::decodeFromMapDecoder(sd, md);
##    REQUIRE(sd_ref == sd);
##  }
##
##  SECTION("mapDecoderFromStream") {
##    mmtf::MapDecoder md;
##    std::istringstream ibuffer(buffer_str);
##    mmtf::mapDecoderFromStream(md, ibuffer);
##    std::vector<int> bondAtomList;
##    REQUIRE_NOTHROW(md.decode("bondAtomList", true, bondAtomList));
##    REQUIRE(sd_ref.bondAtomList == bondAtomList);
##    mmtf::StructureData sd;
##    mmtf::decodeFromMapDecoder(sd, md);
##    REQUIRE(sd_ref == sd);
##  }
##
##}
##
##TEST_CASE("Test is_hetatm (chain_index version)") {
##  std::string working_mmtf = "../temporary_test_data/3zqs.mmtf";
##  mmtf::StructureData sd;
##  mmtf::decodeFromFile(sd, working_mmtf);
##
##  SECTION("CHECK CHAINS") {
##    int modelIndex = 0;
##    int chainIndex = 0;
##    for (int i = 0; i < sd.numModels; i++, modelIndex++) {
##      for (int j = 0; j < sd.chainsPerModel[modelIndex]; j++, chainIndex++) {
##        // chain indices 0 and 1 belong to a polymer entity
##        // all others should be marked as hetatm
##        if (chainIndex < 2) {
##          REQUIRE(is_polymer(chainIndex, sd.entityList));
##        } else {
##          REQUIRE_FALSE(is_polymer(chainIndex, sd.entityList));
##        }
##      }
##    }
##  }
##
##  SECTION("CHECK WITH GROUP") {
##    // chain indices 0 and 1 belong to polymer entity 0
##    std::string expected_sequence = sd.entityList[0].sequence;
##    int modelIndex = 0;
##    int chainIndex = 0;
##    int groupIndex = 0;
##    for (int i = 0; i < sd.numModels; i++, modelIndex++) {
##      for (int j = 0; j < sd.chainsPerModel[modelIndex]; j++, chainIndex++) {
##        std::string found_seq = "";
##        for (int k = 0; k < sd.groupsPerChain[chainIndex]; k++, groupIndex++) {
##          const mmtf::GroupType& group =
##            sd.groupList[sd.groupTypeList[groupIndex]];
##          bool hetatm = is_hetatm(chainIndex, sd.entityList, group);
##          if (chainIndex < 2) {
##            if (!hetatm) found_seq += group.singleLetterCode;
##          } else {
##            REQUIRE(hetatm);
##          }
##        }
##        if (chainIndex < 2) REQUIRE(expected_sequence == found_seq);
##      }
##    }
##  }
##
##  SECTION("throw check") {
##    REQUIRE_THROWS_AS(is_polymer(999, sd.entityList), mmtf::DecodeError);
##  }
##}
