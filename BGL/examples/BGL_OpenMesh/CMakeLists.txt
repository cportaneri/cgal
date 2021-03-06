# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

project( BGL_OpenMesh_Examples )


cmake_minimum_required(VERSION 3.1)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()

# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

find_package( OpenMesh QUIET )

if ( OpenMesh_FOUND )
include( UseOpenMesh )
else()

  message(STATUS "NOTICE: These examples require OpenMesh and will not be compiled.")
  return()

endif()

# include for local directory

# include for local package

# Creating entries for all C++ files with "main" routine
# ##########################################################

if(OpenMesh_FOUND)
  create_single_source_cgal_program( "TriMesh.cpp" )
  target_link_libraries( TriMesh PRIVATE ${OPENMESH_LIBRARIES} )
endif()

