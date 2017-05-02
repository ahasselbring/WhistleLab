cmake_minimum_required(VERSION 3.5)
project(whistle)

set(SOURCES
  Source/Main.cpp
  Source/SampleDatabase.cpp
  Source/SampleDatabase.hpp
)

add_executable(whistle ${SOURCES})
target_compile_options(whistle PUBLIC -std=c++14 -Wall -Wextra -pedantic
  -Werror -pedantic-errors)
target_link_libraries(whistle -lsndfile)
find_package(Boost COMPONENTS filesystem REQUIRED)
target_link_libraries(whistle Boost::filesystem)
