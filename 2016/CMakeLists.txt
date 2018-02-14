cmake_minimum_required(VERSION 3.5)

project(LOMSP2016 LANGUAGES CXX)

option(BUILD_THREADING "Build the STL threading samples" ON)
option(BUILD_CAIRO "Build the Cairo samples" OFF)
option(BUILD_OPENGL "Build the OpenGL samples" OFF)
option(BUILD_DIRECT3D "Build the Direct3D samples" OFF)

if(BUILD_THREADING)
  set(THREADING_SAMPLES Sample1
                        Sample2
                        Sample3
                        Sample4
                        Sample5
                        Sample6
                        Sample7
                        Sample8
                        Sample9)

  foreach(SAMPLE ${THREADING_SAMPLES})
    add_executable(${SAMPLE} Threading/${SAMPLE}.cpp)

    set_target_properties(${SAMPLE}
                          PROPERTIES CXX_STANDARD 14
                                     CXX_STANDARD_REQUIRED ON
                                     CXX_EXTENSIONS OFF)
  endforeach(SAMPLE ${THREADING_SAMPLES})
endif(BUILD_THREADING)
