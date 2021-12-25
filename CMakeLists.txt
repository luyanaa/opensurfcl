cmake_minimum_required(VERSION 2.8)
PROJECT("OPENSURFCL")



SET (LIBRARY_OUTPUT_PATH ${OPENSURFCL_SOURCE_DIR}/build CACHE PATH "Output directory for library files.")
SET (EXECUTABLE_OUTPUT_PATH ${OPENSURFCL_SOURCE_DIR}/build CACHE PATH "Output directory for executable files.")

SET (DEMO_OPENCV_LIB_PATH  CACHE PATH "OpenCV libraries.")
SET (DEMO_OPENCV_DYNLIB_PATH  CACHE PATH "OpenCV dynamic libraries like dll.")
SET (DEMO_OPENCV_INCLUDE_PATH  CACHE PATH "OpenCV include files.")
#the following line allows you to use DATA_DIRECTORY in the sourcecode to get the cl files

SET(RELEASE CACHE BOOL "Copies .cl and .bmp files to the executable and uses a relativ path.")
IF(RELEASE)	
	ADD_DEFINITIONS(-DDATA_DIRECTORY=".")
ELSE()
	ADD_DEFINITIONS(-DDATA_DIRECTORY="${OPENSURFCL_SOURCE_DIR}/data")
ENDIF()
#


	#OpenCL library search tested under:
	#Win32 + ATISDK
	#Win32 + NVIDIA
	#Linux + ATISDK (only if env variable set)
	#Linux + NVIDIA -> didn't work. There is no .a or .so file in SDK
	
	#search amd opencl library
	FIND_PATH(OPENCL_BASE_DIR include/CL/cl.h PATH  $ENV{ATISTREAMSDKROOT} )
	IF(OPENCL_BASE_DIR)
		#AMD adds an extras slash at the end of the ATISTREAMSDKROOT variable
		SET(OPENCL_INCLUDES ${OPENCL_BASE_DIR}/include CACHE PATH "OpenCL include path.")
		IF(WIN32)
			#uses the x86 for best compatibility
			SET(OPENCL_LIBRARY ${OPENCL_BASE_DIR}/lib/x86/OpenCL.lib CACHE PATH "OpenCL library path.")
		ELSE()
			#uses x86_64. no idea how to sperate between x86 and x86_64 with cmake
			#(but the most systems should use x86_64)
			SET(OPENCL_LIBRARY ${OPENCL_BASE_DIR}/lib/x86_64/libOpenCL.so CACHE PATH "OpenCL library path.")
		ENDIF()
		MESSAGE("FOUND AMD OpenCL")
	ELSE()
		#no amd opencl library found -> search for nvidias
		
		FIND_PATH(OPENCL_BASE_DIR OpenCL/common/inc/CL/cl.h PATH  $ENV{NVSDKCOMPUTE_ROOT} )
		IF(OPENCL_BASE_DIR)
			SET(OPENCL_INCLUDES ${OPENCL_BASE_DIR}/OpenCL/common/inc CACHE PATH "OpenCL include path.")

			IF(WIN32)
				#uses the x86 for best compatibility
				SET(OPENCL_LIBRARY ${OPENCL_BASE_DIR}/OpenCL/common/lib/Win32/OpenCL.lib CACHE PATH "OpenCL library path.")
			ELSE()
				#uses 
				SET(OPENCL_LIBRARY ${OPENCL_BASE_DIR}/lib/Linux32/libOpenCL.so CACHE PATH "OpenCL library path.")
			ENDIF()
			
			MESSAGE("FOUND NVIDIA OpenCL")
		ELSE()
			
			SET(OPENCL_INCLUDES OPENCL_INCLUDES-NOTFOUND CACHE PATH "OpenCL include path.")
			SET(OPENCL_LIBRARY OPENCL_LIBRARY-NOTFOUND CACHE PATH "OpenCL library path to the .a, .so or .lib file.")
		ENDIF()
	ENDIF()

SUBDIRS("opensurfcl" "demo")


SET(WRAPPER_CSHARP CACHE BOOL "Creates Wrappercode for CSharp on/off")
SET(WRAPPER_JAVA CACHE BOOL "Creates Wrappercode for Java on/off")
SET(UNIT_TESTS CACHE BOOL "Turn unit testing on/off")
IF(WRAPPER_CSHARP)
	add_subdirectory("csharpwrap")
ENDIF()
IF(WRAPPER_JAVA)
	add_subdirectory("javawrap")
ENDIF()
IF(UNIT_TESTS)
	add_subdirectory("test")
ENDIF()

SET(BENCHMARK CACHE BOOL "Benchmark on/off")
IF(BENCHMARK)
	add_subdirectory("benchmark")
ENDIF()