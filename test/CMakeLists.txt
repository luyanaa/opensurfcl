
#FILE(GLOB ALL_CPP *.cpp)
FILE(GLOB ALL_HEADER *.h)

#sagt CMAKE wo es suchen soll
SET(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} ${OPENSURFCL_SOURCE_DIR}/../lib/cxxtest)

find_package(CxxTest)
find_package(Perl)
find_package(PythonInterp)

IF( NOT PERL_FOUND AND PYTHONINTERP_FOUND)
	SET(CXXTEST_USE_PYTHON 1)
	MESSAGE("using Python")
endif()

if(CXXTEST_FOUND AND ( PYTHONINTERP_FOUND OR PERL_FOUND ))
	
	enable_testing()
	if(CXXTEST_USE_PYTHON)
		set(_cxxtest_executable ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE})
	else()
		set(_cxxtest_executable ${CXXTEST_PERL_TESTGEN_EXECUTABLE})
	endif()
	
	
	SET(TEST_DIR ${OPENSURFCL_SOURCE_DIR}/test)
	#Sucht alle .h Files im Ordner -> Alle Header in dem Ordner sollten Testsuites sein
	FILE(GLOB ALL_TESTS RELATIVE ${TEST_DIR} *.h)
	
	FOREACH (CUR_TEST ${ALL_TESTS})
		
		#Der Header wurde durch FILE(GLOB... bereits bestimmt
		SET(TEST_HEADER ${CUR_TEST})
		
		#Der Testname ist der Header ohne .h
		string(LENGTH ${CUR_TEST} CUR_TESTNAME_LEN)
		MATH(EXPR CUR_TESTNAME_LEN ${CUR_TESTNAME_LEN}-2)
		string(SUBSTRING ${CUR_TEST} 0 ${CUR_TESTNAME_LEN} TEST_NAME)
		
		#Die für den Test zu erstellende cpp ist der Testname + .cpp
		#( wird automatisch von cxxtest erstellt! )
		SET(TEST_CPP ${TEST_NAME}.cpp) # wird erstellt
		#list(APPEND ALL_TEST_CPP ${TEST_DIR}/${TEST_CPP})
		#list(APPEND ALL_TEST_HEADER ${TEST_DIR}/${TEST_HEADER})
		
		add_custom_command(
			COMMAND ${_cxxtest_executable} --error-printer -o ${TEST_DIR}/${TEST_CPP} ${TEST_DIR}/${TEST_HEADER}
			OUTPUT  ${TEST_DIR}/${TEST_CPP}
			DEPENDS ${TEST_HEADER} )
			
		set_source_files_properties(${TEST_DIR}/${TEST_CPP} PROPERTIES GENERATED true)
		
		
		include_directories( ${CXXTEST_INCLUDE_DIR} ${OPENCL_INCLUDES} ${OPENSURFCL_SOURCE_DIR}/opensurfcl)
	
		add_executable(${TEST_NAME} ${TEST_DIR}/${TEST_CPP} ${TEST_DIR}/${TEST_HEADER})
		target_link_libraries(${TEST_NAME} OpenSurfCL ${OPENCL_LIBRARY})
        #add_test(${TEST_NAME} ${_cxxtest_testname})
	ENDFOREACH(CUR_TEST)
	
	
else()
	MESSAGE("Unittests ignored. No Python or Pearl found.")
endif()



