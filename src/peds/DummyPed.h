#ifndef __GTA_DUMMYPED_H__
#define __GTA_DUMMYPED_H__

#include "Dummy.h"

// actually unused
class CDummyPed : CDummy
{
	int32 pedType;
	int32 unknown;
};

VALIDATE_SIZE(CDummyPed, 0x70);

#endif // __GTA_DUMMYPED_H__
