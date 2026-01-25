#!/bin/bash
# Build script for rebuilding everything
set -e # Exit immediately if a command exits with a non-zero status

echo "Building everything..."

# Build the core engine first
pushd engine > /dev/null

bash build.sh
popd > /dev/null

# Build the devtest
pushd devtest > /dev/null
bash build.sh
popd > /dev/null

echo "All assemblies built successfully."