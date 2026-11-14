cmake_minimum_required(VERSION 3.15)
project(cgdoom LANGUAGES C ASM)

include(GenerateG3A)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/cgdoom/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/libprof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/fxlibc_hal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/cgdoom-asm.s")

function(build_platform BUILD_NAME PLATFORM)
  set(PLATFORM_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include/${BUILD_NAME}")
  file(MAKE_DIRECTORY "${PLATFORM_INCLUDE_DIR}")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/platform.h.in"
    "${PLATFORM_INCLUDE_DIR}/platform.h")
  add_executable(${BUILD_NAME} ${SOURCES})

  target_include_directories(${BUILD_NAME} PRIVATE
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cg
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cg/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cgdoom)

  target_compile_options(${BUILD_NAME} PRIVATE
    -Os -flto=auto -fgcse-sm -fgcse-las -fgcse-after-reload -fmerge-all-constants
    -Wall -Wextra -Wno-unused-but-set-parameter -Wno-unused-but-set-variable
    -Wno-sign-compare -Wno-implicit-fallthrough -Wno-misleading-indentation
    -Wno-implicit-function-declaration)

  target_link_options(${BUILD_NAME} PRIVATE
    -L "${CMAKE_CURRENT_SOURCE_DIR}/src-cg"
    -T "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/prizm.ld"
    -Wl,-print-memory-usage -Wl,-Map=${BUILD_NAME}-map -Wl,-gc-sections)

  target_link_libraries(${BUILD_NAME} PRIVATE -lc -lgcc -lsyscall-${PLATFORM})
endfunction()

build_platform(cgdoom cg)
build_platform(cwdoom cw)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET cgdoom VERSION "01.00.0000" OUTPUT "CGDoom.g3a"
    NAME "CGDoom" ICONS src-cg/icon-uns.png src-cg/icon-sel.png)
  generate_g3a(TARGET cwdoom VERSION "01.00.0000" OUTPUT "CWDoom.g3a"
    NAME "CWDoom" ICONS src-cg/icon-uns.png src-cg/icon-sel.png)
endif()
