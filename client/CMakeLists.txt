file(GLOB_RECURSE client_SRC "*.cxx" )

add_executable(async_tcp_echo_client ${client_SRC} )

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(async_tcp_echo_client ws2_32.lib Iphlpapi.lib Psapi.lib Userenv.lib )
ELSE ()
    target_link_libraries(async_tcp_echo_client boost_system boost_filesystem boost_program_options pthread )
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
