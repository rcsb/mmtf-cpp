import time
import numpy as np
import mmtf
import msgpack
from typing import List, Dict

import mmtf_cpp
from mmtf_cpp import CPPStructureData as CPPSD, decodeFromFile, decodeFromBuffer, encodeToFile, encodeToStream


import numpy as np
np.set_printoptions(threshold=np.inf)


class Timer:
    def __init__(self, name, to_add):
        self.name = name
        self.to_add = to_add

    def __enter__(self):
        self.start = time.time()
        return self

    def __exit__(self, *args):
        self.end = time.time()
        interval = self.end - self.start + self.to_add[0]
        print(f"time for {self.name} was: {interval}")
        self.to_add[0] = interval


class Entity:
    def __init__(self, chainIndexList, description, type_, sequence):
        self.chainIndexList = chainIndexList
        self.description = description
        self.type = type_
        self.sequence = sequence

    def __repr__(self):
        return (
            f"chainIndexList: {self.chainIndexList}"
            f"description: {self.description}"
            f"type: {self.type}"
            f"sequence: {self.sequence}"
        )

    def __eq__(self, other: "Entity"):
        return (
            (self.chainIndexList == other.chainIndexList).all()
            and self.description == other.description
            and self.type == other.type
            and self.sequence == other.sequence
        )


class GroupType:
    def __init__(
        self,
        formalChargeList: np.ndarray,
        atomNameList,
        elementList,
        bondAtomList,
        bondOrderList,
        bondResonanceList,
        groupName,
        singleLetterCode,
        chemCompType,
    ):
        self.formalChargeList = formalChargeList
        self.atomNameList = atomNameList
        self.elementList = elementList
        self.bondAtomList = bondAtomList
        self.bondOrderList = bondOrderList
        self.bondResonanceList = bondResonanceList
        self.groupName = groupName
        self.singleLetterCode = singleLetterCode
        self.chemCompType = chemCompType

    def __repr__(self):
        return (
            f"formalChargeList: {self.formalChargeList}"
            f" atomNameList: {self.atomNameList}"
            f" elementList: {self.elementList}"
            f" bondAtomList: {self.bondAtomList}"
            f" bondOrderList: {self.bondOrderList}"
            f" bondResonanceList: {self.bondResonanceList}"
            f" groupName: {self.groupName}"
            f" singleLetterCode: {self.singleLetterCode}"
            f" chemCompType: {self.chemCompType}"
        )

    def __eq__(self, other: "GroupType"):
        return (
            (self.formalChargeList == other.formalChargeList).all()
            and self.atomNameList == other.atomNameList
            and self.elementList == other.elementList
            and (self.bondAtomList == other.bondAtomList).all()
            and (self.bondOrderList == other.bondOrderList).all()
            and (self.bondResonanceList == other.bondResonanceList).all()
            and self.groupName == other.groupName
            and self.singleLetterCode == other.singleLetterCode
            and self.chemCompType == other.chemCompType
        )


class Transform:
    def __init__(self, chainIndexList: List[int], matrix: np.ndarray):
        self.chainIndexList = chainIndexList
        self.matrix = matrix

    def __eq__(self, other: "Transform"):
        return (self.chainIndexList == other.chainIndexList).all() and (self.matrix == other.matrix).all()


class BioAssembly:
    def __init__(self, transformList: List[Transform], name: str):
        self.transformList = transformList
        self.name = name

    def __eq__(self, other: "BioAssembly"):
        return self.transformList == other.transformList and self.name == other.name


