
#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifdef _WIN32_
#include <windows.h>
#include <conio.h>
#else
#define HANDLE long
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

#ifdef _WIN32_
#define QUEST_COLOR		2
#define SYSTEM_COLOR	15
#define WARNING_COLOR	12
#define DESCRIPTION_COLOR	7
#define SKILL_NAME_COLOR	9
#define SPOT_NAME_COLOR		10
#define	NPC_NAME_COLOR		11
#define ITEM_NAME_COLOR		14
#define SYSTEM_HINT_COLOR	13
#else
#define QUEST_STRING "\033[0;35m%s\033[0m"    //voilet
#define SYSTEM_STRING "\033[0;37m%s\033[0m"   //white
#define WARNING_STRING "\033[0;31m%s\033[0m"  //red
#define DESCRIPTION_STRING "\033[0;32m%s\033[0m"  //green
#define SKILL_NAME_STRING "\033[0;34m%s\033[0m"   //blue
#define SPOT_NAME_STRING "\033[0;36m%s\033[0m"    //seablue
#define NPC_NAME_STRING "\033[0;33m%s\033[0m"     //yellow
#define NICK_NAME_STRING "\033[7;36m%s\033[0m"    //seablue
#define ITEM_NAME_STRING "\033[0;32m%s\033[0m"  //green
#define SYSTEM_HINT_STRING "\033[4;37m%s\033[0m"   //white
#endif

#ifdef _WIN32_
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define MAX_SKILL_LEVEL	 10		//ÿ������ʮ��
#define MAX_GOODS_NUM	9
#define MAX_SKILL_NUM	9
#define MAX_NPC_NUM		9
#define MAX_EXIT_NUM	9
#define VAR_NUM 32
#define SKILL_KIND 4
#define SKILL_LVL 5

/*
enum SKILLCLASS {
    SWORD, BLADE, LONG, HAND
};
*/

struct CSkill
{
	char stName[32];
	char stDesc[256];
	short nPower;
	short nMpCost;
    short nLvlUp;  // n * nLvlUp to level up
    short nSklCls; // 0 - sword, 1 - blade, 2 - long, 3 -hand
	short nPreExp; // experience to study this skill
    short nAddExp; // each perform add experience lvl * nAddExp

    short nAddHp;
    short nAddMp;
    short nAddAttack;
    short nAddDef;
    short nAddSpeed;
};

struct CGoods
{
	char stName[32];	//��Ʒ����
	char stDesc[256];	//��Ʒ����
	short nProperty;	//��Ʒ���ԣ��ܳ�(0)����װ��(1)�����߿���ѧϰ(2)��������Ʒ(3)
	short nAddAttack;	//���ӹ���
	short nAddDefence;	//���ӷ���
	short nAddHP;		//����Hp
	short nAddMaxHP;	//����max HP
	short nAddMP;		//����Mp
	short nAddMaxMP;	//����max MP
	short nAddSpeed;	//�����ٶ�
	short nSkillIndex;	//�������Ӧ�ļ���
	short nLocation;	//����װ����λ�á�0-ͷ��1-��2-�֣�3-��
	short nCost;		//ֵ����Ǯ
};

struct CRole
{
	struct SkillHaved {
		short nSkillIndex;
		short nSkillLevel;
		short nTimes;	//ʹ�ô���
	};

    short nID;

	char stName[32];
	char stDescription[256];
    char stNick[32];

    short nExps[SKILL_KIND];
	short nHP;
	short nMaxHP;
	short nMP;
	short nMaxMP;
	short nAttack;
	short nDefend;
	short nSpeed;

	short nSkillNum;						//ӵ�еļ�������
	SkillHaved MySkills[ MAX_SKILL_NUM ];	//���ӵ��9�ּ���
	short nGoodsNum;						//������Ʒ��
	short MyGoods[ MAX_GOODS_NUM ];			//����9����Ʒ

	short MyEquipment[4];	//0-head, 1-body, 2-hand, 3-foot
	short nMoney;			//Ǯ

	short nTalkScript;		//���������Ի�ʱ�����Ľű�
	short nFailScript;		//�����ﱻ���ʱ�����Ľű���-1��ʾû��

};

struct CSpot
{
	char stName[24];
	char stScene[256];
	short nNpcNum;					//npc�ĸ���
	short nNpcs[ MAX_NPC_NUM ];		//�����10��npc
	short nExitNum;					//������Ŀ
	short nExits[ MAX_EXIT_NUM ];	//�����5������
	short nGoodsNum;				//���Լ�Ķ���
	short nGoods[ MAX_GOODS_NUM ];

	short nEnterScript;		//����ʱ�����Ľű�
	short nLeaveScript;		//�뿪ʱ�����Ľű�
};

//����
struct CQuest
{
	char stQuestDesc[128];	//���64�������ַ�
	short nFlagValue;	//0-δ���ܣ�1-���ܵ�δ��ɣ�2-�Ѿ����
    short nRoleIndex;   //ί����
    short nMapIndex;    //ί�������ڵ�ͼ
};

#ifndef _WIN32_
//int getch(void);
#define getch() getchar()
#endif

#endif
