
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

#define MAX_SKILL_LEVEL	 10		//每项技能最高十级
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
	char stName[32];	//物品名称
	char stDesc[256];	//物品描述
	short nProperty;	//物品属性：能吃(0)，能装备(1)，或者可以学习(2)，任务物品(3)
	short nAddAttack;	//增加攻击
	short nAddDefence;	//增加防御
	short nAddHP;		//增加Hp
	short nAddMaxHP;	//增加max HP
	short nAddMP;		//增加Mp
	short nAddMaxMP;	//增加max MP
	short nAddSpeed;	//增加速度
	short nSkillIndex;	//技能书对应的技能
	short nLocation;	//可以装备的位置。0-头，1-身，2-手，3-脚
	short nCost;		//值多少钱
};

struct CRole
{
	struct SkillHaved {
		short nSkillIndex;
		short nSkillLevel;
		short nTimes;	//使用次数
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

	short nSkillNum;						//拥有的技能数量
	SkillHaved MySkills[ MAX_SKILL_NUM ];	//最多拥有9种技能
	short nGoodsNum;						//带的物品数
	short MyGoods[ MAX_GOODS_NUM ];			//最多带9个物品

	short MyEquipment[4];	//0-head, 1-body, 2-hand, 3-foot
	short nMoney;			//钱

	short nTalkScript;		//当与该人物对话时触发的脚本
	short nFailScript;		//当人物被打败时触发的脚本，-1表示没有

};

struct CSpot
{
	char stName[24];
	char stScene[256];
	short nNpcNum;					//npc的个数
	short nNpcs[ MAX_NPC_NUM ];		//最多有10个npc
	short nExitNum;					//出口数目
	short nExits[ MAX_EXIT_NUM ];	//最多有5个出口
	short nGoodsNum;				//可以捡的东西
	short nGoods[ MAX_GOODS_NUM ];

	short nEnterScript;		//进入时触发的脚本
	short nLeaveScript;		//离开时触发的脚本
};

//任务
struct CQuest
{
	char stQuestDesc[128];	//最多64个中文字符
	short nFlagValue;	//0-未接受，1-接受但未完成，2-已经完成
    short nRoleIndex;   //委托者
    short nMapIndex;    //委托人所在地图
};

#ifndef _WIN32_
//int getch(void);
#define getch() getchar()
#endif

#endif
