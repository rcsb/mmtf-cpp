
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


def test_DeltaRecursiveFloat():
    encoded_data = b'\x00\x00\x00\x0a\x00\x00\x00\x03\x00\x00\x03\xe8\x7f\xffD\xab\x01\x8f\xff\xca'
    decoded_data = np.array([50.346, 50.745, 50.691])
    ret = mmtf_cpp.encodeDeltaRecursiveFloat(decoded_data, 1000)
    assert ret == encoded_data


def test_RunLengthFloat():
    encoded_data = b'\x00\x00\x00\x09\x00\x00\x00\x03\x00\x00\x00\x64\x00\x00\x00\x64\x00\x00\x00\x03'
    decoded_data = np.array([1.0, 1.0, 1.0])
    ret = mmtf_cpp.encodeRunLengthFloat(decoded_data, 100)
    assert ret == encoded_data


def test_RunLengthDeltaInt():
    encoded_data = b'\x00\x00\x00\x08\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x07'
    decoded_data = np.array(range(1,8))
    ret = mmtf_cpp.encodeRunLengthDeltaInt(decoded_data)
    assert ret == encoded_data


def test_RunLengthChar():
    encoded_data = b'\x00\x00\x00\x06\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x41\x00\x00\x00\x04'
    decoded_data = np.array(list(map(ord, ("A", "A", "A", "A"))), dtype=np.int8)
    ret = mmtf_cpp.encodeRunLengthChar(decoded_data)
    assert ret == encoded_data


def test_RunLengthInt8():
    encoded_data = b'\x00\x00\x00\x10\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x04'
    decoded_data = np.array((2, 2, 2, 2), dtype=np.int8)
    ret = mmtf_cpp.encodeRunLengthInt8(decoded_data)
    assert ret == encoded_data


def test_Int8():
    encoded_data = b'\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00\x00\x07\x06\x06\x07\x07'
    decoded_data = np.array((7, 6, 6, 7, 7), dtype=np.int8)
    ret = mmtf_cpp.encodeInt8ToByte(decoded_data)
    assert ret == encoded_data


def test_FourByteInt():
    encoded_data = b'\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x02\x00\x01\x00\x00\x00\x00\x00\x00\x00\x02'
    decoded_data = np.array((1, 131073, 0, 2), dtype=np.int32)
    ret = mmtf_cpp.encodeFourByteInt(decoded_data)
    assert ret == encoded_data

# TODO figure out numpy array -> c++ vector string
# def test_encodeStringVector():
#     encoded_data = b'\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x04B\x00\x00\x00A\x00\x00\x00C\x00\x00\x00A\x00\x00\x00A\x00\x00\x00A\x00\x00\x00'
#     decoded_data = np.array(("B", "A", "C", "A", "A", "A"))
#     ret = mmtf_cpp.encodeStringVector(decoded_data, 4)
#     assert ret == encoded_data



def test_atomProperties(test_data_dir):
    working_mmtf_fn = os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/173D.mmtf")
    sd = StructureData(working_mmtf_fn)
    random_data = list(range(256))
    encoded_random_data = mmtf_cpp.encodeRunLengthDeltaInt(list((range(256))))
    sd.atomProperties["256_atomColorList"] = random_data
    sd.atomProperties["256_atomColorList_encoded"] = encoded_random_data
    sd.write_to_file("atomProperties_test.mmtf")
    sd2 = StructureData("atomProperties_test.mmtf")
    assert sd2.atomProperties["256_atomColorList"] == random_data
    assert sd2.atomProperties["256_atomColorList_encoded"] == encoded_random_data
    assert (mmtf_cpp.decode_int32(sd2.atomProperties["256_atomColorList_encoded"]) == np.array(random_data)).all()
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
