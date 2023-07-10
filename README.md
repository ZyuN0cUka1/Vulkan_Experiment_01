# Vulkan_Experiment_01
Nothing but a TRY
# Requirement
```
CMake:		3.16~
Vulkan SDK:	1.3.250
```
# Installation
## Windows (CMake gui)
- create a folder `build` inside root directory
- open CMake gui
- enter the root directory
- in the field `where to build the binaries` paste the path to the build folder you created
- press `configure` twice and then `generate` will generate a Visual Studio solution
## MacOS / Linux
To compile the environment, go into the root directory and enter in a terminal:
```
bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make
```
