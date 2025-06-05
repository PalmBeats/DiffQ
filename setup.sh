#!/bin/bash

# Initialize git if not already done
if [ ! -d ".git" ]; then
    git init
fi

# Add JUCE as a submodule
git submodule add https://github.com/juce-framework/JUCE.git

# Create build directory
mkdir -p build
cd build

# Run CMake
cmake ..

echo "Setup complete! You can now build the project with:"
echo "cd build && cmake --build ." 