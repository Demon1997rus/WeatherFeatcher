set(TARGET weather-fetcher)

qt_add_executable(${TARGET}
    main.cpp
    presenter/weathercontroller.cpp
    presenter/weathercontroller.h
    services/httpservice.cpp
    services/httpservice.h
    services/weatherconverter.cpp
    services/weatherconverter.h
    structures/weatherinfo.cpp
    structures/weatherinfo.h
    utils/citydirectory.cpp
    utils/citydirectory.h
    utils/config.cpp
    utils/config.h
    widgets/weatherwidget.cpp
    widgets/weatherwidget.h
)

target_link_libraries(${TARGET} PRIVATE Qt6::Widgets Qt6::Network)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Runtime data (cities.json) next to the binary,
# so the app can locate it via QCoreApplication::applicationDirPath()
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${TARGET}>/data)
