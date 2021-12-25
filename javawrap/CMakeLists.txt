
#Suche die SWIG files
FIND_PACKAGE(SWIG)
FIND_PACKAGE(Java)
FIND_PACKAGE(JNI)


IF( SWIG_FOUND AND JAVA_FOUND  AND JNI_FOUND )
	#Einbinden der files von Swig
	INCLUDE(${SWIG_USE_FILE})
	INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS} ${OPENSURFCL_SOURCE_DIR}/opensurfcl ${OPENCL_INCLUDES})

	#Ausgabeort der *.java files ins src verzeichnisses des Javaproject
	SET(CMAKE_SWIG_OUTDIR ${OPENSURFCL_SOURCE_DIR}/JOpenSurfCLWrap/src)
	
	SET_SOURCE_FILES_PROPERTIES(JOpenSurfCL.i PROPERTIES CPLUSPLUS ON)
	SET_SOURCE_FILES_PROPERTIES(JOpenSurfCL.i PROPERTIES SWIG_FLAGS "")
	SWIG_ADD_MODULE(JOpenSurfCL java JOpenSurfCL.i)
	SWIG_LINK_LIBRARIES(JOpenSurfCL OpenSurfCL ${JAVA_LIBRARIES})
	
	#Kopieren der Erstellten .dll in den Ordner des Javaprojektes
	add_custom_command(TARGET JOpenSurfCL
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy
					 ARGS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/JOpenSurfCL.dll ${OPENSURFCL_SOURCE_DIR}/JOpenSurfCLWrap)
ELSE()
	MESSAGE("No swig or java or jni found. Project of wrapper JOpenSurfCL will be ignored.")
ENDIF()
