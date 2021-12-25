
FILE(GLOB ALL_CPP *.cpp)
FILE(GLOB ALL_HEADER *.h)

include_directories(${OPENCL_INCLUDES})
add_library(OpenSurfCL ${ALL_CPP} ${ALL_HEADER} ${ALL_CL})
target_link_libraries(OpenSurfCL ${OPENCL_LIBRARY})

IF(RELEASE)
	FILE(GLOB ALL_CL ${OPENSURFCL_SOURCE_DIR}/data/*.cl)
	FOREACH(DATAFILE ${ALL_CL})
		if(MSVC)
			add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} $(TargetDir))
		elseif()
		
			add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} ${EXECUTABLE_OUTPUT_PATH})
		endif()
		add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} .)
	ENDFOREACH()
	FILE(GLOB ALL_CL ${OPENSURFCL_SOURCE_DIR}/data/*.bmp)
	FOREACH(DATAFILE ${ALL_CL})
		if(MSVC)
			add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} $(TargetDir))
		elseif()
		
			add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} ${EXECUTABLE_OUTPUT_PATH})
		endif()
		add_custom_command( TARGET OpenSurfCL PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${DATAFILE} .)
	ENDFOREACH()
ENDIF()