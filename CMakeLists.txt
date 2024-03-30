cmake_minimum_required(VERSION 3.14)
project(TextToPolygon VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        origin/master
)
FetchContent_MakeAvailable(glm)

# FreeType
FetchContent_Declare(
  freetype2
  URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
)
FetchContent_GetProperties(freetype2)
if(NOT freetype2_POPULATED)
  FetchContent_Populate(freetype2)
  add_subdirectory(${freetype2_SOURCE_DIR} ${freetype2_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(freetype2_INCLUDE_DIRS ${freetype2_SOURCE_DIR}/include)
set(freetype2_LIBRARY ${freetype2_BINARY_DIR}/libfreetype.a)

add_executable(TextToPolygon main.cpp)
target_link_libraries(TextToPolygon PRIVATE ${freetype2_LIBRARIES})
target_include_directories(TextToPolygon PRIVATE ${freetype2_INCLUDE_DIRS})

# If GLM was a traditional CMake project and provided targets, it would be linked like this
target_link_libraries(TextToPolygon PRIVATE glm freetype)
