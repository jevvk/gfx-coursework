// TODO better adopt http://developer.amd.com/resources/documentation-articles/articles-whitepapers/opencl-optimization-case-study-simple-reductions/
// TODO for small number of bodies does not quite work!

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

__global volatile float _minX[NUM_WORK_GROUPS], _minY[NUM_WORK_GROUPS],
		_minZ[NUM_WORK_GROUPS];
__global volatile float _maxX[NUM_WORK_GROUPS], _maxY[NUM_WORK_GROUPS],
		_maxZ[NUM_WORK_GROUPS];

#define NUMBER_OF_CELLS 8

/*
 Calculates the bounding box around all bodies.
 */
__attribute__ ((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
__kernel void boundingBox(
		__global volatile float* _posX, __global volatile float* _posY, __global volatile float* _posZ,
		__global float* _velX, __global float* _velY, __global float* _velZ,
		__global float* _accX, __global float* _accY, __global float* _accZ,
		__global int* _step, __global int* _blockCount, __global int* _bodyCount, __global float* _radius, __global int* _maxDepth,
		__global int* _bottom, __global float* _mass, __global int* _child, __global int* _start, __global int* _sorted, __global int* _error) {

	__local volatile float localMinX[WORKGROUP_SIZE], localMinY[WORKGROUP_SIZE], localMinZ[WORKGROUP_SIZE];
	__local volatile float localMaxX[WORKGROUP_SIZE], localMaxY[WORKGROUP_SIZE], localMaxZ[WORKGROUP_SIZE];

	const int localId = get_local_id(0);
	const int groupId = get_group_id(0);
	const int globalId = get_global_id(0);

	// initialize with valid data (in case #bodies < #threads)
	if (localId == 0) {
		localMinX[0] = _posX[0];
		localMinY[0] = _posY[0];
		localMinZ[0] = _posZ[0];
	}

	barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

	localMinX[localId] = localMaxX[localId] = localMinX[0];
	localMinY[localId] = localMaxY[localId] = localMinY[0];
	localMinZ[localId] = localMaxZ[localId] = localMinZ[0];

	// scan all bodies
	// TODO is this get_global_size() ???
	int stepSize = get_local_size(0) * get_num_groups(0);// get_local_size(0) = NUM_THREADS

	for (int i = localId + get_group_id(0) * get_local_size(0); i < NBODIES; i += stepSize) {
		float pos = _posX[i];
		localMinX[localId] = fmin(localMinX[localId], pos);
		localMaxX[localId] = fmax(localMaxX[localId], pos);

		pos = _posY[i];
		localMinY[localId] = fmin(localMinY[localId], pos);
		localMaxY[localId] = fmax(localMaxY[localId], pos);

		pos = _posZ[i];
		localMinZ[localId] = fmin(localMinZ[localId], pos);
		localMaxZ[localId] = fmax(localMaxZ[localId], pos);
	}

	// TODO What kind of input size is needed for this to work?

	for (int offset = get_local_size(0) / 2; offset > 0; offset /= 2) {
		barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

		if (localId < offset) {



			localMinX[localId] = fmin(localMinX[localId], localMinX[localId + offset]);
			localMinY[localId] = fmin(localMinY[localId], localMinY[localId + offset]);
			localMinZ[localId] = fmin(localMinZ[localId], localMinZ[localId + offset]);

			localMaxX[localId] = fmax(localMaxX[localId], localMaxX[localId + offset]);
			localMaxY[localId] = fmax(localMaxY[localId], localMaxY[localId + offset]);
			localMaxZ[localId] = fmax(localMaxZ[localId], localMaxZ[localId + offset]);

		}

		barrier(CLK_LOCAL_MEM_FENCE);
	}


	// Write block result to global memory
	if (localId == 0) {
		_minX[groupId] = localMinX[0];
		_minY[groupId] = localMinY[0];
		_minZ[groupId] = localMinZ[0];

		_maxX[groupId] = localMaxX[0];
		_maxY[groupId] = localMaxY[0];
		_maxZ[groupId] = localMaxZ[0];

		// wait for completion
		mem_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

		// push results to other work items
		atomic_work_item_fence(CLK_GLOBAL_MEM_FENCE, memory_order_seq_cst, memory_scope_device);

		const int lastBlockId = get_num_groups(0) - 1;
		if (lastBlockId == atom_inc(_blockCount)) {

			// last block so combine all results
			for (int i = 0; i <= lastBlockId; ++i) {
				localMinX[0] = fmin(localMinX[0], _minX[i]);
				localMinY[0] = fmin(localMinY[0], _minY[i]);
				localMinZ[0] = fmin(localMinZ[0], _minZ[i]);

				localMaxX[0] = fmax(localMaxX[0], _maxX[i]);
				localMaxY[0] = fmax(localMaxY[0], _maxY[i]);
				localMaxZ[0] = fmax(localMaxZ[0], _maxZ[i]);
			}


			// initialize simulation (create root node)
			const float rootX = 0.5f * (localMinX[0] + localMaxX[0]);
			const float rootY = 0.5f * (localMinY[0] + localMaxY[0]);
			const float rootZ = 0.5f * (localMinZ[0] + localMaxZ[0]);


			*_radius = 0.5f * fmax(fmax(localMaxX[0]- localMinX[0], localMaxY[0] - localMinY[0]), localMaxZ[0] - localMinZ[0]);
			*_bottom = NUMBER_OF_NODES;
			*_blockCount = 0;


			_posX[NUMBER_OF_NODES] = rootX;
			_posY[NUMBER_OF_NODES] = rootY;
			_posZ[NUMBER_OF_NODES] = rootZ;
			_mass[NUMBER_OF_NODES] = -1.0f;
			_start[NUMBER_OF_NODES] = 0;

			// root
#pragma unroll NUMBER_OF_CELLS
			for (int i = 0; i < NUMBER_OF_CELLS; i++) _child[NUMBER_OF_NODES * NUMBER_OF_CELLS + i] = -1;

			(*_step)++;
		}

	}
}

