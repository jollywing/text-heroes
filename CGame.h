
#ifndef _GAME_
#define _GAME_

#include "TextParser.h"
#include "global.h"
#include "ScriptEngine.h"

/* game state */
enum GAMESTATE
{
	system_menu,		//新的故事，旧的回忆，封建归隐，关于
	playing_sys_menu,	//存储进度，读取进度，离开游戏
	player_adven,		//玩家冒险：赶路，交谈，攻击，状态，物品，进度
	show_exits,			//让玩家选择去哪里
	talk_with_npc,		//让玩家选择和谁交谈
	attack_npc,			//让玩家选择攻击谁
	watch_npc,			//让玩家选择观察谁
	player_state,		//玩家察看状态、技能、物品
	list_goods,			//列出所拥有物品
	list_skills,		//列出所拥有的技能
	list_quest,			//查看任务
	use_goods,
	use_skill,
	fighting,			//战斗中
	player_act,			//轮到玩家行动
	list_free_item,		//列出可以拾取的物品
	pick_item,			//捡取物品
	check_equipment,	//检查装备
	unequip_,			//卸除装备
	trading,			//交易
	buy_goods,
	sell_goods
};


class CGame
{
public:

	CGame( HANDLE );
	~CGame();
	void RunGame();

	char * GetLogoBuffer() { return stLogoBuffer;}
	char * GetMapBuffer() { return stMapBuffer;	}

	void SystemOut( const char * stOutString );
	void SystemHint ( const char * stHintString );
	void SystemWarning(const char * stWarningString);
	void SystemPause();

    short GetUserInput();
	short GetUserInput( short nMin, short nMax );

	void ShowNpcName( short nNpcIndex);
	void InitFight( short nNpcIndex );
	void Refresh( short nRoleIndex );	//生命内力全满

	bool AddNpc(short nSpotIndex, short nNpcIndex );
	bool RemoveNpc(short nSpotIndex, short nNpcIndex );
    void AddMoney(short nRoleIdx, short nMoney);
    bool DelMoney(short nRoleIdx, short nMoney);
	bool AddGoods( short nRoleIndex, short nGoodsIndex, bool bVisible );	//增加物品
	bool RemoveGoods( short nRoleIndex, short nGoodsLocation, bool bVisible );	//移除物品
	short HaveGoods( short nRoleIndex, short nGoodsIndex );		//是否有某种物品
    short NumOfGoods( short nRoleIdx, short nGoodsIdx);         //拥有某种物品的数量
    void RandomDrop( short nRoleIdx, short nGoodsIdx, short nProb);

	CQuest * pQuests;
    //提供32个变量供设计者使用
    short nVars[VAR_NUM];
    bool iRunning;

	struct CGameEnvironment
	{
		short nCurrentMap;			//current map index
		short nCurrentNpc;
		short nCurrentEnemy;
		short nCurrentSkill;
		short nCurrentGoods;
		short nCurrentEquipment;
		bool bFighting;
		GAMESTATE eState;			//game state

	}	oGameEnv;


private:

	bool LoadResFileName(const char* pstFileName, const char cHeadFlag);
	bool LoadSpotsData( const char * pstFileName, const char cHeadFlag );	//load spots data
	bool LoadNpcsData( char * pstFileName, char cHeadFlag );	// load npc data
	bool LoadSkillsData ( const char * pstFileName, const char cHeadFlag);	//load skill data
	bool LoadGoodsData ( char * pstFileName, char cHeadFlag );	//load goods data
	bool LoadQuestData ( char * pstFileName, char cHeadFlag );	//装载任务数据
    void LoadNickData( const char * pstFileName, const char cHeadFlag);

	bool ReadWholeTextFile( char * stFileName , char * stBuffer);
	void ShowBuffer(  char * stBuffer );

	void PrintNpcs( short nSpotIndex );
	void PrintGoodsOfNpc( short nNpcIndex );

