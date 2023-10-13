get_filename_component(PROJECTID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECTID ${PROJECTID})
project(${PROJECTID})

set(EXECUTABLE_NAME "${PROJECTID}")
add_library(${EXECUTABLE_NAME} SHARED MyCheckers.cc)
target_link_libraries(${EXECUTABLE_NAME} ${FULL_LLVM_LIBS})
