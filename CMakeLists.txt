cmake_minimum_required(VERSION 3.10)
project(MIS VERSION 1.0.0 LANGUAGES C CXX)

find_package(JlCxx REQUIRED)
option (USE_IFC "Link InertialFlowCutter and KaHIP." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS_DEBUG  "-g")
SET(CMAKE_CXX_FLAGS_RELEASE  "-O3")
set(libname miscb)

if(USE_IFC)
    # load submodules
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()
endif()

if (USE_IFC)
    add_subdirectory(extern/InertialFlowCutter)
endif()

add_executable(mis main.cpp algo/branch_and_reduce_algorithm.cpp algo/modified.cpp algo/separator_refinement/fm_ns_local_search.cpp)
add_library(miscb SHARED algo/branch_and_reduce_algorithm.cpp algo/modified.cpp algo/separator_refinement/fm_ns_local_search.cpp wrapper/jlwrapper.cpp)
set_target_properties(miscb PROPERTIES PUBLIC_HEADER "wrapper/jlwrapper.h")
#target_link_libraries(MIS ${CMAKE_SOURCE_DIR}/Metis/build/Linux-x86_64/libmetis/libmetis.a)
target_link_libraries(miscb PRIVATE JlCxx::cxxwrap_julia JlCxx::cxxwrap_julia_stl)

if (USE_IFC)
    target_compile_definitions(miscb PUBLIC USE_IFC)
    target_link_libraries(miscb inertial_flow)
endif()

install(TARGETS miscb
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
