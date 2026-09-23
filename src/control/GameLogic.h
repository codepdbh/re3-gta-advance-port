#ifndef __GTA_GAMELOGIC_H__
#define __GTA_GAMELOGIC_H__

class CGameLogic
{
public:
	static void InitAtStartOfGame();
	static void PassTime(uint32 time);
	static void SortOutStreamingAndMemory(const CVector &pos);
	static void Update();
	static void RestorePlayerStuffDuringResurrection(class CPlayerPed *pPlayerPed, CVector pos, float angle);

	static uint8 ActivePlayers;
};

#endif // __GTA_GAMELOGIC_H__
