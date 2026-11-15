find_package(Qt6 REQUIRED COMPONENTS Test Gui)

add_executable(tst_weatherconverter
    tst_weatherconverter.cpp
    ${CMAKE_SOURCE_DIR}/src/services/weatherconverter.cpp
    ${CMAKE_SOURCE_DIR}/src/structures/weatherinfo.cpp
)

target_include_directories(tst_weatherconverter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tst_weatherconverter PRIVATE Qt6::Test)

add_executable(tst_citydirectory
    tst_citydirectory.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/citydirectory.cpp
)

target_include_directories(tst_citydirectory PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tst_citydirectory PRIVATE Qt6::Test Qt6::Gui)

add_test(NAME weatherconverter COMMAND tst_weatherconverter)
add_test(NAME citydirectory COMMAND tst_citydirectory)

if(WIN32)
    # The test executables need Qt DLLs next to them at runtime
    set_tests_properties(weatherconverter citydirectory PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
endif()
