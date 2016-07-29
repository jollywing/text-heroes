#include "CFight.h"
#include <assert.h>

CFight::CFight(CRole * player, CRole * enemy)
{
	assert( player );
	assert( enemy );

	pPlayer = player;
	pEnemy = enemy;

	oActSlot[0].nRoleID = pPlayer->nID;
	oActSlot[0].nVirtualSpeed = pPlayer->nSpeed;
	oActSlot[1].nRoleID = pEnemy->nID;
	oActSlot[1].nVirtualSpeed = pEnemy->nSpeed;
}

short CFight::DecideWhoAct()
{
	if (oActSlot[0].nVirtualSpeed >= oActSlot[1].nVirtualSpeed)
	{
		oActSlot[1].nVirtualSpeed += pEnemy->nSpeed;
		return oActSlot[0].nRoleID;
	}
	else
	{
		oActSlot[0].nVirtualSpeed += pPlayer->nSpeed;
		return oActSlot[1].nRoleID;
	}	
}

short CFight::UseWhichSkill( CRole * oRole)
{
	short nSelectedSkill = -1;
	short nSkillPower = oRole->nAttack;
	short i;
    return 0;

    /*
	if( oRole->nSkillNum )
	{
		for( i = 0; i< oRole->nSkillNum; ++i )
		{
			if (nSkillPower < (oRole->nAttack + oRole))
		}
	}
	*/
}
