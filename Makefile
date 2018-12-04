# CC=g++ -fsplit-stack -std=c++17 -w -g -o
CC=g++ -std=c++17 -w -g -o
OUT=bin/main
LIBRARIES=-Llib -lGLU -lGL -lglut -ldl -lOpenCL -lm
INCLUDES=-Iinclude -Isrc
EXTRAS=src/glad.c

convert:
	python scripts/file2bytearray.py _file_simulation_frag src/shaders/simulation.frag src/shaders/simulation.frag.cpp
	python scripts/file2bytearray.py _file_simulation_vert src/shaders/simulation.vert src/shaders/simulation.vert.cpp

	python scripts/file2bytearray.py _file_boundingbox_cl src/kernels/barneshut/boundingbox.cl src/kernels/barneshut/boundingbox.cl.cpp
	python scripts/file2bytearray.py _file_buildtree_cl src/kernels/barneshut/buildtree.cl src/kernels/barneshut/buildtree.cl.cpp
	python scripts/file2bytearray.py _file_calculateforce_cl src/kernels/barneshut/calculateforce.cl src/kernels/barneshut/calculateforce.cl.cpp
	python scripts/file2bytearray.py _file_copyvertices_cl src/kernels/barneshut/copyvertices.cl src/kernels/barneshut/copyvertices.cl.cpp
	python scripts/file2bytearray.py _file_integrate_cl src/kernels/barneshut/integrate.cl src/kernels/barneshut/integrate.cl.cpp
	python scripts/file2bytearray.py _file_sort_cl src/kernels/barneshut/sort.cl src/kernels/barneshut/sort.cl.cpp
	python scripts/file2bytearray.py _file_summarizetree_cl src/kernels/barneshut/summarizetree.cl src/kernels/barneshut/summarizetree.cl.cpp

	python scripts/bmp2bytearray.py _file_body_bmp src/textures/body.bmp src/textures/body.bmp.cpp

compile:
	mkdir -p bin
	$(CC) $(OUT) $(shell find src/ -name '*.cpp' -type f -print) $(EXTRAS) $(LIBRARIES) ${INCLUDES}

clean:
	rm src/kernels/barneshut/*.cpp
	rm src/shaders/*.cpp
	rm src/textures/*.cpp

coursework: convert compile clean
