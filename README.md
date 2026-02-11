![alt text](https://github.com/deep-sea-tactics/Dyver/blob/main/dyver_light.png "Dyver Logo")

# Dyver
Dyver is an application designed to aid in the general operation of an ROV, and contains an extensive pipeline for control and stabilization systems in particular. The repository comes with (some, namely SparkFun, Raspberry Pi, and BlueRobotics) drivers included!™ It additionally will have features for MATE ROV tasks.

## For Developers
Useful information for developers working internally on Dyver.

* Units of Measurement

All units follow the SI standard. Units that do not follow this standard shall be converted into the standard before being placed into any data structures.

## Usage
Binaries are re-configured using the Deep Sea Shell (dss) scripting language. The program is written in C++, and assumes users are building with CMake and Clang.

## Installation Instructions

1. Build on Debian Linux! This means installing WSL (for Windows developers, if you haven't already), or using a Debian Linux machine.
Debian is the target operating system for all parts of Dyver, and it is important to do work on Debian to maintain consistency. Windows
support is not a planned feature.

2. Ensure you have an up-to-date installation of `clang` and `cmake`. In addition to these, you will want to install `gnuplot` and `libpng-dev`

3. Build with an internet connection. The two main targets are `Dyver` *(for the on-deck server and graphical user interface)* and `DyverTest` *(which contains every unit test)*.
