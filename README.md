### Purpose
This project is meant to display all networking data sent through the local connection between the self-hosted server and client when using the Test mode in Roblox Studio

### Building
The project consists of two parts: the client and the dumper. The dumper builds a DLL that dumps raw packet data for the client to then process.

#### Dumper
```
cd dumper
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

#### Client
Just use cargo:
```
cd client
cargo build
```

### Usage
After building (or downloading the release), you need to inject the DLL into a Studio process. This can be done through a number of publicly available DLL injectors. One such example is the "Inject DLL" misc option in [Process Hacker 2](https://processhacker.sourceforge.io).
Once injected, run the client to connect to it and display the data.
