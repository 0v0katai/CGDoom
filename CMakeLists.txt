cmake_minimum_required(VERSION 3.15)
project(cgdoom LANGUAGES C ASM)

include(GenerateG3A)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/cgdoom/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/libprof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/fxlibc_hal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/cgdoom-asm.s")

function(build_platform BUILD_NAME PLATFORM)
  add_executable(${BUILD_NAME} ${SOURCES})

  include_directories(${BUILD_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cg
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cg/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cgdoom)

  target_compile_options(${BUILD_NAME} PRIVATE
    -Os -flto=auto -fgcse-sm -fgcse-las -fgcse-after-reload -fmerge-all-constants
    -Wall -Wextra -Wno-unused-but-set-parameter -Wno-unused-but-set-variable
    -Wno-sign-compare -Wno-implicit-fallthrough -Wno-misleading-indentation
    -Wno-implicit-function-declaration)

  target_link_options(${BUILD_NAME} PRIVATE
    -L ${CMAKE_CURRENT_SOURCE_DIR}
    -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld"
    -Wl,-print-memory-usage -Wl,-Map=map -Wl,-gc-sections)

  target_link_libraries(${BUILD_NAME} PRIVATE -lc -lgcc -lsyscall-${PLATFORM})
endfunction()

build_platform(cgdoom cg)
build_platform(cwdoom cw)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET cgdoom OUTPUT "CGDoom.g3a"
    NAME "CGDoom" ICONS icon-uns.png icon-sel.png)
  generate_g3a(TARGET cwdoom OUTPUT "CWDoom.g3a"
    NAME "CWDoom" ICONS icon-uns.png icon-sel.png)
endif()
