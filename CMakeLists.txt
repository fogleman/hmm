# *********************************
# ************ cmake **************
# *********************************
CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
CMAKE_POLICY(SET CMP0043 NEW)
CMAKE_POLICY(SET CMP0048 NEW)

# Enable debug symbols by default
# must be done before project() statement
SET(CMAKE_BUILD_TYPE_INIT Release) 
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Debug)

# *********************************
# ************ Project ************
# *********************************

STRING(TIMESTAMP VERSION %y.%U.%w.%H UTC)

MESSAGE(STATUS "HMM - HeightMap Meshing utility V${VERSION}")

PROJECT(hmm VERSION ${VERSION})

INCLUDE(GNUInstallDirs)

# ======== For config.h.in =========
SET(PACKAGE_MAINTAINER "fogleman")
#SET(PACKAGE_BUGREPORT  "")
SET(PACKAGE_URL        "https://github.com/fogleman/hmm")

FIND_PROGRAM(LSB_RELEASE_EXE NAMES lsb_release)
IF (DEFINED LSB_RELEASE_EXE)
   EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_EXE} -d OUTPUT_VARIABLE LSB_RELEASE OUTPUT_STRIP_TRAILING_WHITESPACE)
   MESSAGE(STATUS ${LSB_RELEASE})
ENDIF()

# *********************************
# ********* Requirements **********
# *********************************

# =========== pkg-config ============
FIND_PACKAGE(PkgConfig)
FIND_PACKAGE(PackageHandleStandardArgs)

# =========== libraries =============

#SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# *********************************
# *********** Compiler ************
# *********************************

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -flto -Wall -Winit-self -Wpointer-arith \
         -Wcast-align -Wformat=2 -Woverloaded-virtual \
         -Wnon-virtual-dtor -Wshadow -Wno-unknown-pragmas")

#SET(CMAKE_CXX_FLAGS
#	"${CMAKE_CXX_FLAGS} \
#         )

# -------------------------------------------------------
# Inlclude the main source AND build dir itself
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# ********************************************************
# ********************* Resources ************************
# ********************************************************

INCLUDE(CheckIncludeFileCXX)

CHECK_INCLUDE_FILE_CXX(glm/glm.hpp FOUND_GLM)

IF(NOT FOUND_GLM)
	MESSAGE(FATAL_ERROR "glm/glm.hpp can not found, can not be included")
ENDIF()

ADD_EXECUTABLE(hmm
	src/main.cpp
        src/heightmap.cpp
        src/triangulator.cpp
        src/base.cpp
        src/blur.cpp
        src/stl.cpp)

# *********************************
# ********** Installation *********
# *********************************

#SET(PKG_SHARE    "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")
#SET(PKG_APPS     "${CMAKE_INSTALL_DATAROOTDIR}/applications")
#SET(PKG_PIXMAPS  "${CMAKE_INSTALL_DATAROOTDIR}/pixmaps")

INSTALL(TARGETS hmm DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# *********************************
# ************ Packaging **********
# *********************************

# ======== General packaging ======
# https://cmake.org/cmake/help/latest/module/CPack.html

INCLUDE(InstallRequiredSystemLibraries)

# SET(CPACK_STRIP_FILES ON) # Strip debug sysmbold from files

SET(CPACK_PACKAGE_VERSION       "${hmm_VERSION}")
SET(CPACK_PACKAGE_CONTACT       "${PACKAGE_BUGREPORT}")
SET(CPACK_PACKAGE_MAINTAINER    "${PACKAGE_MAINTAINER}")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
#SET(CPACK_PACKAGE_VENDOR "Me, myself, and I")

#SET(CPACK_SOURCE_IGNORE_FILES "/old/;/build/;/[.].*/;/autom4te[.]cache/;.*~;.log$")

# Maybe the text displayed in synaptic?
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HMM - HeightMap Meshing utilitiy")
#CPACK_RESOURCE_FILE_README     ReadMe file to be embedded in the installer.

SET(CPACK_GENERATOR        TBZ2 DEB) # TGZ
SET(CPACK_SOURCE_GENERATOR TBZ2)     # TGZ
#SET(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/Package.txt)
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")


# ======== Debian packaging =======
# https://cmake.org/cmake/help/latest/module/CPackDeb.html
# https://www.debian.org/doc/debian-policy

#SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libao4 (>=1.0), libsfm-system2.3v5 (>=2.3), libsfml-graphicsv2.3v5 (>=2.3), libsfml-audio2.3v5 (>=2.3), libsfm-window2.3v5 (>2.3)")
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS 1) # generate better (atomatic?) dependecies
SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")
SET(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
SET(CPACK_DEBIAN_PACKAGE_SECTION utils)
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PACKAGE_URL}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION
"${CPACK_PACKAGE_DESCRIPTION_SUMMARY}
hmm is a modern implementation of a nice algorithm from the 1995 paper
Fast Polygonal Approximation of Terrains and Height Fields by Garland
and Heckbert. The meshes produced by hmm satisfy the Delaunay condition
and can satisfy a specified maximal error or maximal number of
triangles or vertices. It's also very fast.")

#CPACK_INSTALL_CMAKE_PROJECTS
#List of four values that specify what project to install. Build directory, Project Name, Project Component, Directory.

INCLUDE(CPack)
