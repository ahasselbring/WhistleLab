cmake_minimum_required(VERSION 3.5)
project(whistle)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(SOURCES
  Source/Main.cpp
  Source/SampleDatabase.cpp
  Source/SampleDatabase.hpp
  Source/SampleProvider.cpp
  Source/SampleProvider.hpp
  Source/Detector/HULKsDetector.cpp
  Source/Detector/HULKsDetector.hpp
  Source/Detector/WhistleDetectorInterface.cpp
  Source/Detector/WhistleDetectorInterface.hpp
)

add_executable(whistle ${SOURCES})
target_compile_options(whistle PUBLIC -std=c++14 -Wall -Wextra -pedantic
  -Werror -pedantic-errors)
target_include_directories(whistle PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(whistle -lsndfile)
target_link_libraries(whistle -lfftw3)
find_package(Boost COMPONENTS filesystem REQUIRED)
target_link_libraries(whistle Boost::filesystem)
