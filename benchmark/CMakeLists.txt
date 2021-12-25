

FILE(GLOB ALL_CPP *.cpp)
FILE(GLOB ALL_HEADER *.h)


include_directories(${OPENSURFCL_SOURCE_DIR}/opensurfcl ${OPENCL_INCLUDES})
add_executable(Benchmark ${ALL_CPP} ${ALL_HEADER} )
target_link_libraries(Benchmark OpenSurfCL)
