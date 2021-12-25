
#FILE(GLOB ALL_CPP *.cpp)
#FILE(GLOB ALL_HEADER *.h)
SET(ALL_CPP CSurfCL.cpp CCLDevice.cpp)
SET(ALL_HEADER CSurfCL.h CCLDevice.h)

include_directories(${OPENCL_INCLUDES})
add_library(OpenSurfCL ${ALL_CPP} ${ALL_HEADER} ${ALL_CL})
target_link_libraries(OpenSurfCL ${OPENCL_LIBRARY})

FILE(GLOB ALL_CL *.cl)



#FILE(GLOB ALL_CL RELATIVE ${OPENSURFCL_SOURCE_DIR}/opensurfcl ${OPENSURFCL_SOURCE_DIR}/opensurfcl/*.cl)
#MESSAGE(${ALL_CL})
#foreach(clFile ${ALL_CL})
#	MESSAGE(${clFile})
#	configure_file(${clFile} ${EXECUTABLE_OUTPUT_PATH}/${clFile} @ONLY )
#endforeach()
