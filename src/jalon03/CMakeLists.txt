cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON03)

find_package (Threads)
#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
	)


add_executable(CLIENT ${SRC_CLIENT})

#add here source files you need to compile and link to the server app
SET(SRC_SERVER
		socket_functions.c
		list_client.c
        server.c)


add_executable(SERVER  ${SRC_SERVER})
target_link_libraries (SERVER ${CMAKE_THREAD_LIBS_INIT})

