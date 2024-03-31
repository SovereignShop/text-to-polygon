cmake_minimum_required(VERSION 3.14)
project(TextToPolygon VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(TextToPolygon STATIC
  src/text_to_polygon.cpp
)

target_include_directories(TextToPolygon PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

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
FetchContent_MakeAvailable(freetype2)

target_link_libraries(TextToPolygon PRIVATE freetype)

target_include_directories(TextToPolygon PUBLIC
  $<BUILD_INTERFACE:${freetype2_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(TextToPolygon PUBLIC
  $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
