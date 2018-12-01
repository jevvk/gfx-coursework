CC=g++
LIBRARIES=-Llib -lGLU -lGL -lglut

coursework:
	python convert-shader.py _file_simulation_frag src/shaders/simulation.frag src/shaders/simulation.frag.cpp
	python convert-shader.py _file_simulation_vert src/shaders/simulation.vert src/shaders/simulation.vert.cpp
	mkdir -p bin
	$(CC) -fsplit-stack -std=c++17 -w -g -o bin/main $(shell find src/ -name '*.cpp' -type f -print) $(LIBRARIES) -Iinclude -Isrc

clean:
	rm -rf bin
