add_executable(server tcp_server.c)
add_executable(client tcp_client.c)
install(TARGETS server DESTINATION bin)
install(TARGETS client DESTINATION bin)
