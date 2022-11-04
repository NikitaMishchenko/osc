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

#set(SOURCE_FILES main.cpp)

add_executable(oscillation
              ./src/gnusl_proc/linnear_approximation.h
              ./src/analize_coefficients/dynamic_coefficients.h
              ./src/fft/fftw_impl.h
              ./src/filtration/butterworth_filter.h
              ./src/flow/parse_ptl.cpp
              ./src/flow/parse_ptl.h
              ./src/flow/wt_flow.h
              ./src/flow/wt_flow.cpp
              ./src/model/tr_rod_model_params.h
              ./src/oscillation/angle_history.h
              ./src/oscillation/cut_oscillation_file.h
              ./src/oscillation/oscillation_basic.h
              ./src/oscillation/oscillation_helpers.h
              ./src/oscillation/wt_oscillation.h
              ./src/periods/periods_base.h
              ./src/basic_procedures.h
              ./src/errcodes.h
              ./src/options.h
              ./src/options.cpp
              ./src/main.cpp
)

find_package(GSL REQUIRED)

target_link_libraries(
  oscillation 
  /home/mishnic/src/boost_1_67_0/stage/lib/libboost_program_options.a
  /home/mishnic/CPP_Projects/fftw_test/libfftw3.a
  #/user/local/lib/libfftw3.a
  /usr/local/lib/libgsl.a
  /usr/local/lib/libgslcblas.a
  GSL::gsl
  GSL::gslcblas
  #/home/mishnic/gsl/lib/libgslcblas.a
)


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