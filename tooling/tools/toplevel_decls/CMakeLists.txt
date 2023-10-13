set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

get_filename_component(PROJECTID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECTID ${PROJECTID})
project(${PROJECTID})

llvm_map_components_to_libnames(llvm_libs support core)

file(GLOB SRC_FILES *.cc)
set(EXECUTABLE_NAME "${PROJECTID}.out")
add_executable(${EXECUTABLE_NAME} ${SRC_FILES})
target_link_libraries(${EXECUTABLE_NAME} ${CLANG_LIBS} ${llvm_libs})
