cmake_minimum_required (VERSION 2.8)
set_property (DIRECTORY PROPERTY USE_FOLDERS ON)

set (LIBRARY_NAME filesystem)
set (LIBRARY_CMAKE_PREFIX C2BA_FILESYSTEM)
set (LIBRARY_OUTPUT c2ba_${LIBRARY_NAME})

project (${LIBRARY_OUTPUT})

option (${LIBRARY_CMAKE_PREFIX}_BUILD_TESTS "Build tests for ${LIBRARY_NAME} library" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/lib/cmake/info.hpp.in"
    "${PROJECT_BINARY_DIR}/include/c2ba/${LIBRARY_NAME}/info.hpp"
)

file (
    GLOB_RECURSE
    HEADER_FILES
    lib/*.hpp
)

file (
    GLOB_RECURSE
    PROJECT_FILES
    lib/*.cpp lib/*.hpp lib/*.in
)

add_library (
    ${LIBRARY_OUTPUT}
    ${PROJECT_FILES}
)

target_include_directories (
    ${LIBRARY_OUTPUT}
    PUBLIC "lib/include" "lib/include/c2ba" "${PROJECT_BINARY_DIR}/include"
)

source_group ("c2ba\\${LIBRARY_NAME}" REGULAR_EXPRESSION "lib/(src|include/c2ba/${LIBRARY_NAME})/[^/]*.*")

if (${LIBRARY_CMAKE_PREFIX}_BUILD_TESTS)
    set (${LIBRARY_NAME}_TEST_INCLUDE_LIST "")
    
    foreach (HEADER_PATH ${HEADER_FILES})
        set (${LIBRARY_NAME}_TEST_INCLUDE_LIST "${${LIBRARY_NAME}_TEST_INCLUDE_LIST} #include \"${HEADER_PATH}\"\n")
    endforeach (HEADER_PATH)

    configure_file (
        "${PROJECT_SOURCE_DIR}/tests/headers_compilation_test.cpp.in"
        "${PROJECT_BINARY_DIR}/c2ba/${LIBRARY_NAME}/tests/headers_compilation_test.cpp"
    )

    file (
        GLOB_RECURSE
        TEST_SRC_FILES
        tests/*.cpp tests/*.hpp "${PROJECT_BINARY_DIR}/c2ba/${LIBRARY_NAME}/tests/headers_compilation_test.cpp"
    )
    
    set (TEST_EXECUTABLE_NAME ${LIBRARY_OUTPUT}_run_tests)
    
    add_executable (
        ${TEST_EXECUTABLE_NAME}
        ${TEST_SRC_FILES}
    )
    
    target_link_libraries (
        ${TEST_EXECUTABLE_NAME}
        ${LIBRARY_OUTPUT}
    )
    
    target_include_directories (
        ${TEST_EXECUTABLE_NAME}
        PUBLIC "lib/include" "${PROJECT_BINARY_DIR}/include"
    )
    
endif (${LIBRARY_CMAKE_PREFIX}_BUILD_TESTS)

install (DIRECTORY lib/include/c2ba DESTINATION include)
install (FILES "${PROJECT_BINARY_DIR}/include/c2ba/${LIBRARY_NAME}/info.hpp" DESTINATION include/c2ba/${LIBRARY_NAME})

foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    install (
        TARGETS ${LIBRARY_OUTPUT}
        DESTINATION lib/${CONFIG}
        CONFIGURATIONS ${CONFIG}
    )
    install (
        DIRECTORY ${PROJECT_BINARY_DIR}/${CONFIG}
        DESTINATION lib
        CONFIGURATIONS ${CONFIG}
        FILES_MATCHING PATTERN ${LIBRARY_OUTPUT}.pdb
    )
endforeach (CONFIG)