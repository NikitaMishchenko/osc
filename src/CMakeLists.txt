cmake_minimum_required(VERSION 3.10)

project(BaseOscillationAnalysis VERSION 1.0)

# UNIX, WIN32, WINRT, CYGWIN, APPLE are environment variables as flags set by default system
if(UNIX)
    message("This is a ${CMAKE_SYSTEM_NAME} system")
elseif(WIN32)
    message("This is a Windows System")
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


#результат сборки
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SOURCES	oscillation/oscillation_file.h
		periods/periods_base.h
		fft/fft_impl.h
		AngleHistory.h
		options.h
		options.cpp
		osillation_base.h
		tr_rod_model_params.h
		wt_flow.h
		wt_oscillation.h
		)

target_link_libraries(program_options /home/mishnic/src/boost_1_67_0/stage/lib/libboost_program_options.a)

add_executable(BaseOscillationAnalysis main.cpp ${SOURCES})

