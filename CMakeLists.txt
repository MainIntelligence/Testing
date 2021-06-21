cmake_minimum_required(VERSION 3.5)


project(Test)

SET(PROJECT_MAJOR MI)
SET(INCLUDE_DIRS Detection)

#add_subdirectory(Detection)

#INSTALL
include(GNUInstallDirs)

list(LENGTH INCLUDE_DIRS TOTAL_INCLUDES)
MATH(EXPR TOTAL_INCLUDES "${TOTAL_INCLUDES}-1")
foreach(i RANGE ${TOTAL_INCLUDES})
	list(GET INCLUDE_DIRS ${i} dir)
	install(DIRECTORY ${dir}/ 
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
		FILES_MATCHING PATTERN "*.h"
	)
endforeach()

