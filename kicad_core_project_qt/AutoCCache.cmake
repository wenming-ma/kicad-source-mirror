# AutoCCache.cmake - Automatic ccache detection and setup

if(DEFINED CCACHE_CONFIGURED)
  return()
endif()

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
  message(STATUS "Found ccache: ${CCACHE_PROGRAM}")

  execute_process(COMMAND ${CCACHE_PROGRAM} --version
                  OUTPUT_VARIABLE CCACHE_VERSION_OUTPUT
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(CCACHE_VERSION_OUTPUT MATCHES "ccache version ([0-9.]+)")
    set(CCACHE_VERSION ${CMAKE_MATCH_1})
    message(STATUS "ccache version: ${CCACHE_VERSION}")
  endif()

  # Stable shared cache dir (do not bind to build tree)
  if(WIN32)
    if(NOT DEFINED ENV{CCACHE_DIR})
      # Prefer a user-level persistent directory
      if(DEFINED ENV{LOCALAPPDATA})
        set(ENV{CCACHE_DIR} "$ENV{LOCALAPPDATA}/ccache")
      else()
        set(ENV{CCACHE_DIR} "C:/.ccache")
      endif()
    endif()
  endif()

  # Set cache size via config (better than legacy env var)
  execute_process(COMMAND ${CCACHE_PROGRAM} --set-config=max_size=10G)

  # Reasonable sloppiness for dev (tune if you need stricter reproducibility)
  if(NOT DEFINED ENV{CCACHE_SLOPPINESS})
    set(ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros,file_stat_matches,include_file_mtime")
  endif()

  # Use ccache as launcher
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})

  option(PRINT_CCACHE_STATS "Print ccache stats at configure time" ON)
  if(PRINT_CCACHE_STATS)
    execute_process(COMMAND ${CCACHE_PROGRAM} -s
                    OUTPUT_VARIABLE CCACHE_STATS
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "ccache stats:\n${CCACHE_STATS}")
  endif()

  set(CCACHE_CONFIGURED TRUE CACHE INTERNAL "ccache has been configured")
else()
  message(STATUS "ccache not found - compilation will proceed without caching")
  set(CCACHE_CONFIGURED FALSE CACHE INTERNAL "ccache configuration attempted")
endif()
