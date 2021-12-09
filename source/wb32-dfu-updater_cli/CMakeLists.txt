cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 99)
project(wb32-dfu-updater_cli)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

#   设置输出可执行文件名
set(PROJECTOBJEXE ${PROJECT_NAME})

if(WIN32)
#   设置引用库名
set(PROJECTLIB libusb-1.0)
set(SYSTEM_NAME Windows-x86_64)

elseif(APPLE)
#   设置库搜索路径
link_directories(/usr/local/lib)
link_directories(/usr/local/include)
#   设置引用库名
set(PROJECTLIB libusb-1.0.a)
set(SYSTEM_NAME MacOs)

elseif(UNIX)
#   设置引用库名
set(PROJECTLIB usb-1.0)
set(SYSTEM_NAME Linux-x86_64)

endif()

#   添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/src)

#   将源文件路径保存到DIR_SRCS变量
aux_source_directory(${CMAKE_SOURCE_DIR} DIR_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src DIR_SRCS)

#   设置可执行文件输出路径
set(PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}../../../build/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

#   add compile options
add_compile_options("-Wall")

#   set CMAKE_BUILD_TYPE
set(CMAKE_BUILD_TYPE Debug)

# 编译生成可执行文件
add_executable(${PROJECTOBJEXE} ${DIR_SRCS})

#  添加链接库 
target_link_libraries(${PROJECTOBJEXE} ${PROJECTLIB})

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/../../pack)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../../LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(PROJECT_VERSION_FULL ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-v${PROJECT_VERSION_FULL}-${SYSTEM_NAME})

include(CPack)

INSTALL(TARGETS  ${PROJECTOBJEXE}
    RUNTIME DESTINATION bin
    PERMISSIONS OWNER_EXECUTE
)
