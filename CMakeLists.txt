# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# project name and languages
project(GetModuleVersion C RC)

##############################################################################

# GetModuleVersion.exe
add_executable(GetModuleVersion GetModuleVersion.c)
target_link_libraries(GetModuleVersion shlwapi version)

##############################################################################
