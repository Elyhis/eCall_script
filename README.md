## Steps to follow to use the QT workspace

- Clone the repository
- Open QtCreator 
- Open the CMakeLists.txt file in the eCall_Script folder
- Run the Cmake
- Build the project

## Compilation and Execution

### Ubuntu
Open a Terminal in the current folder and execute the following:
```console
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows
Open a Command Prompt in the current folder and execute the following:
```console
mkdir build
cd build
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### REQUIREMENT

This app use Qt and need some dependencies to work, make sure to import correct .so/.dll using [linuxdeployqt](https://github.com/probonopd/linuxdeployqt) or windeployqt
