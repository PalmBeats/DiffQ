# EQDiff - Parallel EQ VST Plugin

A VST3 plugin that implements a parallel EQ effect rack similar to Ableton Live, featuring two parallel EQs where one is phase-inverted. By default, the phase-inverted EQ cancels out the signal, and you can adjust the 10-band EQ to bring back specific frequencies.

## Features

- 10-band EQ with frequencies at 31Hz, 64Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz, and 16kHz
- Phase-inverted parallel processing
- Default state cancels the signal completely
- Adjustable gain for each band from -12dB to +12dB
- VST3 format compatible with all major DAWs

## Building the Plugin

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- JUCE Framework

### Build Steps

1. Clone the repository:
```bash
git clone https://github.com/yourusername/EQDiff.git
cd EQDiff
```

2. Initialize JUCE as a submodule:
```bash
git submodule add https://github.com/juce-framework/JUCE.git
```

3. Create a build directory and run CMake:
```bash
mkdir build
cd build
cmake ..
```

4. Build the project:
```bash
cmake --build .
```

The VST3 plugin will be built in the `build/EQDiff_artefacts/Release/VST3` directory.

## Usage

1. Install the VST3 plugin in your DAW's VST3 plugins directory
2. Load the plugin on an audio track
3. By default, the signal will be cancelled out
4. Adjust the EQ bands to bring back specific frequencies
5. Each band can be adjusted from -12dB to +12dB

## License

This project is licensed under the MIT License - see the LICENSE file for details. 