cmake_minimum_required(VERSION 3.0)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "F:/LocalSoftWareInstall/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
endif()

#   设置编译器标准
set(CMAKE_C_STANDARD 99)

#   设置工程名与版本号
project(wb32-dfu-updater_cli VERSION 0.0.1)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    #   设置编译类型 Debug / Release
    set(CMAKE_BUILD_TYPE Debug)
    #   添加编译选项
    add_compile_options("-Wall") 
endif()

#   添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})

#   将源文件路径保存到DIR_SRCS变量
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_FILES)

include(${PROJECT_SOURCE_DIR}/cmake/compiler_conf.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/building_output.cmake)
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in cmake_config.h)

find_package(libusb CONFIG REQUIRED)

#   设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)

# 编译生成可执行文件
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

#  链接库头文件
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${LIBUSB_INCLUDE_DIRS})
#  链接库文件
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBUSB_LIBRARIES})

install(TARGETS  ${CMAKE_PROJECT_NAME} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}./bin
    # LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}./lib
    # ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}./lib
    PERMISSIONS OWNER_EXECUTE
)

INSTALL(FILES LICENSE README.MD DESTINATION ${CMAKE_INSTALL_LIBDIR}/)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/pack)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(PROJECT_VERSION_FULL ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(CPACK_SOURCE_IGNORE_FILES
        ${PROJECT_SOURCE_DIR}/.vscode
        ${PROJECT_SOURCE_DIR}/.git
        ${PROJECT_SOURCE_DIR}/*.hex
        ${PROJECT_SOURCE_DIR}/*.bin
        ${PROJECT_SOURCE_DIR}/*.exe
        ${PROJECT_SOURCE_DIR}/windows_driver
        ${PROJECT_SOURCE_DIR}/build
        ${PROJECT_SOURCE_DIR}/docs
        ${PROJECT_SOURCE_DIR}/pack
        ${PROJECT_SOURCE_DIR}/bin)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-v${PROJECT_VERSION_FULL}-${CMAKE_SYSTEM_NAME})

include(CPack)
