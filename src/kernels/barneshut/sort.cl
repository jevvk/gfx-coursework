#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

#define NULL_BODY (-1)
#define LOCK (-2)

#define NUMBER_OF_CELLS 8 // the number of cells per node

__attribute__ ((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
__kernel void sort(
		__global float* _posX, __global float* _posY, __global float* _posZ,
		__global float* _velX, __global float* _velY, __global float* _velZ,
		__global float* _accX, __global float* _accY, __global float* _accZ,
		__global int* _step, __global int* _blockCount, __global int* _bodyCount, __global float* _radius, __global int* _maxDepth,
		__global int* _bottom, __global float* _mass, __global int* _child, __global volatile atomic_int* _start, __global int* _sorted, __global int* _error) {

	int bottom = *_bottom;
	int stepSize = get_local_size(0) * get_num_groups(0);
	int cell = NUMBER_OF_NODES + 1 - stepSize + get_global_id(0);
	// Iterate over all cells assigned to thread
	while (cell >= bottom) {

		// TODO do we need atomics?
		//int start = _start[cell];
		int start = atomic_load_explicit(&_start[cell], memory_order_acquire, memory_scope_device);

		if (start >= 0) {
#pragma unroll NUMBER_OF_CELLS
			for (int i = 0; i < NUMBER_OF_CELLS; ++i) {
				int ch = _child[NUMBER_OF_CELLS * cell + i];
				if (ch >= NBODIES) { // child is a cell

					// TODO do we need atomics?
					// set start id of child
					// _start[ch] = start;
					atomic_store_explicit (&_start[ch], start, memory_order_release, memory_scope_device);

					// TODO don't we need an atomic_work_item_fence for visibility here?
					atomic_work_item_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE, memory_order_seq_cst, memory_scope_device);

					// Add #bodies in subtree
					start += _bodyCount[ch];
				} else if (ch >= 0) { // child is a body
					// Record body in sorted array
					_sorted[start] = ch;

					++start;
				}
			}

			cell -= stepSize; /* Move on to next cell */
		}

		barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE); // throttle
	}
}
