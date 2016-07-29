
#ifndef _GAME_
#define _GAME_

#include "TextParser.h"
#include "global.h"
#include "ScriptEngine.h"

/* game state */
enum GAMESTATE
{
	system_menu,		//�µĹ��£��ɵĻ��䣬�⽨����������
	playing_sys_menu,	//�洢���ȣ���ȡ���ȣ��뿪��Ϸ
	player_adven,		//���ð�գ���·����̸��������״̬����Ʒ������
	show_exits,			//�����ѡ��ȥ����
	talk_with_npc,		//�����ѡ���˭��̸
	attack_npc,			//�����ѡ�񹥻�˭
	watch_npc,			//�����ѡ��۲�˭
	player_state,		//��Ҳ쿴״̬�����ܡ���Ʒ
	list_goods,			//�г���ӵ����Ʒ
	list_skills,		//�г���ӵ�еļ���
	list_quest,			//�鿴����
	use_goods,
	use_skill,
	fighting,			//ս����
	player_act,			//�ֵ�����ж�
	list_free_item,		//�г�����ʰȡ����Ʒ
	pick_item,			//��ȡ��Ʒ
	check_equipment,	//���װ��
	unequip_,			//ж��װ��
	trading,			//����
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
	void Refresh( short nRoleIndex );	//��������ȫ��

	bool AddNpc(short nSpotIndex, short nNpcIndex );
	bool RemoveNpc(short nSpotIndex, short nNpcIndex );
    void AddMoney(short nRoleIdx, short nMoney);
    bool DelMoney(short nRoleIdx, short nMoney);
	bool AddGoods( short nRoleIndex, short nGoodsIndex, bool bVisible );	//������Ʒ
	bool RemoveGoods( short nRoleIndex, short nGoodsLocation, bool bVisible );	//�Ƴ���Ʒ
	short HaveGoods( short nRoleIndex, short nGoodsIndex );		//�Ƿ���ĳ����Ʒ
    short NumOfGoods( short nRoleIdx, short nGoodsIdx);         //ӵ��ĳ����Ʒ������
    void RandomDrop( short nRoleIdx, short nGoodsIdx, short nProb);

	CQuest * pQuests;
    //�ṩ32�������������ʹ��
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
	bool LoadQuestData ( char * pstFileName, char cHeadFlag );	//װ����������
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

	void EndFight( short nNpcIndex );	//��������˭��ʤ����
	short DecideWhoAct();
	short UseWhichSkill( short nRoleIndex );
	void Act( short nRoleIndex, short nSkillIndex, short nEnemyIndex );
	void EscapeOut( short nRoleIndex, short nEnemyIndex);

	short Experience( short nRoleIndex, short nEnemyIndex );		//���㾭��
	void UpdateLevel( short nRoleIndex );							//����
	short Damage( short nRoleIndex, short nEnemyIndex );			//�˺�
	void UpdateSkillLevel( short nRoleIndex, short nSkillIndex );	//���¼��ܵȼ�
	void SkillLevelUp( short nRoleIndex, short nSkillIndex );
	short SkillDamage( short nRoleIndex, short nSkillIndex );		//�����˺�
	short SkillMpCost( short nRoleIndex, short nSkillIndex );

	bool AddSkill( short nRoleIndex, short nSkillIndex );		//���Ӽ���
	bool HaveSkill( short nRoleIndex, short nSkillIndex );		//�Ƿ���ĳ�ּ���
	void LearnSkill ( short nRoleIndex, short nGoodsIndex);		//ѧϰ����
	void Eat( short nRoleIndex, short nGoodsIndex );			//����
	void Equip( short nRoleIndex, short nGoodsLocation );		//װ��
	void UnEquip( short nRoleIndex, short nLocation );			//�Ƴ�װ��
	void UseGoods( short nRoleIndex, short nGoodsLocation );	//ʹ����Ʒ
	void PickGoods( short nRoleIndex, short nGoodsLocation );	//������
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
	//CRole	oPlayer;	//��0��npc�������
	//�򶷵�ʱ���
	struct CActSlot
	{
		short nRoleID;
		short nVirtualSpeed;
	} oActSlot[2];

	//������Դ���ļ���
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
