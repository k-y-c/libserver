# for each "test/x.cpp", generate target "x"
# file(GLOB_RECURSE all_tests *.cpp)
# foreach(v ${all_tests})
#     string(REGEX MATCH "test/.*" relative_path ${v})
#     # message(${relative_path})
#     string(REGEX REPLACE "test/" "" target_name ${relative_path})
#     string(REGEX REPLACE ".cpp" "" target_name ${target_name})

#     add_executable(${target_name} ${v})
#     target_link_libraries(${target_name} pthread)
# endforeach()

add_executable(test_threadpool test_threadpool.cpp)
target_link_libraries(test_threadpool pthread)

add_executable(test_addr test_addr.cpp)
target_link_libraries(test_addr pthread)

add_executable(test_epoll test_epoll.cpp)
target_link_libraries(test_epoll pthread)

add_executable(test_echo test_echo.cpp)
target_link_libraries(test_echo pthread server_net)

add_executable(test_chargen test_chargen.cpp)
target_link_libraries(test_chargen pthread server_net)

add_executable(test_timer test_timer.cpp)
target_link_libraries(test_timer pthread server_net)