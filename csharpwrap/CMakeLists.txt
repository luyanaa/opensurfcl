
#Suche die SWIG files
FIND_PACKAGE(SWIG)


IF( SWIG_FOUND )
	#Einbinden der files von Swig
	INCLUDE(${SWIG_USE_FILE})

	INCLUDE_DIRECTORIES(${OPENSURFCL_SOURCE_DIR}/opensurfcl ${OPENCL_INCLUDES})
	#SET(CMAKE_SWIG_OUTDIR ${OPENSURFCL_SOURCE_DIR}/CsOpenSurfCL CACHE PATH "")
	SET(CMAKE_SWIG_OUTDIR ${OPENSURFCL_SOURCE_DIR}/CsOpenSurfCLWrap/CsOpenSurfCLWrap)
	
	SET_SOURCE_FILES_PROPERTIES(CsOpenSurfCL.i PROPERTIES CPLUSPLUS ON)
	SET_SOURCE_FILES_PROPERTIES(CsOpenSurfCL.i PROPERTIES SWIG_FLAGS "")
	SWIG_ADD_MODULE(CsOpenSurfCL csharp CsOpenSurfCL.i)
	SWIG_LINK_LIBRARIES(CsOpenSurfCL OpenSurfCL)
	
	#Kopieren der erstellten .dll in den Ordner des Javaprojektes
	add_custom_command(TARGET CsOpenSurfCL
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy
					 ARGS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/CsOpenSurfCL.dll ${OPENSURFCL_SOURCE_DIR}/CsOpenSurfCLWrap/CsOpenSurfCLWrap)

ELSE()
	MESSAGE("No swig or java or jni found. Project of wrapper JOpenSurfCL will be ignored.")
ENDIF()
