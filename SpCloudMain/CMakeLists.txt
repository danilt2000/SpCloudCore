# CMakeList.txt : CMake project for SpCloudMain, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(SpCloudMain
    "SpCloudMain.cpp"
    "SpCloudMain.h"
    "Controllers/PublishController.cpp"
    "Service/AuthorizationService.cpp"
    "Service/FileProcessingService.cpp"
    "Service/CommandService.cpp"
    "Service/Logger.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SpCloudMain PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
