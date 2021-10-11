cmake_minimum_required(VERSION 3.0)

project(DEVELOPDFU)

#   设置输出可执行文件名
set(PROJECTOBJEXE wb32-dfu-updater)

if(WIN32)
#   设置引用库名
set(PROJECTLIB libusb-1.0)

elseif(APPLE)
#   设置库搜索路径
link_directories(/usr/local/lib)
#   设置引用库名
set(PROJECTLIB libusb-1.0.a)

elseif(UNIX)
#   设置引用库名
set(PROJECTLIB usb-1.0)

endif()

#   添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

#   将源文件路径保存到DIR_SRCS变量
aux_source_directory(${CMAKE_SOURCE_DIR} DIR_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src DIR_SRCS)

message(STATUS "library path :" ${CMAKE_LIBRARY_PATH}) 
message(STATUS "include path :" ${CMAKE_INCLUDE_PATH}) 

#   设置可执行文件输出路径
set(PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

#   add compile options
add_compile_options("-Wall")

#   set CMAKE_BUILD_TYPE
set(CMAKE_BUILD_TYPE Debug)

# 编译生成可执行文件 my_dfu_exe
add_executable(${PROJECTOBJEXE} ${DIR_SRCS})

#  添加链接库 
target_link_libraries(${PROJECTOBJEXE} ${PROJECTLIB})