cmake_minimum_required (VERSION 3.12)

# Set options for this project.
project(UDP_server ${PROJECT_NAME})

set(SOURCES
    main.cpp)
set(HEADERS
    include/biometric_scanner.hpp
    include/UDP_server.hpp)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=4800")

include_directories(include)

find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS context coroutine date_time timer thread system program_options regex)

# "Add an executable to the project using the specified source files."
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


set_target_properties(${PROJECT_NAME}  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
    coroutine PROPERTIES

)
# "Link a target to given libraries."
 target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})


