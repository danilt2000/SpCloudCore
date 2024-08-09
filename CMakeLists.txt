cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported (only applicable on Windows).
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

project ("SpCloudMain")

# Platform-specific settings
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows-specific configuration
  if (MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux-specific configuration
  # Here you can add Linux-specific flags, libraries, etc.
  # For example, enabling C++17 or another specific Linux configuration
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
endif()

# Include sub-projects.
add_subdirectory ("SpCloudMain")
