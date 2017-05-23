cmake_minimum_required(VERSION 3.5)
project(whistle)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
  Source/Main.cpp
  Source/SampleDatabase.cpp
  Source/SampleDatabase.hpp
  Source/SampleProvider.cpp
  Source/SampleProvider.hpp
  Source/Detector/HULKsDetector.cpp
  Source/Detector/HULKsDetector.hpp
  Source/Detector/WhistleDetector.hpp
  Source/Detector/WhistleDetectorBase.cpp
  Source/Detector/WhistleDetectorBase.hpp
  Source/Detector/WhistleDetectorFactory.hpp
  Source/Detector/WhistleDetectorFactoryBase.cpp
  Source/Detector/WhistleDetectorFactoryBase.hpp
  Source/Engine/WhistleLabel.cpp
  Source/Engine/WhistleLabel.hpp
  Source/Engine/WhistleLabEngine.cpp
  Source/Engine/WhistleLabEngine.hpp
  Source/UI/MainWindow.cpp
  Source/UI/MainWindow.hpp
  Source/UI/SampleDatabaseWidget.cpp
  Source/UI/SampleDatabaseWidget.hpp
)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(whistle ${SOURCES})
target_compile_options(whistle PRIVATE -std=c++14 -Wall -Wextra -pedantic
  -Werror -pedantic-errors)
target_include_directories(whistle PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(whistle -lsndfile)
target_link_libraries(whistle -lfftw3)
target_link_libraries(whistle -lyaml-cpp)
target_link_libraries(whistle Boost::filesystem)
target_link_libraries(whistle Qt5::Widgets)
