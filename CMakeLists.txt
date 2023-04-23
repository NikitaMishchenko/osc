cmake_minimum_required(VERSION 3.0.0)
project(oscillation VERSION 0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(CTest)

#enable_testing()

include_directories (src/)
#include_directories (/home/mishnic/gsl/include/gsl/)

#add_subdirectory(tests build/tests)
#add_subdirectory(tests)


#find_package(Boost 1.67 COMPONENTS program_options REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

#set(SOURCE_FILES main.cpp)

add_library(flow 
          ./src/flow/parse_ptl.cpp
          ./src/flow/parse_ptl.h
          ./src/flow/wt_flow.h
          ./src/flow/wt_flow.cpp
)
set_target_properties(flow PROPERTIES LINKER_LANGUAGE CXX)

#add_library(dynamic_coefficient
#            src/analize_coefficients/dynamic_coefficient.h)
#set_target_properties(dynamic_coefficient PROPERTIES LINKER_LANGUAGE CXX)

add_library(fft_impl
            ./src/fft/fftw_impl.h
)
set_target_properties(fft_impl PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(fft_impl
                      fftw3 )

add_library(basic 
            ./src/core/function.h
            ./src/oscillation/angle_history.h
            ./src/oscillation/angle_history.cpp
            ./src/oscillation/oscillation_basic.h
            ./src/oscillation/oscillation_basic.cpp)


add_library(oscillationlib
            ./src/gnusl_proc/approximation/linnear.h
            ./src/analize_coefficients/dynamic_coefficients.h
            ./src/filtration/butterworth_filter.h
            ./src/model/tr_rod_model_params.h
            #./src/oscillation/wt_oscillation.h
            ./src/oscillation/wt_oscillation.cpp
            ./src/periods/periods_base.h
            ./src/analize_coefficients/dynamic_coefficient.h
)
set_target_properties(oscillationlib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(oscillationlib
                      basic
                      flow
                      fft_impl
                      #dynamic_coefficient
                      libboost_program_options.a
                      GSL::gsl
                      GSL::gslcblas
                      fftw3 )

add_library (wtOscillationlib
            ./src/analize_coefficients/specific/pitch_momentum.h
            ./src/analize_coefficients/specific/pitch_static_momentum.h
            ./src/oscillation/wt_oscillation.h
            ./src/oscillation/wt_oscillation.cpp       
)
set_target_properties(wtOscillationlib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(wtOscillationlib
                      basic
                      flow
                      GSL::gsl
                      GSL::gslcblas)          

add_library (gnuplotWrapperLib
             ./src/gnuplot/gnuplot_wrapper.h
             ./src/gnuplot/gnuplot_1d.h
             )

set_target_properties(gnuplotWrapperLib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(gnuplotWrapperLib
                      libboost_iostreams.a
                      libboost_system.a
                      libboost_filesystem.a)

add_executable(oscillation
              ./src/main.cpp
              ./src/options.h
              ./src/options.cpp
              ./src/basic_procedures.h
              ./src/errcodes.h
              ./src/io_helpers/cut_oscillation_file.h
              ./src/utils/function_generator/function_generator.h
)
set_target_properties(oscillation oscillationlib PROPERTIES LINKER_LANGUAGE CXX)

find_package(GSL REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# sudo apt-get install libgsl-dev libgsl-dev libboost-dev
# libgtest-dev /usr/src/gtest make it
# sudo apt install libgnuplot-iostream-dev
target_link_libraries(oscillation
                      basic  
                      flow
                      wtOscillationlib
                      gnuplotWrapperLib
                      libboost_program_options.a
                      GSL::gsl
                      GSL::gslcblas
                      fftw3
)

add_executable(runTests tests/main.cpp tests/test.cpp)
target_link_libraries(runTests
                     ${GTEST_LIBRARIES}
                     pthread
                     oscillationlib
                     gnuplotWrapperLib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

#TESTS


#add_executable(tests tests/tests.cpp)
#target_link_libraries(tests PRIVATE tests)

#add_test(
#  NAME tests
#  COMMAND $<TARGET_FILE:tests>
#  )
