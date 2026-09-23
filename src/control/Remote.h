#ifndef __GTA_REMOTE_H__
#define __GTA_REMOTE_H__

class CRemote
{
public:
#if GTA_VERSION <= GTA3_PS2_160
	static void Init(void);
#endif
	static void GivePlayerRemoteControlledCar(float, float, float, float, uint16);
	static void TakeRemoteControlledCarFromPlayer(void);
};

#endif // __GTA_REMOTE_H__
