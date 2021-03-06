if (WIN32 AND NOT UNIX)

    INSTALL(TARGETS  ${CMAKE_PROJECT_NAME} 
        RUNTIME DESTINATION bin
        PERMISSIONS OWNER_EXECUTE
    )

    # if (CMAKE_BUILD_TYPE STREQUAL Release)

    #     INSTALL(FILES ${LIBUSB_LIBRARIES}
    #         DESTINATION lib
    #         CONFIGURATIONS Release
    #         COMPONENT Runtime
    #     )

    # elseif(CMAKE_BUILD_TYPE STREQUAL Debug)

    #     INSTALL(FILES ${LIBUSB_LIBRARIES}
    #         DESTINATION lib
    #         CONFIGURATIONS Debug
    #         COMPONENT Runtime
    #     )

    # endif()

    # INSTALL(FILES LICENSE README.MD DESTINATION /)

else (WIN32 AND NOT UNIX)

    INSTALL(TARGETS  ${CMAKE_PROJECT_NAME} 
        RUNTIME DESTINATION bin
        COMPONENT Runtime
    )

endif(WIN32 AND NOT UNIX)
