# Original file Copyright CMU462 Fall 2015: 
# Kayvon Fatahalian, Keenan Crane,
# Sky Gao, Bryce Summers, Michael Choquette.
cmake_minimum_required(VERSION 2.8)
project(HW0)

#-------------------------------------------------------------------------------
# Build options
#-------------------------------------------------------------------------------
option(BUILD_LIBCGL "Build with libCGL" ON)
option(BUILD_DEBUG "Build with debug settings" OFF)
option(BUILD_DOCS "Build documentation" OFF)
option(OpenMP "Set OpenMP" ON)
#-------------------------------------------------------------------------------
# Platform-specific settings
#-------------------------------------------------------------------------------

###################
# Building on OSX #
###################
if (APPLE)

    # OSX Framework dependencies
    if (NOT BUILD_LIBCGL)
        include_directories("/System/Library/Frameworks")
        find_library(COCOA_LIBRARIES Cocoa)
        find_library(IOKIT_LIBRARIES IOkit)
        find_library(COREVIDEO_LIBRARIES CoreVideo)
    endif ()

    # Clang configuration
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        if (OpenMP)
            set(CLANG_CXX_FLAGS "-std=c++11 -m64 -fopenmp")
        else(OpenMP)
            set(CLANG_CXX_FLAGS "-std=c++11 -m64")
        endif(OpenMP)

        if (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Debug)
            set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -g")
        else (BUILD_DEBUG)
            set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -O3")
            set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -funroll-loops")
            set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -Wno-narrowing")
            set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -Wno-deprecated-register")
#            set(CLANG_CXX_FLASG "${CLANG_CXX_FLAGS} -Wdeprecated-declarations")
        endif (BUILD_DEBUG)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_CXX_FLAGS}")

    endif ()

    # GCC configuration
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        set(GCC_CXX_FLAGS "-std=gnu++11 -m64")

        if (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Debug)
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -g")
        else (BUILD_DEBUG)
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -O3")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -fopenmp")
        endif (BUILD_DEBUG)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_FLAGS}")

    endif ()

endif (APPLE)

##################
# Build on Linux #
##################
if (UNIX AND NOT APPLE)

    # GCC only
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        set(GCC_CXX_FLAGS "-std=gnu++11 -m64")
        set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -Wno-deprecated-declarations")
        set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -Wno-misleading-indentation")

        # X11 Dependencies
        if (NOT BUILD_LIBCGL)
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXi")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXxf86vm")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXinerama")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXcursor")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXfixes")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXrandr")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXext")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXrender")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lX11")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lpthread")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lxcb")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXau")
        endif ()

        # Debug configuration
        if (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Debug)
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -g")
        else (BUILD_DEBUG)
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -O3")
            set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -fopenmp")
        endif (BUILD_DEBUG)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_FLAGS}")

    endif ()

endif ()

####################
# Build on Windows #
####################
if (WIN32)

    # EIGEN fix:
    # See "Cause 4" here:
    # https://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html

    # NOTE: future maintainers, you may want to use add_compile_definitions instead.
    # this was added because too many students are using old CMake (<3.12) versions.
    add_definitions(/DEIGEN_DONT_ALIGN)

    if (MSVC)

        set(MSVC_CXX_FLAGS "-std=gnu++11")

        if (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Debug)
        else (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Release)
        endif (BUILD_DEBUG)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_CXX_FLAGS}")

    endif (MSVC)

    if (MINGW)

        set(MSVC_CXX_FLAGS "-std=gnu++11")

        if (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Debug)
        else (BUILD_DEBUG)
            set(CMAKE_BUILD_TYPE Release)
        endif (BUILD_DEBUG)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_CXX_FLAGS}")

    endif (MINGW)

endif (WIN32)

#-------------------------------------------------------------------------------
# nanogui configuration and compilation
#-------------------------------------------------------------------------------

# Disable building extras we won't need (pure C++ project)
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL OFF CACHE BOOL " " FORCE)
set(NANOGUI_USE_GLAD ON CACHE BOOL " " FORCE)

# Add the configurations from nanogui
add_subdirectory(ext/nanogui)
include_directories(ext/nanogui/include)

# For reliability of parallel build, make the NanoGUI targets dependencies
set_property(TARGET glfw glfw_objects nanogui PROPERTY FOLDER "dependencies")

# For Windows, set the library output directory to put the DLL's next
# to the binary. I tried to use add_custom_command to just do a copy as a
# POST_BUILD setting, but for some reason no matter what the command does,
# Visual Studio will complain about its solution file being modified?
# In the interest of avoiding the flood of Piazza posts inquiring about this,
# we take the more robust route.
if (WIN32)
    # Also worth mentioning is that since NANOGUI produces a DLL on windows,
    # it is considered a "RUNTIME" and not a "LIBRARY" target according to CMake.
    # See https://cmake.org/cmake/help/v3.0/prop_tgt/RUNTIME_OUTPUT_DIRECTORY.html
    # > For DLL platforms the DLL part of a shared library is treated as a runtime target
    set_target_properties(nanogui PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif (WIN32)


#-------------------------------------------------------------------------------
# Find dependencies
#-------------------------------------------------------------------------------

# Required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
if (NOT WIN32)
    find_package(Freetype REQUIRED)
endif ()

# CGL
if (BUILD_LIBCGL)
    add_subdirectory(CGL)
    include_directories(CGL/include)
else (BUILD_LIBCGL)
    find_package(CGL REQUIRED)
endif (BUILD_LIBCGL)


# Application source
set(APPLICATION_SOURCE
        src/main.cpp
        src/grid.cpp
        src/grid.h
        src/callback.h
        src/common.h
        src/color.h src/smoke_screen.h)

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
add_definitions(${NANOGUI_EXTRA_DEFS})

include_directories(
        #  ${GLEW_INCLUDE_DIRS}
        #  ${GLFW_INCLUDE_DIRS}
        ${CGL_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${NANOGUI_EXTRA_INCS}
)

if (WIN32)
    include_directories(dirent)
endif (WIN32)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
        #  ${GLEW_LIBRARY_DIRS}
        #  ${GLFW_LIBRARY_DIRS}
        ${CGL_LIBRARY_DIRS}
        ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(smoke_simulator ${APPLICATION_SOURCE})

target_link_libraries(smoke_simulator
        CGL ${CGL_LIBRARIES}
        nanogui ${NANOGUI_EXTRA_LIBS}
        #    glew ${GLEW_LIBRARIES}
        #    glfw ${GLFW_LIBRARIES}
        #    ${OPENGL_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${CMAKE_THREADS_INIT}
        )

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if (APPLE)
    set_property(TARGET smoke_simulator APPEND_STRING PROPERTY COMPILE_FLAGS
            "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif (APPLE)

set(CMAKE_INSTALL_PREFIX "${HW0_SOURCE_DIR}/")
install(TARGETS smoke_simulator DESTINATION ${HW0_SOURCE_DIR})
