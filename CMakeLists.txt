cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(wx_gl_shadertoy_tutorial LANGUAGES CXX C)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX AND NOT APPLE) # For Linux
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)

set(wxBUILD_SHARED OFF)

message(STATUS "Fetching wxWidgets...")

FetchContent_Declare(
   wxWidgets
   GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
   GIT_SHALLOW ON
)
FetchContent_MakeAvailable(wxWidgets)

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)
target_link_libraries(glad PRIVATE OpenGL::GL)

set(SRCS src/main.cpp src/openglcanvas.cpp)

if(APPLE)
    # create bundle on apple compiles
    add_executable(main MACOSX_BUNDLE ${SRCS})

    # Set a custom plist file for the app bundle - needed for Mac OS Retina display
    set_target_properties(main PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist)
else()
    # the WIN32 is needed for Windows in order for it to look for WinMain
    # instead of the main function. This is ignored on other systems,
    # so it works on all platforms
    add_executable(main WIN32 ${SRCS} src/main.exe.manifest)
endif()

target_link_libraries(main PRIVATE wxcore wxstc wxgl glad)
