add_executable(server udp_server.c)
add_executable(client udp_client.c)
install(TARGETS server DESTINATION bin)
install(TARGETS client DESTINATION bin)
