find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(tst_weatherconverter
    tst_weatherconverter.cpp
    ${CMAKE_SOURCE_DIR}/src/services/weatherconverter.cpp
    ${CMAKE_SOURCE_DIR}/src/structures/weatherinfo.cpp
)

target_include_directories(tst_weatherconverter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tst_weatherconverter PRIVATE Qt6::Test)

add_test(NAME weatherconverter COMMAND tst_weatherconverter)

if(WIN32)
    # The test executable needs Qt DLLs next to it at runtime
    set_tests_properties(weatherconverter PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
endif()
