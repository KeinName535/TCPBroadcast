cmake_minimum_required(VERSION 3.5)

project(TCPServerBroadcast)

add_subdirectory(sockpp)


file(GLOB_RECURSE A3_CPP_SERVER_SOURCES ./TCPSockServer/*.cpp ./TCPSockServer/*.h)

file(GLOB_RECURSE A3_CPP_CLIENT_SOURCES ./TCPSockCLient/*.cpp ./TCPSockCLient/*.h)



add_executable(TCP_Server ${A3_CPP_SERVER_SOURCES})
add_executable(TCP_Client ${A3_CPP_CLIENT_SOURCES})

add_compile_options(-pthread)

target_include_directories(TCP_Server PUBLIC sockpp/include)
target_include_directories(TCP_Client PUBLIC sockpp/include)


target_link_libraries(TCP_Server PUBLIC sockpp)
target_link_libraries(TCP_Client PUBLIC sockpp)
