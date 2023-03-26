find_package(Boost 1.81.0 REQUIRED COMPONENTS json)

set(TARGET gltfpp)

set(SRC
	gltf.h
	model.h
	write.cpp
	write.h)

add_library(${TARGET} STATIC ${SRC})

target_compile_features(${TARGET}
	PRIVATE
		cxx_std_20)

target_link_libraries(${TARGET} PRIVATE Boost::json)
