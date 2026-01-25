#!/bin/bash
# Build script for devtest
set -e # Exit on error

mkdir -p ../bin

# Get a list of all the .cpp files.
cppFilenames=$(find . -type f -name "*.cpp")

assembly="devtest"
compilerFlags="-g -fdeclspec" 
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin/ -lengine -L$VULKAN_SDK/lib -Wl,-rpath,@executable_path -Wl,-rpath,$VULKAN_SDK/lib"
defines="-D_DEBUG -DMGO_IMPORT"

echo "Building $assembly for macOS..."
clang++ $cppFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags