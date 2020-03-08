cmake_minimum_required(VERSION 3.10)
project(priority_queue)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

file(GLOB head
        *.hpp)

file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/data"
        code.cpp)

foreach (main_file ${mains})
    get_filename_component(test_name ${main_file} DIRECTORY)
    add_executable(test_${test_name}
            data/${main_file}
            ${head}
            )
endforeach ()