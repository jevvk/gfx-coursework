// TODO better adopt http://developer.amd.com/resources/documentation-articles/articles-whitepapers/opencl-optimization-case-study-simple-reductions/
// TODO for small number of bodies does not quite work!

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

#include "kernels/nbody/debug.h"

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

	DEBUG_PRINT(("- Info Boundingbox -\n"));
	DEBUG_PRINT(("block_count = %d\n", *_blockCount));
	__local volatile float localMinX[WORKGROUP_SIZE], localMinY[WORKGROUP_SIZE], localMinZ[WORKGROUP_SIZE];
	__local volatile float localMaxX[WORKGROUP_SIZE], localMaxY[WORKGROUP_SIZE], localMaxZ[WORKGROUP_SIZE];

	const int localId = get_local_id(0);
	const int groupId = get_group_id(0);
	const int globalId = get_global_id(0);

	DEBUG_PRINT(("get_num_groups = %d\n", get_num_groups(0)));
	DEBUG_PRINT(("get_local_size = %d\n", get_local_size(0)));
	DEBUG_PRINT(("group_id = %d\n", groupId));
	DEBUG_PRINT(("local_id = %d\n", localId));
	DEBUG_PRINT(("global_id = %d\n", globalId));

	// initialize with valid data (in case #bodies < #threads)
	if (localId == 0) {
		DEBUG_PRINT(("_posX[0] = %f\n", _posX[0]));
		DEBUG_PRINT(("_posY[0] = %f\n", _posY[0]));
		DEBUG_PRINT(("_posZ[0] = %f\n", _posZ[0]));

		localMinX[0] = _posX[0];
		localMinY[0] = _posY[0];
		localMinZ[0] = _posZ[0];
	}

	barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

	localMinX[localId] = localMaxX[localId] = localMinX[0];
	localMinY[localId] = localMaxY[localId] = localMinY[0];
	localMinZ[localId] = localMaxZ[localId] = localMinZ[0];

	DEBUG_PRINT(("localMinX[localId] = %f\n", localMinX[localId]));
	DEBUG_PRINT(("localMinY[localId] = %f\n", localMinY[localId]));
	DEBUG_PRINT(("localMinZ[localId] = %f\n", localMinZ[localId]));

	// scan all bodies
	// TODO is this get_global_size() ???
	int stepSize = get_local_size(0) * get_num_groups(0);// get_local_size(0) = NUM_THREADS
	DEBUG_PRINT(("stepSize = %d\n", stepSize));
	DEBUG_PRINT(("nbodies = %d\n", NBODIES));
	DEBUG_PRINT(("- code boundingbox -\n"));
	for (int i = localId + get_group_id(0) * get_local_size(0); i < NBODIES; i += stepSize) {
		DEBUG_PRINT(("i = %d (%d, %d)\n", i, get_group_id(0), get_local_id(0)));
		float pos = _posX[i];
		DEBUG_PRINT(("particle x %f (%d, %d)\n", pos, get_group_id(0), get_local_id(0)));
		localMinX[localId] = fmin(localMinX[localId], pos);
		DEBUG_PRINT(("minX: %f (%d, %d)\n", localMinX[localId], get_group_id(0), get_local_id(0)));
		localMaxX[localId] = fmax(localMaxX[localId], pos);
		DEBUG_PRINT(("maxX: %f (%d, %d)\n", localMaxX[localId], get_group_id(0), get_local_id(0)));

		pos = _posY[i];
		DEBUG_PRINT(("particle y %f (%d, %d)\n", pos, get_group_id(0), get_local_id(0)));
		localMinY[localId] = fmin(localMinY[localId], pos);
		DEBUG_PRINT(("minY: %f (%d, %d)\n", localMinY[localId], get_group_id(0), get_local_id(0)));
		localMaxY[localId] = fmax(localMaxY[localId], pos);
		DEBUG_PRINT(("maxY: %f (%d, %d)\n", localMaxY[localId], get_group_id(0), get_local_id(0)));

		pos = _posZ[i];
		DEBUG_PRINT(("particle z %f (%d, %d)\n", pos, get_group_id(0), get_local_id(0)));
		localMinZ[localId] = fmin(localMinZ[localId], pos);
		DEBUG_PRINT(("minZ: %f (%d, %d)\n", localMinZ[localId], get_group_id(0), get_local_id(0)));
		localMaxZ[localId] = fmax(localMaxZ[localId], pos);
		DEBUG_PRINT(("maxZ: %f (%d, %d)\n", localMaxZ[localId], get_group_id(0), get_local_id(0)));
	}

	// TODO What kind of input size is needed for this to work?

	for (int offset = get_local_size(0) / 2; offset > 0; offset /= 2) {
		DEBUG_PRINT(("offset: %d (%d, %d)\n", offset, get_group_id(0), get_local_id(0)));
		barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

		if (localId < offset) {

			DEBUG_PRINT(("localId(%d) < offset(%d)\n", get_local_id(0), offset));

			DEBUG_PRINT(("localMinX(local): %f localMinX(local + off):\n", localMinX[localId], localMinX[localId + offset]));
			DEBUG_PRINT(("localMinY(local): %f localMinY(local + off):\n", localMinY[localId], localMinY[localId + offset]));
			DEBUG_PRINT(("localMinZ(local): %f localMinZ(local + off):\n", localMinZ[localId], localMinZ[localId + offset]));
			DEBUG_PRINT(("localMaxX(local): %f localMaxX(local + off):\n", localMaxX[localId], localMaxX[localId + offset]));
			DEBUG_PRINT(("localMaxY(local): %f localMaxY(local + off):\n", localMaxY[localId], localMaxY[localId + offset]));
			DEBUG_PRINT(("localMaxZ(local): %f localMaxZ(local + off):\n", localMaxZ[localId], localMaxZ[localId + offset]));

			localMinX[localId] = fmin(localMinX[localId], localMinX[localId + offset]);
			localMinY[localId] = fmin(localMinY[localId], localMinY[localId + offset]);
			localMinZ[localId] = fmin(localMinZ[localId], localMinZ[localId + offset]);

			localMaxX[localId] = fmax(localMaxX[localId], localMaxX[localId + offset]);
			localMaxY[localId] = fmax(localMaxY[localId], localMaxY[localId + offset]);
			localMaxZ[localId] = fmax(localMaxZ[localId], localMaxZ[localId + offset]);

			DEBUG_PRINT(("minX: %f\n", localMinX[localId]));
			DEBUG_PRINT(("minY: %f\n", localMinY[localId]));
			DEBUG_PRINT(("minZ: %f\n", localMinZ[localId]));
			DEBUG_PRINT(("maxX: %f\n", localMaxX[localId]));
			DEBUG_PRINT(("maxY: %f\n", localMaxY[localId]));
			DEBUG_PRINT(("maxZ: %f\n", localMaxZ[localId]));
		}

		barrier(CLK_LOCAL_MEM_FENCE);
	}

	DEBUG_PRINT(("localId: %d\n", localId));

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

			DEBUG_PRINT(("RESULT\n"));
			DEBUG_PRINT(("minX: %f\n", localMinX[0]));
			DEBUG_PRINT(("minY: %f\n", localMinY[0]));
			DEBUG_PRINT(("minZ: %f\n", localMinZ[0]));
			DEBUG_PRINT(("maxX: %f\n", localMaxX[0]));
			DEBUG_PRINT(("maxY: %f\n", localMaxY[0]));
			DEBUG_PRINT(("maxZ: %f\n", localMaxZ[0]));

			// initialize simulation (create root node)
			const float rootX = 0.5f * (localMinX[0] + localMaxX[0]);
			const float rootY = 0.5f * (localMinY[0] + localMaxY[0]);
			const float rootZ = 0.5f * (localMinZ[0] + localMaxZ[0]);

			DEBUG_PRINT(("rootX: %f\n", rootX));
			DEBUG_PRINT(("rootY: %f\n", rootY));
			DEBUG_PRINT(("rootZ: %f\n", rootZ));

			*_radius = 0.5f * fmax(fmax(localMaxX[0]- localMinX[0], localMaxY[0] - localMinY[0]), localMaxZ[0] - localMinZ[0]);
			*_bottom = NUMBER_OF_NODES;
			*_blockCount = 0;

			DEBUG_PRINT(("bottom: %d\n", *_bottom));

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

