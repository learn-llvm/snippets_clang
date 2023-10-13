cmake_minimum_required(VERSION 3.25)

get_filename_component(PROJECTID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECTID ${PROJECTID})
project(${PROJECTID})

find_package(Clang REQUIRED CONFIG)
file(GLOB SRC_FILES *.cc)
set(EXECUTABLE_NAME "${PROJECTID}.out")
add_executable(${EXECUTABLE_NAME} ${SRC_FILES})
target_link_libraries(${EXECUTABLE_NAME} ${CLANG_LIBS} ${FULL_LLVM_LIBS})
