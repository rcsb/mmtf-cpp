
import time
import numpy as np
import mmtf
import msgpack

import example
from example import CPPStructureData as CPPSD, decodeFromFile

class StructureData:
    def __init__(self, file_name=None, file_bytes=None):
        if file_name:
            self.init_from_file_name(file_name)
        elif file_bytes:
            self.init_from_raw_bytes(file_bytes)
        else:
            self.raw_init()

    def init_from_file_name(self, file_name: str):
        cppsd = CPPSD()
        decodeFromFile(cppsd, file_name)
        self.init_from_cppsd(cppsd)

    def init_from_cppsd(self, cppsd: 'CPPStructureData'):
        self.mmtfVersion = cppsd.mmtfVersion
        self.mmtfProducer = cppsd.mmtfProducer
        self.unitCell = cppsd.unitCell()
        self.spaceGroup = cppsd.spaceGroup
        self.structureId = cppsd.structureId
        self.title = cppsd.title
        self.depositionDate = cppsd.depositionDate
        self.releaseDate = cppsd.releaseDate
        self.ncsOperatorList = cppsd.ncsOperatorList()
        # self.bioAssemblyList = cppsd.bioAssemblyList
        # self.entityList = cppsd.entityList
        self.experimentalMethods = cppsd.experimentalMethods
        self.resolution = cppsd.resolution
        self.rFree = cppsd.rFree
        self.rWork = cppsd.rWork
        self.numBonds = cppsd.numBonds
        self.numAtoms = cppsd.numAtoms
        self.numGroups = cppsd.numGroups
        self.numChains = cppsd.numChains
        self.numModels = cppsd.numModels
        # self.groupList = cppsd.groupList

        self.bondAtomList = cppsd.bondAtomList()
        self.bondOrderList = cppsd.bondOrderList()
        self.bondResonanceList = cppsd.bondResonanceList()
        self.xCoordList = cppsd.xCoordList()
        self.yCoordList = cppsd.yCoordList()
        self.zCoordList = cppsd.zCoordList()
        self.bFactorList = cppsd.bFactorList()
        self.atomIdList = cppsd.atomIdList()
        self.altLocList = cppsd.altLocList
        self.occupancyList = cppsd.occupancyList()
        # print(type(cppsd.groupIdList()))
        self.groupIdList = cppsd.groupIdList()
        # print(self.groupIdList)
        self.groupIdList = cppsd.groupIdList()
        self.groupTypeList = cppsd.groupTypeList()
        self.secStructList = cppsd.secStructList()
        self.insCodeList = cppsd.insCodeList
        self.sequenceIndexList = cppsd.sequenceIndexList()
        self.chainIdList = cppsd.chainIdList
        self.chainNameList = cppsd.chainNameList
        self.groupsPerChain = cppsd.groupsPerChain()
        self.chainsPerModel = cppsd.chainsPerModel()


        # self.bondAtomList = np.array(cppsd.bondAtomList(), copy=False)
        # self.bondOrderList = np.array(cppsd.bondOrderList(), copy=False)
        # self.bondResonanceList = np.array(cppsd.bondResonanceList(), copy=False)
        # self.xCoordList = np.array(cppsd.xCoordList(), copy=False)
        # self.yCoordList = np.array(cppsd.yCoordList(), copy=False)
        # self.zCoordList = np.array(cppsd.zCoordList(), copy=False)
        # self.bFactorList = np.array(cppsd.bFactorList(), copy=False)
        # self.atomIdList = np.array(cppsd.atomIdList(), copy=False)
        # self.altLocList = np.array(cppsd.altLocList, copy=False)
        # self.occupancyList = np.array(cppsd.occupancyList(), copy=False)
        # self.groupIdList = np.array(cppsd.groupIdList(), copy=False)
        # # print(self.groupIdList)
        # self.groupIdList = np.array(cppsd.groupIdList(), copy=False)
        # self.groupTypeList = np.array(cppsd.groupTypeList(), copy=False)
        # self.secStructList = np.array(cppsd.secStructList(), copy=False)
        # self.insCodeList = np.array(cppsd.insCodeList, copy=False)
        # self.sequenceIndexList = np.array(cppsd.sequenceIndexList(), copy=False)
        # self.chainIdList = np.array(cppsd.chainIdList, copy=False)
        # self.chainNameList = np.array(cppsd.chainNameList, copy=False)
        # self.groupsPerChain = np.array(cppsd.groupsPerChain(), copy=False)
        # self.chainsPerModel = np.array(cppsd.chainsPerModel(), copy=False)

        raw_properties = cppsd.raw_properties()
        raw_properties = msgpack.unpackb(raw_properties, raw=False)
        # print(type(raw_properties), len(raw_properties))
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



start = time.time()
for x in range(10000):
    sd = StructureData("4lgr.mmtf")
stop = time.time()
python_t = stop-start
print("python", python_t)

start = time.time()
for x in range(1000):
    sd = mmtf.parse("4lgr.mmtf")
stop = time.time()
python_t = stop-start
print("python og", python_t)
 
start = time.time()
for x in range(10000):
    cppsd = CPPSD()
    decodeFromFile(cppsd, "4lgr.mmtf")
stop = time.time()
cpp_t = stop-start
print("cpp", cpp_t)