    void ShowNickName( short nNpcIndex);
    void ShowNpcState( short nNpcIndex);
    void UpdateNick(short nRoleIdx);
	void ShowNpcDetail(short nNpcIndex);
	void ShowNpcDetailX ( short nNpcIndex );	//show npcs
	void ShowSpotName( short nSpotIndex );
	void ShowSpotDetail( short nSpotIndex );
	void ShowSkillName( short nSkillIndex);
	void ShowSkillDetail ( short nSkillIndex );		//show specific skill
	void ShowGoodsName( short nGoodsIndex );
	void ShowGoodsDetail ( short nGoodsIndex);
	void ShowQuests( bool bCompleted );
	void ShowEquipment( short nRoleIndex, short nLocation );

	bool LoadPlayerData( bool bNewGame );
	bool SavePlayerData();

	void EndFight( short nNpcIndex );	//参数表明谁是胜利者
	short DecideWhoAct();
	short UseWhichSkill( short nRoleIndex );
	void Act( short nRoleIndex, short nSkillIndex, short nEnemyIndex );
	void EscapeOut( short nRoleIndex, short nEnemyIndex);

	short Experience( short nRoleIndex, short nEnemyIndex );		//计算经验
	void UpdateLevel( short nRoleIndex );							//升级
	short Damage( short nRoleIndex, short nEnemyIndex );			//伤害
	void UpdateSkillLevel( short nRoleIndex, short nSkillIndex );	//更新技能等级
	void SkillLevelUp( short nRoleIndex, short nSkillIndex );
	short SkillDamage( short nRoleIndex, short nSkillIndex );		//技能伤害
	short SkillMpCost( short nRoleIndex, short nSkillIndex );

	bool AddSkill( short nRoleIndex, short nSkillIndex );		//增加技能
	bool HaveSkill( short nRoleIndex, short nSkillIndex );		//是否有某种技能
	void LearnSkill ( short nRoleIndex, short nGoodsIndex);		//学习技能
	void Eat( short nRoleIndex, short nGoodsIndex );			//服用
	void Equip( short nRoleIndex, short nGoodsLocation );		//装备
	void UnEquip( short nRoleIndex, short nLocation );			//移除装备
	void UseGoods( short nRoleIndex, short nGoodsLocation );	//使用物品
	void PickGoods( short nRoleIndex, short nGoodsLocation );	//捡起东西
	void RmGoodsFromSpot( short nSpotIndex, short nGoodsLocation );
	void AddGoodsToSpot( short nSpotIndex, short nGoodsIndex );
	void BuyGoods( short nRoleIndex, short nGoodsIndex );
	void SellGoods( short nRoleIndex, short nGoodsLocation);

	short GetAddedAttack( short nRoleIndex );
	short GetAddedDefend( short nRoleIndex );
	short GetAddedSpeed( short nRoleIndex );


private:
	short nSpotsNum;
	short nNpcsNum;
	short nSkillsNum;
	short nGoodsNum;
	short nQuestsNum;
	HANDLE hConsoleHandle;

    ScriptEngine * pScriptEngine;
	CRole * pNpcs;
	CSpot * pSpots;
	CSkill * pSkills;
	CGoods * pGoods;
	//CRole	oPlayer;	//第0个npc就是玩家
	//打斗的时间槽
	struct CActSlot
	{
		short nRoleID;
		short nVirtualSpeed;
	} oActSlot[2];

	//各种资源的文件名
	struct CResFiles
	{
		char stSkillFile[128];
		char stGoodsFile[128];
		char stRoleFile[128];
		char stSpotFile[128];
		char stQuestFile[128];
		char stScriptFile[128];
		char stMapFile[128];
		char stLogoFile[128];
        char stNickFile[128];

	} oResFiles;

	char stLogoBuffer[2048];
	char stMapBuffer[1024];

    char stSkillNicks[SKILL_KIND][16];
    char stLvlNicks[SKILL_LVL][16];
    short nNickExp[SKILL_LVL];

};

#endif
