cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(LibsIIPCT LANGUAGES CXX)

set(COMPILE_WIN_64 OFF)

if(NOT DEFINED COMPILE_ARM)
    set(COMPILE_ARM OFF)
    set(USE_OPENMP OFF)
endif()

if((NOT COMPILE_ARM) AND (NOT COMPILE_WIN_64))
   add_definitions(-DSTART_UP)
endif()

if(USE_OPENMP)
    set(CMAKE_CXX_FLAGS "-fopenmp")
endif()

message("COMPILE_ARM is set ${COMPILE_ARM}!")
message("USE_OPENMP is set ${USE_OPENMP}!")

if(NOT DEFINED DEFAULT_PATH_ZLIB)
    set(DEFAULT_PATH_ZLIB OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(COMPILE_ARM)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_CROSSCOMPILING ON)
    set(ARM_PLATFORM "32")
    set(CROSS_COMPILE_LIBS "/home/shockzort/work/repos/Release/PRIBOR/arm_libs/${ARM_PLATFORM}")

    set(CMAKE_FIND_ROOT_PATH "${CROSS_COMPILE_LIBS}")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    include_directories("${CMAKE_FIND_ROOT_PATH}/include")

    set(FFTW_INCLUDE_DIR "${CMAKE_FIND_ROOT_PATH}/include/fftw3")
    set(FFTW_LIBRARY_DIR "${CMAKE_FIND_ROOT_PATH}/lib")

    set(BOOST_PATH "${CMAKE_FIND_ROOT_PATH}/lib")

    include_directories(${FFTW_INCLUDE_DIR})

    if(${ARM_PLATFORM} MATCHES "32")
        set(CMAKE_CXX_FLAGS "-mfloat-abi=hard  -mfpu=neon  -march=armv7-a+mp+simd")
    endif()
    message("Using arm ${ARM_PLATFORM} bit libraries")

    if(USE_OPENMP)
        set(BUILD_PATH "${PROJECT_SOURCE_DIR}/../build_arm/${ARM_PLATFORM}/OMP")
    else()
         set(BUILD_PATH "${PROJECT_SOURCE_DIR}/../build_arm/${ARM_PLATFORM}/NO_OMP")
    endif()
else()
    if(COMPILE_WIN_64)
        message("COMPILE_WIN_64 is set ${COMPILE_WIN_64}!")
        set(CMAKE_FIND_ROOT_PATH "d:/Work/Projects/KPP_UZGA/kpp_libs/win")

        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(ZLIB_INCLUDE_DIR "${CMAKE_FIND_ROOT_PATH}/zlib/include")
        set(ZLIB_LIBRARY "${CMAKE_FIND_ROOT_PATH}/zlib/lib")

        set(JPEG_INCLUDE_DIR "${CMAKE_FIND_ROOT_PATH}/LibJpeg")
        set(JPEG_LIBRARY "${CMAKE_FIND_ROOT_PATH}/LibJpeg/libjpeg/lib64")

        set(BOOST_PATH "${CMAKE_FIND_ROOT_PATH}/boost")

        include_directories(${CMAKE_FIND_ROOT_PATH})
        include_directories(${CMAKE_FIND_ROOT_PATH}/boost/include/boost-1_74)
        include_directories("${CMAKE_FIND_ROOT_PATH}/libzip/include")
        include_directories("${CMAKE_FIND_ROOT_PATH}/fftw-3.3.4-dll64")
    else()       
        if(NOT DEFINED LIBS_PATH)
            if(NOT DEFAULT_PATH_ZLIB)
                set(LIBS_PATH "${PROJECT_SOURCE_DIR}/../kpp_libs/linux")
                set(OpenCV_DIR "${LIBS_PATH}/OpenCV_4.5.0/")
                set(ZLIB_ROOT "${LIBS_PATH}/zlib/")
                set(LIBZIP_DIR "${LIBS_PATH}/libzip/")
            endif()
        endif()
    endif()
    
    set(CMAKE_CXX_FLAGS "-march=native -mtune=intel") # -fopt-info-vec
    if(USE_OPENMP)
        set(BUILD_PATH "${PROJECT_SOURCE_DIR}/../build_desktop/OMP")
    else()
         set(BUILD_PATH "${PROJECT_SOURCE_DIR}/../build_desktop/NO_OMP")
    endif()
endif()

set(RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}")
set(LIBRARY_OUTPUT_DIRECTORY "${BUILD_PATH}")
set(ARCHIVE_OUTPUT_DIRECTORY "${BUILD_PATH}")

add_subdirectory(Primitives)
add_subdirectory(Algorithms)
add_subdirectory(KeyPoints)
add_subdirectory(ImageProcessing)
add_subdirectory(MachineLearning)
add_subdirectory(Utility)
add_subdirectory(Tracking)

if((NOT COMPILE_ARM) AND (NOT COMPILE_WIN_64))
    if(USE_NetWorker)
        add_subdirectory(NetWorker)
    endif()

    add_subdirectory(UAC)
    add_subdirectory(VideoProcessing)

    if(USE_SIMPLE_BGS)
        add_subdirectory(Simple_BGS)
    endif()
endif()
