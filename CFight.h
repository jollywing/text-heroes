
#ifndef _FIGHT_
#define _FIGHT_

#include "global.h"

class CFight
{
public:
	CFight( CRole * player, CRole * enemy);
	~CFight();
	short DecideWhoAct();
	short UseWhichSkill(CRole * oRole);

	struct CActSlot
	{
		short nRoleID;
		short nVirtualSpeed;
	};
private:
	CRole* pPlayer;
	CRole* pEnemy;
	CActSlot oActSlot[2];

};
#endif

