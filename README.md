![alt text](https://github.com/deep-sea-tactics/Dyver/blob/main/dyver_light.png "Dyver Logo")

# Dyver
Dyver is an application designed to aid in the general operation of an ROV, and contains an extensive pipeline for control and stabilization systems in particular. The repository comes with (some, namely SparkFun, Raspberry Pi, and BlueRobotics) drivers included!™ It additionally will have features for MATE ROV tasks.

## Contributing
Useful information for developers working internally on Dyver.

* Units of Measurement

All units follow the SI standard. Units that do not follow this standard shall be converted into the standard before being placed into any data structures.

* Formatting

Formatting rules are defined in the `.clang-format` file and must be used for all files. Snake case is heavily encouraged.

* Documentation

Write documentation with Doxygen. Use `javadoc` style documentation comments when documenting internal code.

* Dependencies

Create an issue requesting a dependency before any pull requests or changes. After a project manager (most like Estelle Coonan) reviews the issue, it will then be decided upon. Please keep things organized.

## Usage
Binaries are re-configured using the Deep Sea Shell (dss) scripting language. The program is written in C++, and assumes users are building with CMake and Clang.

## Installation Instructions

1. Build on Linux. Windows support is not a planned feature, nor is it guaranteed that the application will build/run on Windows.

2. Ensure you have an up-to-date installation of `clang` (and its related components, such as `clang-format`) and `cmake`. The `clang` compiler is highly recommended for building Dyver. You will want to run the `deps.sh` script located in the top-level repository directory.

3. Build with an internet connection. The two main targets are `Dyver` *(for the on-deck server and graphical user interface)* and `DyverTest` *(which contains every unit test)*.
