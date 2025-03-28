### Purpose
This project is meant to display RakNet traffic sent through the local connection between the self-hosted server and client when using the Test mode in Roblox Studio

### Building
This project builds a DLL that dumps raw packet data to a console window.

Use CMake with MSVC:
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
After building (or downloading the release), you need to inject the DLL into a Studio process. This can be done through a number of publicly available DLL injectors, such as the "Load Module" option in [System Informer](https://systeminformer.sourceforge.io/). Once injected, a window should appear with all the networking data being sent over the local Studio socket.
