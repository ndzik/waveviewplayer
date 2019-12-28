project (JRWVP)
cmake_minimum_required (VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
set (wxWidgets_CONFIG_EXECUTABLE /usr/bin/wx-config-gtk3)

# for youcompleteme
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

### GLOBBING ###
file (GLOB_RECURSE sources src/*.cpp)
file (GLOB_RECURSE includes include/*.h)

### TARGET DEFINITIONS ###
add_executable (WaveViewPlayer ${sources})

# compiler options
target_compile_options (WaveViewPlayer PUBLIC -Wall)

target_include_directories (WaveViewPlayer PUBLIC include)

### DEPENDENCIES ###

# tinyxml2 lib
# INTERFACE : We are not allowed to provide source files since this lib is
#             is not meant to generate any build output.

# finding specific packages
# find_package (Boost 1.3.6.0 COMPONENTS filesystem system REQUIRED)

# target_link_libraries (LearnCMake PUBLIC
#    ${Boost_LIBRARIES}
    # here one can add any library dependencies
#   )

find_package (wxWidgets COMPONENTS all REQUIRED)
include (${wxWidgets_USE_FILE})

find_library (lib_mpg123 mpg123)
find_library (lib_tinyxml2 tinyxml2)

IF (NOT lib_tinyxml2)
  include(ExternalProject)
  MESSAGE (STATUS "tinyxml2 not found, trying to install tinyxml2...")
  ExternalProject_Add(tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2-latest)
ENDIF ()

target_link_libraries(WaveViewPlayer ${wxWidgets_LIBRARIES} ${lib_mpg123}
  ${lib_tinyxml2})

### PACKAGING ###
install (TARGETS WaveViewPlayer DESTINATION builds/default)

# Package name
set (CPACK_PACKAGE_NAME "JRWaveViewPlayer")
set (CPACK_PACKAGE_VERSION "1.0.0")

# We want a single program
set (CPACK_MONOLITHIC_INSTALL 1)

include (CPack)
