cmake_minimum_required(VERSION 3.0.0)
project(oscillation VERSION 0.1.0)

include(CTest)
#enable_testing()

include_directories (/src/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)



find_library(LIB_TO_INCLUDE ClassLibrary /path/to/your/library)
find_path (LIB_INCLUDES ClassLibrary.h /home/mishnic/src/boost_1_67_0/libs/)

#find_package(Boost 1.67 COMPONENTS program_options REQUIRED)
#target_link_libraries(anyExecutable Boost::program_options)


add_executable(oscillation
              ./src/analize_coefficients/dynamic_coefficients.h
              ./src/fft/fftw_impl.h
              ./src/filtration/butterworth_filter.h
              ./src/flow/parse_ptl.cpp
              ./src/flow/parse_ptl.h
              ./src/flow/wt_flow.h
              ./src/flow/wt_flow.cpp
              ./src/model/tr_rod_model_params.h
              ./src/oscillation/AngleHistory.h
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

target_link_libraries(
  oscillation 
  /home/mishnic/src/boost_1_67_0/stage/lib/libboost_program_options.a
  /home/mishnic/CPP_Projects/fftw_test/libfftw3.a
)

#target_link_libraries(oscillation Boost::program_options)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
