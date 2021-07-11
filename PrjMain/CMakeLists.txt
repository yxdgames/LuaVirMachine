cmake_minimum_required(VERSION 3.00)

project(PrjMain)

aux_source_directory(${ROOT_DIR}/Codes SRC_ROOT)

add_executable(${PROJECT_NAME} ${SRC_ROOT})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/")
target_link_libraries(${PROJECT_NAME} -llua)