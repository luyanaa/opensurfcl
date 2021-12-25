

FILE(GLOB ALL_CPP *.cpp)
FILE(GLOB ALL_HEADER *.h)


#TODO -> so and a files files
FILE(GLOB OPENCVLIBS ${DEMO_OPENCV_LIB_PATH}/*.lib)
FILE(GLOB OPENCVDYNLIB ${DEMO_OPENCV_DYNLIB_PATH}/*.dll)



include_directories(${OPENSURFCL_SOURCE_DIR}/opensurfcl ${OPENCL_INCLUDES} ${DEMO_OPENCV_INCLUDE_PATH})
add_executable(Demo ${ALL_CPP} ${ALL_HEADER})
target_link_libraries(Demo OpenSurfCL ${OPENCVLIBS})


FOREACH (CUR_DLL ${OPENCVDYNLIB})
	
	if(MSVC)
		add_custom_command( TARGET Demo POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy ${CUR_DLL} $(TargetDir))
	else()
		add_custom_command( TARGET Demo POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy ${CUR_DLL} ${EXECUTABLE_OUTPUT_PATH})
	endif()
ENDFOREACH(CUR_DLL)

