### Purpose
This project is meant to display all networking data sent through the local connection between the self-hosted server and client when using the Test mode in Roblox Studio

### Building
There are currently no custom build options and building can be done as normal with CMake and MSVC. Running the following in the root project directory should suffice:
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Clang and Ninja should also work without any additional setup (assuming CMake is able to locate them)
```
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build . 
```

### Usage
After building (or downloading the release), all that is needed is a way to inject the DLL. This can be done through a number of publicly available DLL injectors. One such example is the "Inject DLL" misc option in [Process Hacker 2](https://processhacker.sourceforge.io).
