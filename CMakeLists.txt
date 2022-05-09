cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(bunny_simulator VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug 
    CACHE STRING 
      "Choose the type of build (Debug or Release)" 
      FORCE
    )
endif()

# Generate config.h
configure_file(config.h.in config.h)

# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
  )

file(COPY resources DESTINATION /)

include_directories(src/inc)

set(SRC
  src/util.cpp
  src/tile_map.cpp
  src/bunny_names.cpp
  src/bunny.cpp
  src/path_finding.cpp
  src/bunny_manager.cpp
  src/main.cpp
)

# Compile executable
add_executable(${PROJECT_NAME} ${SRC})

# Set include directory search paths
target_include_directories(${PROJECT_NAME} 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )

# Link executable to required SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

