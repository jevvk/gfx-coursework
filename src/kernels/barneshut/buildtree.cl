#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

#define NULL_BODY (-1)
#define LOCK (-2)

#define NUMBER_OF_CELLS 8 // the number of cells per node

__attribute__ ((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
__kernel void buildTree(
		__global float* _posX, __global float* _posY, __global float* _posZ,
		__global float* _velX, __global float* _velY, __global float* _velZ,
		__global float* _accX, __global float* _accY, __global float* _accZ,
		__global int* _step, __global int* _blockCount, __global volatile int* _bodyCount, __global float* _radius, __global volatile int* _maxDepth,
		__global volatile int* _bottom, __global volatile float* _mass, __global volatile int* _child, __global int* _start, __global int* _sorted, __global int* _error) {

	int localMaxDepth = 1;
	const int stepSize = get_local_size(0) * get_num_groups(0);

	// Cache root data 
	float radius = *_radius;
	float rootX = _posX[NUMBER_OF_NODES];
	float rootY = _posY[NUMBER_OF_NODES];
	float rootZ = _posZ[NUMBER_OF_NODES];

	int childPath;

	bool newBody = true;
	int node;

	// iterate over all bodies assigned to this thread
	int bodyIndex = get_global_id(0);
	while (bodyIndex < NBODIES) {
		float currentR;
		float bodyX, bodyY, bodyZ;
		int depth;

		if (newBody) {
			// new body, so start traversing at root
			newBody = false;

			bodyX = _posX[bodyIndex];
			bodyY = _posY[bodyIndex];
			bodyZ = _posZ[bodyIndex];

			node = NUMBER_OF_NODES;
			depth = 1;
			currentR = radius;

// Determine which child to follow
			childPath = 0;
			if (rootX < bodyX) childPath = 1;
			if (rootY < bodyY) childPath += 2;
			if (rootZ < bodyZ) childPath += 4;
		}

		int childIndex = _child[NUMBER_OF_CELLS * node + childPath];

		// follow path to leaf cell
		while (childIndex >= NBODIES) {
			node = childIndex;
			++depth;
			currentR *= 0.5f;

			// determine which child to follow
			childPath = 0;
			if (_posX[node] < bodyX) childPath = 1;
			if (_posY[node] < bodyY) childPath += 2;
			if (_posZ[node] < bodyZ) childPath += 4;

			childIndex = _child[NUMBER_OF_CELLS * node + childPath];
		}


		if (childIndex != LOCK) {
			int locked = NUMBER_OF_CELLS * node + childPath;
			if (childIndex == atom_cmpxchg(&_child[locked], childIndex, LOCK)) { // try locking
				if (childIndex == NULL_BODY) {
					// no body has been here, so just insert
					_child[locked] = bodyIndex;
				} else {
					int patch = -1;
					// Create new cell(s) and insert the old and new body
					do {
						depth++;
						const int cell = atom_dec(_bottom) - 1;
						if (cell <= NBODIES) {

							// TODO REPORT ERROR
							*_error = 1;
							printf("ERROR ABORT\n");
							printf("\t(%f, %f, %f)\n", bodyX, bodyY, bodyZ);
							*_bottom = NUMBER_OF_NODES;
							return;
						}
						patch = max(patch, cell);

						float x = (childPath & 1) * currentR;
						float y = ((childPath >> 1) & 1) * currentR;
						float z = ((childPath >> 2) & 1) * currentR;

						currentR *= 0.5f;

						// reset						
						_mass[cell] = -1.0f;
						_start[cell] = -1;

						x = _posX[cell] = _posX[node] - currentR + x;
						y = _posY[cell] = _posY[node] - currentR + y;
						z = _posZ[cell] = _posZ[node] - currentR + z;

#pragma unroll NUMBER_OF_CELLS
						for (int k = 0; k < NUMBER_OF_CELLS; k++) _child[cell * NUMBER_OF_CELLS + k] = -1;

						if (patch != cell) {
							_child[NUMBER_OF_CELLS * node + childPath] = cell;
						}

						childPath = 0;
						if (x < _posX[childIndex]) childPath = 1;
						if (y < _posY[childIndex]) childPath += 2;
						if (z < _posZ[childIndex]) childPath += 4;
						_child[NUMBER_OF_CELLS * cell + childPath] = childIndex;

						// next child
						node = cell;
						childPath = 0;
						if (x < bodyX) childPath = 1;
						if (y < bodyY) childPath += 2;
						if (z < bodyZ) childPath += 4;

						childIndex = _child[NUMBER_OF_CELLS * node + childPath];
					}while (childIndex >= 0);

					_child[NUMBER_OF_CELLS * node + childPath] = bodyIndex;

					// TODO memory_order_seq_cst needed?
					// push out 
					atomic_work_item_fence(CLK_GLOBAL_MEM_FENCE, memory_order_seq_cst, memory_scope_device);

					// TODO this needs to be an atomic set in order for other threads to see it correctly??
					//atomic_store_explicit (&_child[locked], patch, memory_order_seq_cst, memory_scope_device);

					//global volatile atomic_int* atomChild = (global volatile atomic_int*)_child;
					//atomic_store(&atomChild[locked], patch);
					_child[locked] = patch;
				}

// TODO is this needed?
				// atomic_work_item_fence(CLK_GLOBAL_MEM_FENCE, memory_order_seq_cst, memory_scope_device);

				localMaxDepth = max(depth, localMaxDepth);

				// move to next body
				bodyIndex += stepSize;
				newBody = true;
			}
		}

		// wait for others to finish to reducue memory pressure
		barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

	}

	atom_max(_maxDepth, localMaxDepth);
}
