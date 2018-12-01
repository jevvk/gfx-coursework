# CC=g++ -fsplit-stack -std=c++17 -w -g -o
CC=g++ -std=c++17 -w -g -o
OUT=bin/main
LIBRARIES=-Llib -lGLU -lGL -lglut -lOpenCL
INCLUDES=-Iinclude -Isrc

coursework:
	python scripts/convert-shader.py _file_simulation_frag src/shaders/simulation.frag src/shaders/simulation.frag.cpp
	python scripts/convert-shader.py _file_simulation_vert src/shaders/simulation.vert src/shaders/simulation.vert.cpp
	mkdir -p bin
	$(CC) $(OUT) $(shell find src/ -name '*.cpp' -type f -print) $(LIBRARIES) ${INCLUDES}

clean:
	rm -rf bin
