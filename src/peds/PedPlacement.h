#ifndef __GTA_PEDPLACEMENT_H__
#define __GTA_PEDPLACEMENT_H__

class CPedPlacement {
public:
	static void FindZCoorForPed(CVector* pos);
	static CEntity* IsPositionClearOfCars(Const CVector*);
	static bool IsPositionClearForPed(CVector*);
};

#endif // __GTA_PEDPLACEMENT_H__
