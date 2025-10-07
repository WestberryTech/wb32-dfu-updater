cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)

project(wb32-dfu-updater_cli VERSION 1.0.0)

if (NOT (CMAKE_BUILD_TYPE STREQUAL Release))
    #   Build type Debug / Release
    set(CMAKE_BUILD_TYPE Debug)
    #   Build option
    add_compile_options("-Wall") 
endif()

#   Add head file path.
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src_common)
if (WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/src_windows)
endif()

#   Add source file.
aux_source_directory(${CMAKE_SOURCE_DIR}/src_common SRC_FILES)
if (WIN32)
    aux_source_directory(${CMAKE_SOURCE_DIR}/src_windows SRC_FILES)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/compiler_conf.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/building_output.cmake)
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in cmake_config.h)


if (LIBUSB_INCLUDE_DIRS AND LIBUSB_LIBRARIES)

    # in cache already
    set(LIBUSB_FOUND TRUE)

    message(STATUS  "LIBUSB_INCLUDE_DIRS: ${LIBUSB_INCLUDE_DIRS}")
    message(STATUS  "LIBUSB_LIBRARIES: ${LIBUSB_LIBRARIES}")

else (LIBUSB_INCLUDE_DIRS AND LIBUSB_LIBRARIES)

    find_path(LIBUSB_INCLUDE_DIR
      NAMES libusb-1.0/libusb.h
      DOC "libusb include directory")
    set(LIBUSB_INCLUDE_DIR ${LIBUSB_INCLUDE_DIR}/libusb-1.0)
    mark_as_advanced(LIBUSB_INCLUDE_DIR)

    message(STATUS "LIBUSB_INCLUDE_DIR=${LIBUSB_INCLUDE_DIR}")

    find_library(LIBUSB_LIBRARY
      NAMES libusb-1.0.so libusb-1.0.dylib libusb-1.0.dll
      DOC "libusb library")
    mark_as_advanced(LIBUSB_LIBRARY)

    message(STATUS "LIBUSB_LIBRARY=${LIBUSB_LIBRARY}")

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LIBUSB REQUIRED_VARS LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)

    if ((LIBUSB_INCLUDE_DIR STREQUAL LIBUSB_INCLUDE_DIR-NOTFOUND) OR 
        (LIBUSB_LIBRARY STREQUAL LIBUSB_LIBRARY-NOTFOUND))

        message(FATAL_ERROR "
            No libusb path information is provided!
            LIBUSB_INCLUDE_DIRS=???
            LIBUSB_LIBRARIES=???
        ")
    else()

        set(LIBUSB_INCLUDE_DIRS ${LIBUSB_INCLUDE_DIR})
        set(LIBUSB_LIBRARIES ${LIBUSB_LIBRARY})

    endif()

endif (LIBUSB_INCLUDE_DIRS AND LIBUSB_LIBRARIES)

#   Set execute file path.
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)

#   Compile exe
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

#  include head file.
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${LIBUSB_INCLUDE_DIRS})
#  link lib file.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBUSB_LIBRARIES})

#  install configuration.
include(${PROJECT_SOURCE_DIR}/cmake/install_common.cmake)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/package)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(PROJECT_VERSION_FULL ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(CPACK_SOURCE_IGNORE_FILES
        ${PROJECT_SOURCE_DIR}/.DS_Store
        ${PROJECT_SOURCE_DIR}/.vscode
        ${PROJECT_SOURCE_DIR}/.git
        ${PROJECT_SOURCE_DIR}/*.hex
        ${PROJECT_SOURCE_DIR}/*.bin
        ${PROJECT_SOURCE_DIR}/*.exe
        ${PROJECT_SOURCE_DIR}/package
        ${PROJECT_SOURCE_DIR}/driver
        ${PROJECT_SOURCE_DIR}/build
        ${PROJECT_SOURCE_DIR}/docs
        ${PROJECT_SOURCE_DIR}/bin)

if(WIN32 AND NOT UNIX)
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ${CMAKE_PROJECT_NAME}")
    set(CPACK_NSIS_MODIFY_PATH ON)
endif()


set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_FULL})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-v${PROJECT_VERSION_FULL}-${CMAKE_SYSTEM_NAME})
set(CPACK_PACKAGE_EXECUTABLES "${CPACK_SOURCE_PACKAGE_FILE_NAME}" "${CPACK_SOURCE_PACKAGE_FILE_NAME}")

include(CPack)