def cppSD_from_SD(sd: "StructureData"):
    cppsd = CPPSD()
    cppsd.mmtfVersion = sd.mmtfVersion
    cppsd.mmtfProducer = sd.mmtfProducer
    cppsd.unitCell_io = sd.unitCell
    cppsd.spaceGroup = sd.spaceGroup
    cppsd.structureId = sd.structureId
    cppsd.title = sd.title
    cppsd.depositionDate = sd.depositionDate
    cppsd.releaseDate = sd.releaseDate
    cppsd.ncsOperatorList_io = sd.ncsOperatorList
    mmtf_cpp.set_bioAssemblyList(sd.bioAssemblyList, cppsd)
    mmtf_cpp.set_entityList(sd.entityList, cppsd)
    cppsd.experimentalMethods = sd.experimentalMethods
    cppsd.resolution = sd.resolution
    cppsd.rFree = sd.rFree
    cppsd.rWork = sd.rWork
    cppsd.numBonds = sd.numBonds
    cppsd.numAtoms = sd.numAtoms
    cppsd.numGroups = sd.numGroups
    cppsd.numChains = sd.numChains
    cppsd.numModels = sd.numModels
    mmtf_cpp.set_groupList(sd.groupList, cppsd)
    cppsd.bondAtomList_io = sd.bondAtomList
    cppsd.bondOrderList_io = sd.bondOrderList
    cppsd.bondResonanceList_io = sd.bondResonanceList
    cppsd.xCoordList_io = sd.xCoordList
    cppsd.yCoordList_io = sd.yCoordList
    cppsd.zCoordList_io = sd.zCoordList
    cppsd.bFactorList_io = sd.bFactorList
    cppsd.atomIdList_io = sd.atomIdList
    tmp_altLocList = np.array([ord(x) if x else 0 for x in sd.altLocList], dtype=np.int8)
    cppsd.set_altLocList(tmp_altLocList)
    del tmp_altLocList
    cppsd.occupancyList_io = sd.occupancyList
    cppsd.groupIdList_io = sd.groupIdList
    cppsd.groupTypeList_io = sd.groupTypeList
    cppsd.secStructList_io = sd.secStructList
    tmp_insCodeList = np.array([ord(x) if x else 0 for x in sd.insCodeList], dtype=np.int8)
    cppsd.set_insCodeList(np.int8(tmp_insCodeList))
    del tmp_insCodeList
    cppsd.sequenceIndexList_io = sd.sequenceIndexList
    cppsd.chainIdList_io = sd.chainIdList
    cppsd.chainNameList_io = sd.chainNameList
    cppsd.groupsPerChain_io = sd.groupsPerChain
    cppsd.chainsPerModel_io = sd.chainsPerModel
    packed_data = msgpack.packb({
            "bondProperties": sd.bondProperties,
            "atomProperties": sd.atomProperties,
            "groupProperties": sd.groupProperties,
            "chainProperties": sd.chainProperties,
            "modelProperties": sd.modelProperties,
            "extraProperties": sd.extraProperties},
            use_bin_type=True)
    cppsd.set_properties(packed_data)
    return cppsd


