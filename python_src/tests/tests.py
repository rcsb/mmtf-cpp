import os
import mmtf_cppy
from mmtf_cppy import CPPStructureData, decodeFromFile, StructureData
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
    doesnt_work = ["mmtf_spec/test-suite/mmtf/empty-mmtfVersion99999999.mmtf"]
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
    encoded_data = b"\x00\x00\x00\x0a\x00\x00\x00\x03\x00\x00\x03\xe8\x7f\xffD\xab\x01\x8f\xff\xca"
    decoded_data = np.array([50.346, 50.745, 50.691])
    ret = mmtf_cppy.encodeDeltaRecursiveFloat(decoded_data, 1000)
    assert ret == encoded_data


def test_RunLengthFloat():
    encoded_data = b"\x00\x00\x00\x09\x00\x00\x00\x03\x00\x00\x00\x64\x00\x00\x00\x64\x00\x00\x00\x03"
    decoded_data = np.array([1.0, 1.0, 1.0])
    ret = mmtf_cppy.encodeRunLengthFloat(decoded_data, 100)
    assert ret == encoded_data


def test_RunLengthDeltaInt():
    encoded_data = b"\x00\x00\x00\x08\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x07"
    decoded_data = np.array(range(1, 8))
    ret = mmtf_cppy.encodeRunLengthDeltaInt(decoded_data)
    assert ret == encoded_data


def test_RunLengthChar():
    encoded_data = b"\x00\x00\x00\x06\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x41\x00\x00\x00\x04"
    decoded_data = np.array(list(map(ord, ("A", "A", "A", "A"))), dtype=np.int8)
    ret = mmtf_cppy.encodeRunLengthChar(decoded_data)
    assert ret == encoded_data


def test_RunLengthInt8():
    encoded_data = b"\x00\x00\x00\x10\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x04"
    decoded_data = np.array((2, 2, 2, 2), dtype=np.int8)
    ret = mmtf_cppy.encodeRunLengthInt8(decoded_data)
    assert ret == encoded_data


def test_Int8():
    encoded_data = b"\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00\x00\x07\x06\x06\x07\x07"
    decoded_data = np.array((7, 6, 6, 7, 7), dtype=np.int8)
    ret = mmtf_cppy.encodeInt8ToByte(decoded_data)
    assert ret == encoded_data


def test_FourByteInt():
    encoded_data = b"\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x02\x00\x01\x00\x00\x00\x00\x00\x00\x00\x02"
    decoded_data = np.array((1, 131073, 0, 2), dtype=np.int32)
    ret = mmtf_cppy.encodeFourByteInt(decoded_data)
    assert ret == encoded_data


# TODO figure out numpy array -> c++ vector string
# def test_encodeStringVector():
#     encoded_data = b'\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x04B\x00\x00\x00A\x00\x00\x00C\x00\x00\x00A\x00\x00\x00A\x00\x00\x00A\x00\x00\x00'
#     decoded_data = np.array(("B", "A", "C", "A", "A", "A"))
#     ret = mmtf_cppy.encodeStringVector(decoded_data, 4)
#     assert ret == encoded_data


def test_atomProperties(test_data_dir):
    working_mmtf_fn = os.path.join(test_data_dir, "mmtf_spec/test-suite/mmtf/173D.mmtf")
    sd = StructureData(working_mmtf_fn)
    random_data = list(range(256))
    encoded_random_data = mmtf_cppy.encodeRunLengthDeltaInt(list((range(256))))
    sd.atomProperties["256_atomColorList"] = random_data
    sd.atomProperties["256_atomColorList_encoded"] = encoded_random_data
    sd.write_to_file("atomProperties_test.mmtf")
    sd2 = StructureData("atomProperties_test.mmtf")
    assert sd2.atomProperties["256_atomColorList"] == random_data
    assert sd2.atomProperties["256_atomColorList_encoded"] == encoded_random_data
    assert (mmtf_cppy.decode_int32(sd2.atomProperties["256_atomColorList_encoded"]) == np.array(random_data)).all()
