file(GLOB_RECURSE server_SRC "*.cxx" )

add_executable(async_tcp_echo_server ${server_SRC} )

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(async_tcp_echo_server ws2_32.lib Iphlpapi.lib Psapi.lib Userenv.lib )
ELSE ()
    target_link_libraries(async_tcp_echo_server boost_system boost_filesystem boost_program_options pthread )
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
