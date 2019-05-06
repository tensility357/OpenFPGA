cmake_minimum_required(VERSION 2.8.12)

if (${CMAKE_VERSION} VERSION_GREATER "3.8")
    #For cmake >= 3.9 INTERPROCEDURAL_OPTIMIZATION behaviour we need to explicitly
    #set the cmake policy version number
    cmake_policy(VERSION 3.9) 

    # If we are using verison < 3.9 then setting INTERPROCEDURAL_OPTIMIZATION
    # has no effect unless an Intel compiler is used
endif()

# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build: None, Debug, Release, RelWithDebInfo, MinSizeRel"
        FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#Create the project 
project("OPENFPGA" C CXX)

# Options
# Option to enable/disable graphic in compilation
option(ENABLE_VPR_GRAPHICS "Enables VPR graphics" ON)

# Version number
set(OPENFPGA_VERSION_MAJOR 1)
set(OPENFPGA_VERSION_MINOR 0)
set(OPENFPGA_VERSION_PATCH 0)
set(OPENFPGA_VERSION_PRERELEASE "dev")

# Include user-defined functions
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(FilesToDirs)


# compiler flag configuration checks
include(CheckCXXCompilerFlag)

# Set WARN FLAGS
set(WARN_FLAGS "")

if (MSVC)
  # Visual studio warnings 
  # Note that we do not use /Wall since it generates warnings about standard library headers
  set(WARN_FLAGS_TO_CHECK # the flags to check if the compiler supports
      "/W4" # Most warnings
     )
else ()
  set(WARN_FLAGS_TO_CHECK # the flags to check if the compiler supports
      #GCC-like
      "-Wpointer-arith"
      "-Wcast-qual"
      "-D__USE_FIXED_PROTOTYPES__"
      "-ansi"
      "-Wshadow"
      "-Wcast-allign"
      "-Wno-write-strings"
      "-D_POSIX_SOURCE"
      "-Wall"                         #Most warnings, typically good
      "-Wextra"                       #Extra warning, usually good
      "-Wpedantic"                    #Ensure ISO compliance (i.e. no non-standard extensions)
      "-Wcast-qual"                   #Warn if cast removes qualifier (e.g. const char* -> char*)
      "-Wcast-align"                  #Warn if a cast causes memory alignment changes
      "-Wshadow"                      #Warn if local variable shadows another variable
      "-Wformat=2"                    #Sanity checks for printf-like formatting
      "-Wno-format-nonliteral"        # But don't worry about non-literal formtting (i.e. run-time printf format strings)
      "-Wlogical-op"                  #Checks for logical op when bit-wise expected
      "-Wmissing-declarations"        #Warn if a global function is defined with no declaration
      "-Wmissing-include-dirs"        #Warn if a user include directory is missing
      "-Wredundant-decls"             #Warn if there are overlapping declarations
      "-Wswitch-default"              #Warn if a switch has no default
      "-Wundef"                       #Warn if #if() preprocessor refers to an undefined directive
      "-Wunused"                      #Warn about unused variables/parameters
      "-Wunused-variable"             #Warn about variables that are not used
      "-Wunused-parameter"            #Warn about function parameters which are unused
      "-Wdisabled-optimization"       #Warn when optimizations are skipped (usually due to large/complex code)
      "-Wnoexcept"                    #Warn when functions should be noexcept (i.e. compiler know it doesn't throw)
      "-Woverloaded-virtual"          #Warn when a function declaration overrides a virtual method
      "-Wctor-dtor-privacy"           #Warn about inaccessible constructors/destructors
      "-Wnon-virtual-dtor"            #Warn about missing virtual destructors
      "-Wduplicated-cond"             #Warn about identical conditions in if-else chains
      "-Wduplicated-branches"         #Warn when different branches of an if-else chain are equivalent
      "-Wnull-dereference"            #Warn about null pointer dereference execution paths
      "-Wuninitialized"               #Warn about unitialized values
      "-Winit-self"                   #Warn about self-initialization
      "-Wcatch-value=3"               #Warn when catch statements don't catch by reference
"-Wextra-semi" #Warn about redudnant semicolons
      )
endif()

# check and see if the compiler supports the various warning flags
# and add valid flags
foreach (flag ${WARN_FLAGS_TO_CHECK})
  CHECK_CXX_COMPILER_FLAG(${flag} CXX_COMPILER_SUPPORTS_${flag})
  if (CXX_COMPILER_SUPPORTS_${flag})
    # flag supported, so enable it
    set (WARN_FLAGS "${WARN_FLAGS} ${flag}")
  endif()
endforeach()
#
# Sanitizer flags
#

set(SANITIZE_FLAGS "")
if(OPENFPGA_ENABLE_SANITIZE)
    #Enable sanitizers
    # -fuse-ld=gold force the gold linker to be used (required for sanitizers, but not enabled by default on some systems)
    set(SANITIZE_FLAGS "-g -fsanitize=address -fsanitize=leak -fsanitize=undefined -fuse-ld=gold")
    message(STATUS "SANTIIZE_FLAGS: ${SANITIZE_FLAGS}")
    link_libraries("-static-libasan") #Fixes 'ASan runtime does not come first in initial library list'
endif()

# Set final flags
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN_FLAGS} ${SANITIZE_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")


# Unit Testing
#
enable_testing()

# 
# Sub-projects
#
add_subdirectory(yosys)
add_subdirectory(abc)
add_subdirectory(ace2)
add_subdirectory(vpr7_x2p)

# run make to extract compiler options, linker options and list of source files
#add_custom_target(
#  yosys
#  COMMAND make run
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yosys
#)

# Add extra compilation flags to suppress warnings from some libraries/tools
# Note that target_compile_options() *appends* to the current compilation options of
# the specified target

#Since ABC is an externally developed tool, we suppress all compiler warnings
CHECK_CXX_COMPILER_FLAG("-w" CXX_COMPILER_SUPPORTS_-w)
if(CXX_COMPILER_SUPPORTS_-w)
    target_compile_options(libabc PRIVATE "-w")
    target_compile_options(abc PRIVATE "-w")
endif()

#Some ABC headers generate warnings, treat them as system headers to suppress warnings
get_property(ABC_INCLUDE_DIRS TARGET libabc PROPERTY INCLUDE_DIRECTORIES)
target_include_directories(libabc SYSTEM INTERFACE ${ABC_INCLUDE_DIRS})

# We use Yosys and abc Makefile now, TODO: create CMakeLists.txt as well
set_target_properties(libabc abc
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/abc"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/abc"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/abc")

# Set output locations to be in the main source tree under the relevant folder
set_target_properties(libace ace
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ace2"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ace2"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ace2")


# Set output locations to be in the main source tree under the relevant folder
set_target_properties(libarchfpga read_arch
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libarchfpga"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libarchfpga"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libarchfpga")

set_target_properties(libpcre pcredemo
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libpcre"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libpcre"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libpcre")

set_target_properties(libprinthandler printhandlerdemo
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libprinthandler"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libprinthandler"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/libprinthandler")

set_target_properties(libvpr vpr
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/vpr"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/vpr"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vpr7_x2p/vpr")


