# CMakeList.txt : CMake project for GameOfLife, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GameOfLife")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Indicar dónde está SFML dentro del proyecto
set(SFML_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-2.5.1/install/lib/cmake/SFML")

add_subdirectory(Grid)
# Buscar SFML, versión 2.5 o superior, y los componentes que usás
find_package(SFML 2.5.1 COMPONENTS graphics window audio REQUIRED)


add_executable(GameOfLife GameOfLife.cpp GameOfLife.h "Grid/Grid.h" "Grid/Grid.cpp")

target_link_libraries(GameOfLife PRIVATE sfml-graphics sfml-window sfml-audio sfml-system)
target_link_libraries(GameOfLife PRIVATE Grid)
set_property(TARGET GameOfLife PROPERTY CXX_STANDARD 20)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameOfLife PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
