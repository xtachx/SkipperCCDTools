cmake_minimum_required(VERSION 3.1)
project( SkReconstruction )
find_package(OpenCV REQUIRED)

#Assuming Mac owners use macports!
if(APPLE)
    set(CMAKE_C_COMPILER "/opt/local/bin/gcc-mp-9")
    set(CMAKE_CXX_COMPILER "/opt/local/bin/g++-mp-9")
endif(APPLE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
find_package( CFITSIO )

#set(ENV{ROOTSYS} "/home/pitam/ROOT6")
#find_package(ROOT)

set (CMAKE_CXX_FLAGS "-Wno-write-strings -std=gnu++1y -O3 -march=native -m64 -lstdc++ -g")
set (CMAKE_EXE_LINKER_FLAGS "-g -Wno-write-strings -lRIO -lCore -lNet -lHist -lSpectrum -lGraf -lGraf3d -lGpad -lTree -lTreePlayer -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -rdynamic -lpthread -lz -lm -ldl -lfftw3")



set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/CoreImageOps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ComputeMeans.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UtilityFunctions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LoadSaveFITS/FITSFileOperations.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/SKFFTViewer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SKChargeAndNoise.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/DarkCurrentModels.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AnalyzeCCDSpectra.cpp
   )
set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/CoreImageOps.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UtilityFunctions.hpp
        #${CMAKE_CURRENT_SOURCE_DIR}/SKFFTViewer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ROOTIncludes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SKChargeAndNoise.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ProcessingParams.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PaulTolColours.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/NicerPlots.hpp
        )




add_executable( SKChargeAndNoise SKChargeAndNoise.cpp ${SOURCE} ${HEADERS})
target_include_directories(SKChargeAndNoise PUBLIC ${CFITSIO_INCLUDE_DIR} "/usr/include/root/")
target_link_directories(SKChargeAndNoise PUBLIC "/usr/lib64/root/")
target_link_libraries( SKChargeAndNoise ${OpenCV_LIBS} ${CFITSIO_LIBRARIES} stdc++fs)
