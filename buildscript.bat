@echo off
cmake -DCMAKE_INSTALL_PREFIX:PATH=%cd%/out/install/x64-Debug -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_CL_64=1 -Ax64 -DCMAKE_BUILD_TYPE=Debug %cd%
cmake -G "Visual Studio 15 2017" %cd%
cmake --build %cd% --config Debug
copy %cd%\out\install\x64-Debug\bin\*.dll %cd%\Debug