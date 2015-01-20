cmake_minimum_required (VERSION 2.8)
project (lua_embed C)
 
#find_package(Lua REQUIRED)
#find_package(torch REQUIRED)
 
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
)
 
set (SOURCES
    main.c
)
 
add_executable (${PROJECT_NAME} ${SOURCES} ${LUA_LIBRARIES})
include_directories(/home/nouiz/torch/include/TH)
#include_directories(/opt/OpenBLAS/lib/)
target_link_libraries (${PROJECT_NAME} /home/nouiz/torch/lib/libluajit.so /opt/OpenBLAS/lib/libopenblas.so /home/nouiz/torch/lib/libTH.so  /home/nouiz/torch/lib/libluaT.so )