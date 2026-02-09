#!/bin/bash
# Build script for engine
set -e

mkdir -p ../bin

# Get a list of all the .cpp and .mm files.
sourceFiles=$(find . -type f \( -name "*.cpp" -o -name "*.mm" \))

assembly="engine"
compilerFlags="-g -dynamiclib -fdeclspec -fPIC -fvisibility=hidden -std=c++17"
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -L$VULKAN_SDK/lib -Wl,-rpath,$VULKAN_SDK/lib -framework Cocoa -framework QuartzCore"
defines="-D_DEBUG -DMGO_EXPORT"

echo "Building $assembly for macOS..."
clang++ $sourceFiles $compilerFlags -o ../bin/lib$assembly.dylib $defines $includeFlags $linkerFlags