# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

## v1.1.0 - 2022-10-03
### Added
- New mapDecoderFrom.. functions to decode only part of an MMTF file
- Support for extra fields in MMTF files according to the
  [latest MMTF specification](https://github.com/rcsb/mmtf/pull/36).
- Support for binary strategy 16 (Run-length encoded 8-bit array),
  bondResonanceList field and optional groupType.bondAtomList &
  groupType.bondOrderList according to the proposed version 1.1 of the
  [MMTF specification](https://github.com/rcsb/mmtf/pull/35).
- New methods to find polymer chains and HETATM following discussions in
  [rcsb/mmtf#28](https://github.com/rcsb/mmtf/issues/28).
- Altered submodule locations [rcsb/mmtf-cpp#37](https://github.com/rcsb/mmtf-cpp/pull/37)
  from the base directory to the new submodules directory.

## v1.0.0 - 2019-02-05
### Added
- Initial release including decoder and encoder for the
  [MMTF specification 1.0](https://github.com/rcsb/mmtf/blob/v1.0/spec.md).

[Unreleased]: https://github.com/rcsb/mmtf-cpp/compare/v1.1.0...HEAD
