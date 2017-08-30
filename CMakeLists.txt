cmake_minimum_required(VERSION 3.5)
project(whistle)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
  Source/Main.cpp
  Source/WhistleLabApplication.cpp
  Source/WhistleLabApplication.hpp
  Source/Detector/AHDetector.cpp
  Source/Detector/AHDetector.hpp
  Source/Detector/BembelbotsDetector.cpp
  Source/Detector/BembelbotsDetector.hpp
  Source/Detector/EvaluationHandle.cpp
  Source/Detector/EvaluationHandle.hpp
  Source/Detector/HULKsDetector.cpp
  Source/Detector/HULKsDetector.hpp
  Source/Detector/NaoDevilsDetector.cpp
  Source/Detector/NaoDevilsDetector.hpp
  Source/Detector/UNSWDetector.cpp
  Source/Detector/UNSWDetector.hpp
  Source/Detector/WhistleDetector.hpp
  Source/Detector/WhistleDetectorBase.cpp
  Source/Detector/WhistleDetectorBase.hpp
  Source/Detector/WhistleDetectorFactory.hpp
  Source/Detector/WhistleDetectorFactoryBase.cpp
  Source/Detector/WhistleDetectorFactoryBase.hpp
  Source/Engine/AudioChannel.cpp
  Source/Engine/AudioChannel.hpp
  Source/Engine/AudioFile.cpp
  Source/Engine/AudioFile.hpp
  Source/Engine/SampleDatabase.cpp
  Source/Engine/SampleDatabase.hpp
  Source/Engine/WhistleLabel.cpp
  Source/Engine/WhistleLabel.hpp
  Source/Engine/WhistleLabEngine.cpp
  Source/Engine/WhistleLabEngine.hpp
  Source/UI/LabelWidget.cpp
  Source/UI/LabelWidget.hpp
  Source/UI/MainWindow.cpp
  Source/UI/MainWindow.hpp
  Source/UI/SampleDatabaseWidget.cpp
  Source/UI/SampleDatabaseWidget.hpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

add_executable(whistle ${SOURCES})
target_compile_options(whistle PRIVATE -std=c++14 -Wall -Wextra -Wconversion -pedantic
  -Werror -pedantic-errors)
target_include_directories(whistle PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(whistle -lsndfile)
target_link_libraries(whistle -lfftw3 -lfftw3f)
target_link_libraries(whistle -lfann)
target_link_libraries(whistle Qt5::Widgets Qt5::Multimedia)
