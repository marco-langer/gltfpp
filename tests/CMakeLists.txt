find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

enable_testing()

set(SRC
	"test_gltf.cpp")

set(TARGET hello_gltf)

add_executable(${TARGET} ${SRC})

target_compile_features(${TARGET} PRIVATE cxx_std_20)

target_include_directories(${TARGET}
	PRIVATE
		${GTEST_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(${TARGET}
	PRIVATE
		gltfpp
		fmt::fmt
		GTest::gtest_main)

gtest_discover_tests(${TARGET})