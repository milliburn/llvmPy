set(name ${LLVMPY_EXECUTABLE_NAME})

add_executable(
        ${name}
        src/main.cc
        src/build.h.in)

configure_file(
        src/build.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/build.h)

target_include_directories(
        ${name}
        PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(
        ${name}
        PRIVATE lib${name})
