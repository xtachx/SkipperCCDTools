cmake_minimum_required(VERSION 3.1)
project( SkReconstruction )

#Assuming Mac owners use macports!
if(APPLE)
    set(CMAKE_C_COMPILER "/opt/local/bin/gcc-mp-9")
    set(CMAKE_CXX_COMPILER "/opt/local/bin/g++-mp-9")
endif(APPLE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
find_package( CFITSIO )
set(OpenCV_DIR "/home/pitam/opencv/build")
find_package(OpenCV REQUIRED)


set (CMAKE_CXX_FLAGS "-std=gnu++1z -O3 -march=native -m64 -lstdc++ -g")
set (CMAKE_EXE_LINKER_FLAGS "-g")



set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/CoreImageOps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ComputeMeans.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LoadSaveFITS/FITSFileOperations.cpp
   )
set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/CoreImageOps.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CLI11.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ProgressBar.hpp
    )


add_executable( DAMICMRecon SKChargeAndNoise.cpp ${SOURCE} ${HEADERS})
target_include_directories(DAMICMRecon PUBLIC ${CFITSIO_INCLUDE_DIR})
target_link_libraries( DAMICMRecon ${OpenCV_LIBS} ${CFITSIO_LIBRARIES} stdc++fs)