class StructureData:
    def __init__(self, file_name=None, file_bytes=None, debuf_filemames=None):
        """

        Note:
            file and bytes are separated because it will be faster
            if you just let c++ handle the file (rather than have
            python read the bytes itself and pass them to c++)
        """
        if file_name:
            self.init_from_file_name(file_name)
        elif file_bytes:
            self.init_from_bytes(file_bytes)
        elif debuf_filemames:
            self.init_from_file_names_debug(debuf_filemames)
        else:
            self.raw_init()

    def init_from_bytes(self, file_bytes: bytes):
        cppsd = CPPSD()
        decodeFromBuffer(cppsd, file_bytes, len(file_bytes))
        self.init_from_cppsd(cppsd)

    def init_from_file_name(self, file_name: str):
        cppsd = CPPSD()
        decodeFromFile(cppsd, file_name)
        self.init_from_cppsd(cppsd)

    def init_from_cppsd(self, cppsd: "CPPStructureData"):
        self.mmtfVersion = cppsd.mmtfVersion
        self.mmtfProducer = cppsd.mmtfProducer
        self.unitCell = cppsd.unitCell()
        self.spaceGroup = cppsd.spaceGroup
        self.structureId = cppsd.structureId
        self.title = cppsd.title
        self.depositionDate = cppsd.depositionDate
        self.releaseDate = cppsd.releaseDate
        self.ncsOperatorList = cppsd.ncsOperatorList()
        self.bioAssemblyList = cppsd.bioAssemblyList()
        self.entityList = cppsd.entityList()
        self.experimentalMethods = cppsd.experimentalMethods
        self.resolution = cppsd.resolution
        self.rFree = cppsd.rFree
        self.rWork = cppsd.rWork
        self.numBonds = cppsd.numBonds
        self.numAtoms = cppsd.numAtoms
        self.numGroups = cppsd.numGroups
        self.numChains = cppsd.numChains
        self.numModels = cppsd.numModels
        self.groupList = cppsd.groupList()
        self.bondAtomList = cppsd.bondAtomList()
        self.bondOrderList = cppsd.bondOrderList()
        self.bondResonanceList = cppsd.bondResonanceList()
        self.xCoordList = cppsd.xCoordList()
        self.yCoordList = cppsd.yCoordList()
        self.zCoordList = cppsd.zCoordList()
        self.bFactorList = cppsd.bFactorList()
        self.atomIdList = cppsd.atomIdList()
        self.altLocList = cppsd.altLocList()  # slow
        self.occupancyList = cppsd.occupancyList()
        self.groupIdList = cppsd.groupIdList()
        self.groupTypeList = cppsd.groupTypeList() # slow
        self.secStructList = cppsd.secStructList()
        self.insCodeList = cppsd.insCodeList()  # slow
        self.sequenceIndexList = cppsd.sequenceIndexList()
        self.chainIdList = cppsd.chainIdList()
        self.chainNameList = cppsd.chainNameList()
        self.groupsPerChain = cppsd.groupsPerChain()
        self.chainsPerModel = cppsd.chainsPerModel()

        raw_properties = cppsd.raw_properties()
        raw_properties = msgpack.unpackb(raw_properties, raw=False)
        self.bondProperties = raw_properties["bondProperties"]
        self.atomProperties = raw_properties["atomProperties"]
        self.groupProperties = raw_properties["groupProperties"]
        self.chainProperties = raw_properties["chainProperties"]
        self.modelProperties = raw_properties["modelProperties"]
        self.extraProperties = raw_properties["extraProperties"]

    def raw_init(self):
        self.mmtfVersion = None
        self.mmtfProducer = None
        self.unitCell = None
        self.spaceGroup = None
        self.structureId = None
        self.title = None
        self.depositionDate = None
        self.releaseDate = None
        self.ncsOperatorList = None
        self.bioAssemblyList = None
        self.entityList = None
        self.experimentalMethods = None
        self.resolution = None
        self.rFree = None
        self.rWork = None
        self.numBonds = None
        self.numAtoms = None
        self.numGroups = None
        self.numChains = None
        self.numModels = None
        self.groupList = None
        self.bondAtomList = None
        self.bondOrderList = None
        self.bondResonanceList = None
        self.xCoordList = None
        self.yCoordList = None
        self.zCoordList = None
        self.bFactorList = None
        self.atomIdList = None
        self.altLocList = None
        self.occupancyList = None
        self.groupIdList = None
        self.groupTypeList = None
        self.secStructList = None
        self.insCodeList = None
        self.sequenceIndexList = None
        self.chainIdList = None
        self.chainNameList = None
        self.groupsPerChain = None
        self.chainsPerModel = None
        self.bondProperties = None
        self.atomProperties = None
        self.groupProperties = None
        self.chainProperties = None
        self.modelProperties = None
        self.extraProperties = None

    def write_to_file(self, filename: str):
        cppsd = cppSD_from_SD(self)
        encodeToFile(cppsd, filename)

    def write_to_bytes(self):
        cppsd = cppSD_from_SD(self)
        buff = encodeToStream(cppsd)
        return buff

    def check_equals(self, other: "StructureData"):
        if not (self.mmtfVersion == other.mmtfVersion):
            print("NOT self.mmtfVersion == other.mmtfVersion")
        if not (self.mmtfProducer == other.mmtfProducer):
            print("NOT self.mmtfProducer == other.mmtfProducer")
        if not ((self.unitCell == other.unitCell).all()):
            print("NOT (self.unitCell == other.unitCell).all()")
        if not (self.spaceGroup == other.spaceGroup):
            print("NOT self.spaceGroup == other.spaceGroup")
        if not (self.structureId == other.structureId):
            print("NOT self.structureId == other.structureId")
        if not (self.title == other.title):
            print("NOT self.title == other.title")
        if not (self.depositionDate == other.depositionDate):
            print("NOT self.depositionDate == other.depositionDate")
        if not (self.releaseDate == other.releaseDate):
            print("NOT self.releaseDate == other.releaseDate")
        if not ((self.ncsOperatorList == other.ncsOperatorList).all()):
            print("NOT (self.ncsOperatorList == other.ncsOperatorList).all()")
        if not (self.bioAssemblyList == other.bioAssemblyList):
            print("NOT self.bioAssemblyList == other.bioAssemblyList")
        if not (self.entityList == other.entityList):
            print("NOT self.entityList == other.entityList")
        if not (self.experimentalMethods == other.experimentalMethods):
            print("NOT self.experimentalMethods == other.experimentalMethods")
        if not (self.resolution == other.resolution):
            print("NOT self.resolution == other.resolution")
        if not (self.rFree == other.rFree):
            print("NOT self.rFree == other.rFree")
        if not (self.rWork == other.rWork):
            print("NOT self.rWork == other.rWork")
        if not (self.numBonds == other.numBonds):
            print("NOT self.numBonds == other.numBonds")
        if not (self.numAtoms == other.numAtoms):
            print("NOT self.numAtoms == other.numAtoms")
        if not (self.numGroups == other.numGroups):
            print("NOT self.numGroups == other.numGroups")
        if not (self.numChains == other.numChains):
            print("NOT self.numChains == other.numChains")
        if not (self.numModels == other.numModels):
            print("NOT self.numModels == other.numModels")
        if not (self.groupList == other.groupList):
            print("NOT self.groupList == other.groupList")
        if not ((self.bondAtomList == other.bondAtomList).all()):
            print("NOT (self.bondAtomList == other.bondAtomList).all()")
        if not ((self.bondOrderList == other.bondOrderList).all()):
            print("NOT (self.bondOrderList == other.bondOrderList).all()")
        if not ((self.bondResonanceList == other.bondResonanceList).all()):
            print("NOT (self.bondResonanceList == other.bondResonanceList).all()")
        if not ((self.xCoordList == other.xCoordList).all()):
            print("NOT (self.xCoordList == other.xCoordList).all()")
        if not ((self.yCoordList == other.yCoordList).all()):
            print("NOT (self.yCoordList == other.yCoordList).all()")
        if not ((self.zCoordList == other.zCoordList).all()):
            print("NOT (self.zCoordList == other.zCoordList).all()")
        if not ((self.bFactorList == other.bFactorList).all()):
            print("NOT (self.bFactorList == other.bFactorList).all()")
        if not ((self.atomIdList == other.atomIdList).all()):
            print("NOT (self.atomIdList == other.atomIdList).all()")
        if not ((self.altLocList == other.altLocList).all()):
            print("NOT (self.altLocList == other.altLocList).all()")
        if not ((self.occupancyList == other.occupancyList).all()):
            print("NOT (self.occupancyList == other.occupancyList).all()")
        if not ((self.groupIdList == other.groupIdList).all()):
            print("NOT (self.groupIdList == other.groupIdList).all()")
        if not ((self.groupTypeList == other.groupTypeList).all()):
            print("NOT (self.groupTypeList == other.groupTypeList).all()")
        if not ((self.secStructList == other.secStructList).all()):
            print("NOT (self.secStructList == other.secStructList).all()")
        if not ((self.insCodeList == other.insCodeList).all()):
            print("NOT (self.insCodeList == other.insCodeList).all()")
        if not ((self.sequenceIndexList == other.sequenceIndexList).all()):
            print("NOT (self.sequenceIndexList == other.sequenceIndexList).all()")
        if not ((self.chainIdList == other.chainIdList).all()):
            print("NOT (self.chainIdList == other.chainIdList).all()")
        if not ((self.chainNameList == other.chainNameList).all()):
            print("NOT (self.chainNameList == other.chainNameList).all()")
        if not ((self.groupsPerChain == other.groupsPerChain).all()):
            print("NOT (self.groupsPerChain == other.groupsPerChain).all()")
        if not ((self.chainsPerModel == other.chainsPerModel).all()):
            print("NOT (self.chainsPerModel == other.chainsPerModel).all()")
        if not (self.bondProperties == other.bondProperties):
            print("NOT self.bondProperties == other.bondProperties")
        if not (self.atomProperties == other.atomProperties):
            print("NOT self.atomProperties == other.atomProperties")
        if not (self.groupProperties == other.groupProperties):
            print("NOT self.groupProperties == other.groupProperties")
        if not (self.chainProperties == other.chainProperties):
            print("NOT self.chainProperties == other.chainProperties")
        if not (self.modelProperties == other.modelProperties):
            print("NOT self.modelProperties == other.modelProperties")
        if not (self.extraProperties == other.extraProperties):
            print("NOT self.extraProperties == other.extraProperties")

    def __eq__(self, other: "StructureData"):
        return (
            self.mmtfVersion == other.mmtfVersion
            and self.mmtfProducer == other.mmtfProducer
            and (self.unitCell == other.unitCell).all()
            and self.spaceGroup == other.spaceGroup
            and self.structureId == other.structureId
            and self.title == other.title
            and self.depositionDate == other.depositionDate
            and self.releaseDate == other.releaseDate
            and (self.ncsOperatorList == other.ncsOperatorList).all()
            and self.bioAssemblyList == other.bioAssemblyList
            and self.entityList == other.entityList
            and self.experimentalMethods == other.experimentalMethods
            and self.resolution == other.resolution
            and self.rFree == other.rFree
            and self.rWork == other.rWork
            and self.numBonds == other.numBonds
            and self.numAtoms == other.numAtoms
            and self.numGroups == other.numGroups
            and self.numChains == other.numChains
            and self.numModels == other.numModels
            and self.groupList == other.groupList
            and (self.bondAtomList == other.bondAtomList).all()
            and (self.bondOrderList == other.bondOrderList).all()
            and (self.bondResonanceList == other.bondResonanceList).all()
            and (self.xCoordList == other.xCoordList).all()
            and (self.yCoordList == other.yCoordList).all()
            and (self.zCoordList == other.zCoordList).all()
            and (self.bFactorList == other.bFactorList).all()
            and (self.atomIdList == other.atomIdList).all()
            and (self.altLocList == other.altLocList).all()
            and (self.occupancyList == other.occupancyList).all()
            and (self.groupIdList == other.groupIdList).all()
            and (self.groupTypeList == other.groupTypeList).all()
            and (self.secStructList == other.secStructList).all()
            and (self.insCodeList == other.insCodeList).all()
            and (self.sequenceIndexList == other.sequenceIndexList).all()
            and (self.chainIdList == other.chainIdList).all()
            and (self.chainNameList == other.chainNameList).all()
            and (self.groupsPerChain == other.groupsPerChain).all()
            and (self.chainsPerModel == other.chainsPerModel).all()
            and self.bondProperties == other.bondProperties
            and self.atomProperties == other.atomProperties
            and self.groupProperties == other.groupProperties
            and self.chainProperties == other.chainProperties
            and self.modelProperties == other.modelProperties
            and self.extraProperties == other.extraProperties
        )


# import numpy as np

# x = np.array(range(10), dtype=np.uint8)
# x[0] = 4
# y = np.array(range(10), dtype=np.uint8)
# y[0] = 4
# print(mmtf_cpp.encodeInt8ToByte(x), x)
# print(mmtf_cpp.encodeInt8ToByte_destructive(y), y)
