#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <mmtf.hpp>

// Tests for structure_data.hpp
TEST_CASE("Test for equality of StructureData") {
	mmtf::StructureData sd1, sd2;
	REQUIRE( sd1 == sd2 );
}
