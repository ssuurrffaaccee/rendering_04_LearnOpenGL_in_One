set CONAN_EXE=C:\Users\boc\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.11_qbz5n2kfra8p0\LocalCache\local-packages\Python311\Scripts\conan.exe

set BUILD_TYPE=Release

%CONAN_EXE% profile detect --force 

%CONAN_EXE% install ./win32_conanfile.txt --build=missing  --settings:all=compiler.cppstd=17 --settings:all=build_type=%BUILD_TYPE%
cmake --preset conan-default
cd build/
cmake --build . --config %BUILD_TYPE%
cmake --install .
cd ..