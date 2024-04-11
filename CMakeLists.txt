# Require CMake 3.15+ (matching scikit-build-core) Use new versions of all
# policies up to CMake 3.27
cmake_minimum_required(VERSION 3.15...3.27)

# Scikit-build-core sets these values for you, or you can just hard-code the
# name and version.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)
 
 set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find the module development requirements (requires FindPython from 3.17 or
# scikit-build-core's built-in backport)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# opencv-devel needs to be installed on the target machine
find_package(OpenCV 4.5 REQUIRED COMPONENTS core imgproc imgcodecs)

# Qt-devel needs to be installed on the target machine
find_package(Qt6 REQUIRED Core Gui)

find_package(macs_io REQUIRED)

# Add a library using FindPython's tooling (pybind11 also provides a helper like
# this)
Python3_add_library(macs_io_python src/macs_io_python.cpp)
target_link_libraries(macs_io_python pybind11::headers Python3::Python Qt6::Core Qt6::Gui ${OpenCV_LIBS} macs_io)
set_target_properties(macs_io_python PROPERTIES
			VISIBILITY_INLINES_HIDDEN ON
			OUTPUT_NAME macs_io_python)

# This is passing in the version as a define just as an example
target_compile_definitions(macs_io_python VERSION_INFO=${PROJECT_VERSION})

# The install directory is the output (wheel) directory
install(TARGETS macs_io_python DESTINATION macs_io_python)
