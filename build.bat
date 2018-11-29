@echo off
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
) else (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
)


python convert-shader.py _file_simulation_frag .\src\shaders\simulation.frag .\src\shaders\simulation.frag.cpp
python convert-shader.py _file_simulation_vert .\src\shaders\simulation.vert .\src\shaders\simulation.vert.cpp

set universeFiles=src\universe\SimpleUniverse.cpp src\universe\TwinCollisionUniverse.cpp
set simulationFiles=src\simulation\Simulation.cpp src\simulation\CPUNaiveSimulation.cpp src\simulation\GPUBarnesHutSimulation.cpp
set shaderFiles=src\shaders\simulation.frag.cpp src\shaders\simulation.vert.cpp
set extraFilelist=src\glad.c src\simulation\Camera.cpp

REM set libraryFlags=-Llib\ -lglut -lglut32 -lglu32 -lopengl32 -luser32

REM g++ -std=c++17 -w -g -o bin\main.exe src\Main.cpp %universeFiles% %simulationFiles% %shaderFiles% %extraFilelist% %libraryFlags% -Iinclude\ -Isrc\

set compilerflags=/Od /Zi /EHsc /std:c++latest /I src /I include
set linkerflags=/OUT:bin\main.exe /LIBPATH:lib glut32.lib glu32.lib opengl32.lib user32.lib

cl.exe %compilerflags% src\Main.cpp %universeFiles% %simulationFiles% %shaderFiles% %extraFilelist% /link %linkerflags%
del bin\*.ilk *.obj *.pdb
