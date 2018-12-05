#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

#define NULL_BODY (-1)
#define LOCK (-2)

// TODO pass as argument
#define WARPSIZE 16

#define NUMBER_OF_CELLS 8 // the number of cells per node

__attribute__ ((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
__kernel void summarizeTree(
		__global float* _posX, __global float* _posY, __global float* _posZ,
		__global float* _velX, __global float* _velY, __global float* _velZ,
		__global float* _accX, __global float* _accY, __global float* _accZ,
		__global int* _step, __global int* _blockCount, __global atomic_int* _bodyCount, __global float* _radius, __global int* _maxDepth,
		__global int* _bottom, __global volatile atomic_float* _mass, __global volatile int* _child, __global volatile int* _start, __global volatile int* _sorted, __global int* _error) {

	// TODO
	__local volatile int localChild[WORKGROUP_SIZE * NUMBER_OF_CELLS];

	int bottom = *_bottom;
	int stepSize = get_local_size(0) * get_num_groups(0);

	// TODO small work group sizes DO NOT WORK WITH this?!
	// TODO alignment to warp size somehow does not work?!
	// 		- This might only work on real devices?

	// align to warp size
	int node = (bottom & (-WARPSIZE)) + get_global_id(0);
	if (node < bottom)
	node += stepSize;

	//int node = bottom + get_global_id(0);


	int missing = 0;

	int cellBodyCount = 0;
	float cellMass;
	float mass;
	float centerX, centerY, centerZ;

	// iterate over all cells assigned to thread
	while (node <= NUMBER_OF_NODES) {
		if (missing == 0) {
			// new cell, so initialize
			cellMass = 0.0f;
			centerX = 0.0f;
			centerY = 0.0f;
			centerZ = 0.0f;
			cellBodyCount = 0;

			// gets incremented when cell is used
			int usedChildIndex = 0;

#pragma unroll NUMBER_OF_CELLS
			for (int childIndex = 0; childIndex < NUMBER_OF_CELLS; childIndex++) {
				int child = _child[node * NUMBER_OF_CELLS + childIndex];

				// is used
				if (child >= 0) {
					if (childIndex != usedChildIndex) {
						_child[NUMBER_OF_CELLS * node + childIndex] = -1;
						_child[NUMBER_OF_CELLS * node + usedChildIndex] = child;
					}

					// Cache missing children
					localChild[WORKGROUP_SIZE * missing + get_local_id(0)] = child;

					mass = atomic_load_explicit(&_mass[child], memory_order_seq_cst, memory_scope_device);
					//mass = _mass[child];

					++missing;

					if (mass >= 0.0f) {
						// child is ready	
						--missing;

						if (child >= NBODIES) {

							// cellBodyCount += _bodyCount[child] - 1;
							cellBodyCount += atomic_load_explicit(&_bodyCount[child], memory_order_seq_cst, memory_scope_device) - 1;

						}

						cellMass += mass;
						centerX += _posX[child] * mass;
						centerY += _posY[child] * mass;
						centerZ += _posZ[child] * mass;

					}
					usedChildIndex++;
				}
			}

			// mem_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);
			cellBodyCount += usedChildIndex;

		}

		if (missing != 0) {
			do {
				int child = localChild[(missing - 1) * WORKGROUP_SIZE + get_local_id(0)];

				//mass = _mass[child];
				mass = atomic_load_explicit(&_mass[child], memory_order_seq_cst, memory_scope_device);


				// Body children can never be missing, so this is a cell
				if (mass >= 0.0f) {
					--missing;
					if (child >= NBODIES) {
						// cellBodyCount +=  _bodyCount[child] - 1;
						cellBodyCount += atomic_load_explicit(&_bodyCount[child], memory_order_seq_cst, memory_scope_device) - 1;
					}

					cellMass += mass;
					centerX += _posX[child] * mass;
					centerY += _posY[child] * mass;
					centerZ += _posZ[child] * mass;
				}
			}while ((mass >= 0.0f) && (missing != 0));

		}

		if (missing == 0) {
			//_bodyCount[node] = cellBodyCount;
			atomic_store_explicit (&_bodyCount[node], cellBodyCount, memory_order_seq_cst, memory_scope_device);
			mass = 1.0f / cellMass;
			_posX[node] = centerX * mass;
			_posY[node] = centerY * mass;
			_posZ[node] = centerZ * mass;

			// make sure data is visible before setting mass
			atomic_work_item_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE, memory_order_seq_cst, memory_scope_device);

			atomic_store_explicit (&_mass[node], cellMass, memory_order_seq_cst, memory_scope_device);
			//_mass[node] = cellMass;

			node += stepSize;// next cell
		}
	}
}
