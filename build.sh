#!/bin/bash
python convert-shader.py _file_simulation_frag src/shaders/simulation.frag src/shaders/simulation.frag.cpp
python convert-shader.py _file_simulation_vert src/shaders/simulation.vert src/shaders/simulation.vert.cpp

UNIVERSE_FILES="src/universe/SimpleUniverse.cpp"
SIMULATION_FILES="src/simulation/Simulation.cpp src/simulation/CPUNaiveSimulation.cpp src/simulation/CPUBarnesHutSimulation.cpp"
SHADER_FILES="src/shaders/simulation.frag.cpp src/shaders/simulation.vert.cpp"
EXTRA_FILES="src/simulation/Camera.cpp src/Particle.cpp src/simulation/barneshut/Octree.cpp src/simulation/barneshut/BarnesHut.cpp"
LIBS_FLAGS="-Llib -lGLU -lGL -lglut"

g++ -std=c++17 -w -g -o bin/main src/Main.cpp $UNIVERSE_FILES $SIMULATION_FILES $SHADER_FILES $EXTRA_FILES $LIBS_FLAGS -Iinclude -Isrc
