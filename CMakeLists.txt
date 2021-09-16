cmake_minimum_required (VERSION 3.16)

project(test_asio)

    include_directories( $ENV{SPDLOG_PATH} $ENV{BOOST_PATH})

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")

    LINK_DIRECTORIES( $ENV{BOOST_PATH}/lib64-msvc-14.2/ )

    add_compile_options("/std:c++17")
    add_compile_options(/w44819)
    add_compile_options(/w44251)
    add_compile_options(/w44244)

    add_definitions(-DWIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
    add_definitions(-D_USE_MATH_DEFINES)

ELSE ()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fdiagnostics-color=auto")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-limit-debug-info -std=c++14")
    add_compile_options(-g)
    #add_compile_options(-std=c++17)
    add_compile_options(-ggdb)
    add_compile_options(-Wall)
    link_libraries(stdc++fs)

ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_subdirectory(server)
add_subdirectory(client)

