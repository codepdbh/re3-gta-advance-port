#ifndef __GTA_ROADBLOCKS_H__
#define __GTA_ROADBLOCKS_H__
#include "common.h"

class CVehicle;

class CRoadBlocks
{
public:
	static int16 NumRoadBlocks;
	static int16 RoadBlockObjects[NUMROADBLOCKS];
	static bool InOrOut[NUMROADBLOCKS];

	static void Init(void);
	static void GenerateRoadBlockCopsForCar(CVehicle* pVehicle, int32 roadBlockType, int16 roadBlockNode);
	static void GenerateRoadBlocks(void);
};

#endif // __GTA_ROADBLOCKS_H__
