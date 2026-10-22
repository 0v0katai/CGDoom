cmake_minimum_required(VERSION 3.15)
project(cgdoom LANGUAGES C ASM)

include(GenerateG3A)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/cgdoom/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/libprof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/fxlibc_hal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src-cg/cgdoom-asm.s")

add_executable(cgdoom ${SOURCES})

include_directories(cgdoom PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src-cg
  ${CMAKE_CURRENT_SOURCE_DIR}/src-cg/include
  ${CMAKE_CURRENT_SOURCE_DIR}/cgdoom)

target_compile_options(cgdoom PRIVATE
  -Os -flto=auto -fgcse-sm -fgcse-las -fgcse-after-reload -fmerge-all-constants
  -Wall -Wextra -Wno-unused-but-set-parameter -Wno-unused-but-set-variable
  -Wno-sign-compare -Wno-implicit-fallthrough -Wno-misleading-indentation
  -Wno-implicit-function-declaration)

target_link_options(cgdoom PRIVATE
  -L ${CMAKE_CURRENT_SOURCE_DIR}
  -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld"
  -Wl,-print-memory-usage -Wl,-Map=map -Wl,-gc-sections)

target_link_libraries(cgdoom PRIVATE -lc -lgcc -lsyscall-cg)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET cgdoom OUTPUT "CGDoom.g3a"
    NAME "CGDoom" ICONS icon-uns.png icon-sel.png)
endif()
