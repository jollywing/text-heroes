
#include "CGame.h"
#include <assert.h>
#include <string.h>

CGame::CGame( HANDLE hConsole)
{
	hConsoleHandle = hConsole;

	pSkills = NULL;
	pGoods = NULL;
	pNpcs = NULL;
	pSpots = NULL;
	pQuests = NULL;

    pScriptEngine = new ScriptEngine();

	oGameEnv.bFighting = false;
}

CGame::~CGame()
{
	if(pScriptEngine)
	{
		delete pScriptEngine;
		pScriptEngine = NULL;
	}

	if( pQuests )
	{
		delete [] pQuests;
		pQuests = NULL;
	}

	if( pSpots )
	{
		delete [] pSpots;
		pSpots = NULL;
	}

	if( pNpcs )
	{
		delete [] pNpcs;
		pNpcs = NULL;
	}

	if( pSkills )
	{
		delete [] pSkills;
		pSkills = NULL;
	}

	if( pGoods )
	{
		delete [] pGoods;
		pGoods = NULL;
	}
}

bool CGame::ReadWholeTextFile(char * stFileName, char * stBuffer )
{
	//要想准确的计算文件长度，要用ios::binary。
	ifstream hTextFile( stFileName, ios::binary  );

	if ( !hTextFile )
	{
		cerr << "open whole text file failed !"<<endl;
		return false;
	}

	//得到文件长度
	int nFileLength;
	hTextFile.seekg(0, ios::end);
	nFileLength = hTextFile.tellg();

	//将指针返回文件开头
	hTextFile.seekg(0);

	//读取整个文件到buffer中
	try
	{
		hTextFile.read(stBuffer, nFileLength );
	}
	catch ( ... ) {
		cout << "read whole text file error !" << endl;
		return false;
	}

	stBuffer[nFileLength] = '\0';
    return true;
}

void CGame::ShowBuffer( char * stBuffer )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, ITEM_NAME_COLOR|0);
	cout << stBuffer << endl << endl;
#else
    printf(ITEM_NAME_STRING, stBuffer);
    cout << endl << endl;
#endif
}

void CGame::PrintNpcs( short nSpotIndex )
{
    short i;
	for( i = 0; i < pSpots[nSpotIndex].nNpcNum; ++ i)
	{
		cout << i+1 <<".";
		ShowNpcName( pSpots[nSpotIndex].nNpcs[i] );
		cout << endl;
	}
	cout << i+1 << "." << "取消" <<endl << endl;
}

//显示角色身上的物品
void CGame::PrintGoodsOfNpc( short nNpcIndex )
{
    short i;
	for( i = 0; i< pNpcs[nNpcIndex].nGoodsNum; ++i)
	{
		cout << i+1 << ".";
		ShowGoodsName(pNpcs[nNpcIndex].MyGoods[i]);

		cout << "\t\t";
		if( oGameEnv.eState == buy_goods )
			 cout << pGoods[ pNpcs[nNpcIndex].MyGoods[i] ].nCost << "文";
		else if ( oGameEnv.eState == sell_goods )
			cout << pGoods[ pNpcs[nNpcIndex].MyGoods[i] ].nCost / 2 << "文";

		cout << endl;
	}
	cout << i+1 << ".取消" <<endl <<endl;
}


short CGame::GetUserInput()
{
	char str[1];
	str[0] = getch();
	return atoi(str);
}

short CGame::GetUserInput(short nMin, short nMax )
{
	short n = GetUserInput();

	while( n < nMin || n > nMax )
		n = GetUserInput();

	return n;
}

/* 读取玩家数据 */
bool CGame::LoadPlayerData( bool bNewGame )
{
	//装载npc和场景数据
	if(!LoadNpcsData( oResFiles.stRoleFile, '#')){
        SystemWarning("装载NPC数据失败！");
        return false;
    }
	if(!LoadSpotsData( oResFiles.stSpotFile, '#')){
        SystemWarning("装载场景信息失败！");
        return false;
    }

	//如果是新游戏
	if (bNewGame)
	{
		short i;

		//一开始是在主角家中
		oGameEnv.nCurrentMap = 0;

        /*
		//读取玩家数据后，清空所有任务标志位
		for ( i = 0; i< nQuestsNum; ++i )
		{
			pQuests[i].nFlagValue = 0;
		}
        */
	}

	//如果是读取存档
	else
	{
		//打开存档文件
		ifstream hSaveFile( "save.dat");
		if (!hSaveFile)
		{
			cerr << "open save file failed !" << endl;
			return false;
		}

		//读取玩家数据
		hSaveFile.read( reinterpret_cast<char *> (&pNpcs[0]), sizeof (CRole) );
		hSaveFile.read( reinterpret_cast<char *> (&oGameEnv.nCurrentMap), sizeof(short));

		short i, j;

		//读取任务标志
		for( i= 0; i < nQuestsNum; ++ i )
			hSaveFile.read( reinterpret_cast<char *>(& pQuests[i].nFlagValue), sizeof(short));
        for (i=0; i < VAR_NUM; ++i)
            hSaveFile.read( reinterpret_cast<char *>(&nVars[i]), sizeof(short));

		//读取各个场景的npc和物品
        /*
		for( i= 0; i < nSpotsNum; ++i )
		{
			hSaveFile.read( reinterpret_cast<char *>(&pSpots[i].nNpcNum), sizeof(short));
			for(j = 0; j < pSpots[i].nNpcNum; ++j )
				hSaveFile.read( reinterpret_cast<char *>(&pSpots[i].nNpcs[j]), sizeof(short) );

			hSaveFile.read( reinterpret_cast<char *>(&pSpots[i].nGoodsNum), sizeof(short));
			for(j = 0; j < pSpots[i].nGoodsNum; ++j )
				hSaveFile.read( reinterpret_cast<char *>(&pSpots[i].nGoods[j]), sizeof(short) );
		}
        */
	}
    UpdateNick(0);
    return true;
}

/* 存储玩家数据 */
bool CGame::SavePlayerData()
{
	//打开存档文件
	ofstream hSaveFile( "save.dat");
	if (!hSaveFile)
	{
		cerr << "open save file failed !" << endl;
		return false;
	}

	//写入玩家属性数据
	hSaveFile.write( reinterpret_cast<char *> (&pNpcs[0]), sizeof (CRole) );
	hSaveFile.write( reinterpret_cast<char *> (&oGameEnv.nCurrentMap), sizeof(short));

	short i, j;

	//写入任务标志
	for( i= 0; i < nQuestsNum; ++ i )
		hSaveFile.write( reinterpret_cast<char *>(& pQuests[i].nFlagValue), sizeof(short) );
    for(i =0; i < VAR_NUM; ++i)
        hSaveFile.write( reinterpret_cast<char *>(& nVars[i]), sizeof(short) );

	//写入各个场景npc和 free item的情况
    /*
	for( i= 0; i < nSpotsNum; ++i )
	{
		hSaveFile.write( reinterpret_cast<char *>(&pSpots[i].nNpcNum), sizeof(short) );
		for(j = 0; j < pSpots[i].nNpcNum; ++j )
			hSaveFile.write( reinterpret_cast<char *>(&pSpots[i].nNpcs[j]), sizeof(short) );

		hSaveFile.write( reinterpret_cast<char *>(&pSpots[i].nGoodsNum), sizeof(short));
		for(j = 0; j < pSpots[i].nGoodsNum; ++j )
			hSaveFile.write( reinterpret_cast<char *>(&pSpots[i].nGoods[j]), sizeof(short) );
	}
    */

    return true;
}

/* 运行游戏，这个函数必须在文本文件中的数据都已经读入才能被调用 */
void CGame::RunGame()
{
	short nInput;

	if(!LoadResFileName( "res_index.txt", '#'))
        return;

	if(!LoadSkillsData( oResFiles.stSkillFile, '#'))
        return;
	if(!LoadGoodsData( oResFiles.stGoodsFile, '#'))
        return;
	if(!LoadQuestData( oResFiles.stQuestFile, '#' ))
        return;
	if(!pScriptEngine->LoadScripts( oResFiles.stScriptFile, '#' ))
        return;
	if(!ReadWholeTextFile( oResFiles.stMapFile, stMapBuffer ))
        return;
	if(!ReadWholeTextFile( oResFiles.stLogoFile, stLogoBuffer ))
        return;
    LoadNickData( oResFiles.stNickFile, '#');

	oGameEnv.eState = system_menu ;

    iRunning = true;

	while(iRunning)
	{
		short i;

		switch( oGameEnv.eState ) {

		//显示游戏的标题和系统菜单
		case system_menu :

			system(CLEAR);
			ShowBuffer( stLogoBuffer );
			//RunScripts( 0 );
			//cout << endl;
			SystemOut( "1. 新的征途	2.再战江湖	3.封剑归隐" );

			nInput = GetUserInput(1, 3);

			//如果是新建游戏
			if ( nInput == 1)
			{
				SystemHint("读取玩家数据...");
				if (!LoadPlayerData( true )){
                    iRunning = false;
                }

				cout << "你名叫";
				ShowNpcDetail( 0 );
				SystemPause();
				oGameEnv.eState = player_adven ;
			}

			//如果是读取存档
			else if( nInput == 2 )
			{
				SystemHint("读取玩家数据...");
				if(!LoadPlayerData( false )){
                    iRunning = false;
                }
				oGameEnv.eState = player_adven;
			}

			//如果要离开游戏
			else if( nInput == 3 )
				return;

			break;

		//玩家冒险状态
		case player_adven :
			system(CLEAR);	//清屏
			ShowSpotDetail(oGameEnv.nCurrentMap);
            ShowNpcState(0);
			SystemOut( "1.交谈	2.观察	3.攻击	4.拾取	5.赶路	6.状态	7.系统	8.地图" );

			nInput = GetUserInput(1, 8);

			//和人交谈
			if (nInput == 1)
			{
				if ( pSpots[oGameEnv.nCurrentMap].nNpcNum == 0 )
					SystemWarning("这里没有人可以交谈！");
				else
					oGameEnv.eState = talk_with_npc ;
			}

			//观察别人
			else if( nInput == 2 )
			{
				if ( pSpots[oGameEnv.nCurrentMap].nNpcNum == 0 )
					SystemWarning("这里没有人可以观察！");
				else
					oGameEnv.eState = watch_npc;
			}

			//攻击别人
			else if( nInput == 3)
			{
				if ( pSpots[oGameEnv.nCurrentMap].nNpcNum == 0 )
					SystemWarning("这里没有人可以攻击。");
				else
					oGameEnv.eState = attack_npc;
			}

			//拣东西
			else if( nInput == 4 )
			{
				if ( pSpots[oGameEnv.nCurrentMap].nGoodsNum == 0 )
					SystemWarning( "这里没有可拣的东西。");
				else
					oGameEnv.eState = list_free_item;
			}

			//赶路
			else if( nInput == 5){
				if( pSpots[oGameEnv.nCurrentMap].nLeaveScript >= 0 ){
					pScriptEngine->RunScripts( pSpots[oGameEnv.nCurrentMap].nLeaveScript, this );
					oGameEnv.eState = player_adven;
				}
				else
					oGameEnv.eState = show_exits;
			}

			//玩家察看状态
			else if( nInput == 6)
				oGameEnv.eState = player_state;

			//系统菜单
			else if (nInput == 7 )
			{
				oGameEnv.eState = playing_sys_menu;
			}

			//查看地图
			else if (nInput == 8 )
			{
				ShowBuffer( stMapBuffer);
				SystemPause();
				oGameEnv.eState = player_adven;
			}

			break;

		//玩家察看状态
		case player_state:
			SystemHint("要察看什么？");
			SystemOut( "1.属性	2.物品	3.装备	4.技能	5.任务	6.取消" );

			nInput = GetUserInput(1, 6);

			//如果要察看属性
			if ( nInput == 1 )
			{
				ShowNpcDetailX( 0 );
				SystemPause();
				oGameEnv.eState = player_state;
			}

			//查看身上的物品
			else if ( nInput == 2 )
			{
				if ( pNpcs[0].nGoodsNum)
					oGameEnv.eState = list_goods;
				else
				{
					SystemWarning("你身上没有任何物品.");
					oGameEnv.eState = player_state;
				}
			}

			//查看装备
			else if( nInput == 3 )
				oGameEnv.eState = check_equipment;

			//查看拥有的技能
			else if ( nInput == 4 )
			{
				if ( pNpcs[0].nSkillNum)
					oGameEnv.eState = list_skills;
				else
				{
					SystemWarning("你还没学会任何技能。");
					oGameEnv.eState = player_state;
				}
			}

			//查看任务
			else if ( nInput == 5 )
				oGameEnv.eState = list_quest;

			//取消
			else
				oGameEnv.eState = player_adven;

			break;

		//选择移动的目的地
		case show_exits:
			SystemHint("要去哪里？");

			//显示可以去的地方
			for ( i = 0; i < pSpots[ oGameEnv.nCurrentMap ].nExitNum; ++i )
			{
				cout << (i+1) << ".";
				ShowSpotName(pSpots[ oGameEnv.nCurrentMap].nExits[i] );
				cout << endl;
			}
			cout << i+1 << ".取消" <<endl << endl;

			nInput = GetUserInput( 1, i+1 );

			//取消
			if ( nInput == i+ 1 )
				oGameEnv.eState = player_adven;

			//转换到目的地
			else
			{
				oGameEnv.nCurrentMap = pSpots[oGameEnv.nCurrentMap].nExits[ nInput - 1 ];
				pScriptEngine->RunScripts( pSpots[oGameEnv.nCurrentMap].nEnterScript,
                    this);
				oGameEnv.eState = player_adven;
			}

			break;

		//和人交谈
		case talk_with_npc:
			SystemHint( "要和谁交谈？" );
			PrintNpcs( oGameEnv.nCurrentMap );

			nInput = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].nNpcNum + 1 );

			//取消
			if (nInput == pSpots[oGameEnv.nCurrentMap].nNpcNum + 1 )
				oGameEnv.eState = player_adven;

			//与选中的人物对话
			else
			{
				oGameEnv.nCurrentNpc = pSpots[oGameEnv.nCurrentMap].nNpcs[nInput -1];
				pScriptEngine->RunScripts( pNpcs [ oGameEnv.nCurrentNpc ].nTalkScript, this );
                if (oGameEnv.eState == talk_with_npc)
                    oGameEnv.eState = player_adven;
			}

			break;

		//观察别人
		case watch_npc:
			SystemHint( "要观察谁？" );
			PrintNpcs( oGameEnv.nCurrentMap );

			nInput = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].nNpcNum + 1);

			//取消
			if( nInput == pSpots[oGameEnv.nCurrentMap].nNpcNum + 1 )
				oGameEnv.eState = player_adven;

			//显示对那人的描述
			else
			{
				ShowNpcDetail(pSpots[oGameEnv.nCurrentMap].nNpcs[nInput -1]);
				SystemPause();
				oGameEnv.eState = player_adven;
			}

			break;

		//攻击别人
		case attack_npc :
			SystemHint("要攻击谁？");
			PrintNpcs( oGameEnv.nCurrentMap );

			nInput = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].nNpcNum + 1 );

			//取消
			if( nInput == pSpots[oGameEnv.nCurrentMap].nNpcNum + 1 )
				oGameEnv.eState = player_adven;

			//开战
			else
			{
				cout << "你与";
				ShowNpcName( pSpots[oGameEnv.nCurrentMap].nNpcs[nInput -1]);
				SystemWarning("斗在一处！");

				InitFight( pSpots[oGameEnv.nCurrentMap].nNpcs[nInput -1] );
			}

			break;

		//战斗过程
		case fighting:

			short nActRole ;
			short nSelectedSkill ;
			nActRole = DecideWhoAct();

			//如果是玩家行动
			if ( nActRole == 0 )
			{
				oGameEnv.eState = player_act;
			}

			//如果是npc行动
			else
			{
				nSelectedSkill = UseWhichSkill( nActRole );
				Act(nActRole, nSelectedSkill, 0 );
			}

			break;

		//玩家在战斗中的行动
		case player_act:

			SystemHint("轮到你行动了！");
            ShowNpcState(0);
			SystemOut("1.攻击	2.技能	3.物品	4.逃走	");

			nInput = GetUserInput(1, 4);

			//选择攻击
			if( nInput == 1 )
				Act( 0, -1, oGameEnv.nCurrentEnemy );

			//使用技能
			else if ( nInput == 2 )
			{
				if ( pNpcs[0].nSkillNum == 0 )
				{
					SystemWarning("你还没学会任何技能。");
				}
				else
					oGameEnv.eState = list_skills;
			}

			//使用物品
			else if (nInput == 3 )
			{
				if( pNpcs[0].nGoodsNum == 0 )
				{
					SystemWarning("你身上没有任何物品！");
				}
				else
					oGameEnv.eState = list_goods;
			}

			//逃跑
			else if ( nInput == 4 )
				EscapeOut( 0, oGameEnv.nCurrentEnemy );

			//出错
			else
				SystemWarning("没有您选的那项，请重新选择！");

			break;

		//游戏过程中调出系统菜单
		case playing_sys_menu:

			SystemHint("要做什么？");
			SystemOut( "1.存储进度\n2.读取进度\n3.返回标题\n4.继续游戏" );

			nInput = GetUserInput(1, 4);

			//如果是存储进度
			if ( nInput == 1 )
			{
				if(!SavePlayerData()){
                    SystemWarning("进度存储失败。");
                    iRunning = false;
                }
                else{
                    SystemHint("进度存储完毕。");
                    SystemPause();
                }
				oGameEnv.eState = playing_sys_menu;
			}

			//读取进度
			else if ( nInput == 2 )
			{
				if (!LoadPlayerData( false )){
                    SystemHint("进度读取失败！");
                    iRunning = false;
                }
                else{
                    SystemHint("进度读取成功！");
                    SystemPause();
                }
                oGameEnv.eState = player_adven;
			}

			//返回标题
			else if ( nInput == 3 )
				oGameEnv.eState = system_menu;

			//取消
			else
				oGameEnv.eState = player_adven;

			break;

		//查看任务
		case list_quest:

			SystemHint("要查看哪类任务？");
			SystemOut("1.未完成任务		2.已完成任务	3.返回");
			nInput = GetUserInput(1, 3);

			if( nInput == 1 )
			{
				ShowQuests( false );
			}

			else if( nInput == 2 )
			{
				ShowQuests( true );
			}

			else if (nInput == 3 )
				oGameEnv.eState = player_adven;

			break;

		//查看拥有物品
		case list_goods:

			SystemHint("要使用什么物品？");
			PrintGoodsOfNpc( 0 );

			nInput = GetUserInput(1, pNpcs[0].nGoodsNum +1 );

			//取消
			if ( nInput ==  pNpcs[0].nGoodsNum +1 )
			{
				if (oGameEnv.bFighting)
					oGameEnv.eState = player_act;
				else
					oGameEnv.eState = player_adven;
			}

			//准备使用选中的物品
			else
			{
				oGameEnv.nCurrentGoods = nInput - 1;
				oGameEnv.eState = use_goods;
			}

			break;

		//使用物品
		case use_goods:
			ShowGoodsDetail( pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);
			SystemOut("1.使用	2.丢弃	3.返回");

			nInput = GetUserInput(1, 3);

			//使用物品
			if (nInput == 1)
			{
				UseGoods( 0, oGameEnv.nCurrentGoods );

				if( oGameEnv.bFighting)
					oGameEnv.eState = fighting;
				else
					oGameEnv.eState = list_goods;
			}

			else if ( nInput == 2 )
			{
				ShowNpcName(0);
				cout<<"扔掉了";
				ShowGoodsName( pNpcs[0].MyGoods[oGameEnv.nCurrentGoods ] );
				cout << "。" << endl;
				SystemPause();

				AddGoodsToSpot(oGameEnv.nCurrentMap,
					pNpcs[0].MyGoods[oGameEnv.nCurrentGoods ]);
				RemoveGoods(0, oGameEnv.nCurrentGoods, false);
				oGameEnv.eState = list_goods;
			}

			//取消
			else
				oGameEnv.eState =  list_goods;

			break;

		//查看拥有的技能
		case list_skills:

			SystemHint("要使用什么技能？");
			for( i = 0; i< pNpcs[0].nSkillNum; ++i)
			{
				cout << i+1 << ".";
				ShowSkillName( pNpcs[0].MySkills[i].nSkillIndex );
				cout << "\t" << pNpcs[0].MySkills[i].nSkillLevel << "重" << endl;
			}
			cout << i+1 << ".取消" <<endl <<endl;

			nInput = GetUserInput(1, i+ 1 );

			//取消
			if ( nInput == i + 1 )
			{
				if (oGameEnv.bFighting)
					oGameEnv.eState = player_act;
				else
					oGameEnv.eState = player_adven;
			}

			//准备使用技能
			else
			{
				oGameEnv.nCurrentSkill = nInput - 1;
				oGameEnv.eState = use_skill;
			}

			break;

		//使用技能
		case use_skill:

			//计算内力消耗
			short nMpCost;
			nMpCost = SkillMpCost(0, oGameEnv.nCurrentSkill);

			//显示
			ShowSkillDetail( pNpcs[0].MySkills[oGameEnv.nCurrentSkill].nSkillIndex );
			cout << "当前级别："<< pNpcs[0].MySkills[oGameEnv.nCurrentSkill].nSkillLevel
				<< '\t';
			cout << "耗费内力："<< nMpCost <<'\t';
			cout << "附加伤害："<< SkillDamage(0, oGameEnv.nCurrentSkill) << endl;
			SystemOut("1.使用	2.返回");

			nInput = GetUserInput(1, 2);

			//使用技能
			if (nInput == 1)
			{
				if (oGameEnv.bFighting)
				{
					if(pNpcs[0].nMP >= nMpCost )
					{
						Act( 0 , oGameEnv.nCurrentSkill, oGameEnv.nCurrentEnemy );
					}
					else
					{
						SystemWarning("内力不足。");
						oGameEnv.eState = player_act;
					}
				}

				else
				{
					SystemWarning("只能在战斗时使用！");
					oGameEnv.eState = list_skills;
				}
			}

			//取消
			else if ( nInput ==  2 )
			{
				oGameEnv.eState =  list_skills;
			}

			break;

		//列出可以捡的东西
		case list_free_item:
			SystemHint("要捡起什么？");
			for( i= 0; i < pSpots[oGameEnv.nCurrentMap].nGoodsNum; ++ i)
			{
				cout << i+1 << ".";
				ShowGoodsName( pSpots[oGameEnv.nCurrentMap].nGoods[i]);
				cout << "\t";
			}
			cout << i+1 << ".取消" <<endl << endl;

			nInput = GetUserInput(1, i+1);

			if( nInput == i + 1 )
				oGameEnv.eState = player_adven;
			else
			{
				oGameEnv.nCurrentGoods = nInput - 1;
				oGameEnv.eState = pick_item;
			}
			break;

		case pick_item:
			ShowGoodsDetail(
				pSpots[oGameEnv.nCurrentMap ].nGoods[ oGameEnv.nCurrentGoods ] );
			SystemOut("1.捡取	2.取消");

			nInput = GetUserInput(1, 2);

			if(nInput == 1 )
			{
				PickGoods( 0, oGameEnv.nCurrentGoods);
				if( pSpots[oGameEnv.nCurrentMap ].nGoodsNum == 0)
					oGameEnv.eState = player_adven;
				else
					oGameEnv.eState = list_free_item;
			}
			else
				oGameEnv.eState = list_free_item;

			break;

		case check_equipment:
			ShowNpcName( 0 );
			cout<<"的装备情况："<<endl;

#ifdef _WIN32_
			SetConsoleTextAttribute( hConsoleHandle, ITEM_NAME_COLOR|0 );
#endif
			cout <<"1.头戴："; ShowEquipment( 0, 0); cout << endl;
			cout <<"2.身着："; ShowEquipment( 0, 1); cout << endl;
			cout <<"3.手持："; ShowEquipment( 0, 2); cout << endl;
			cout <<"4.脚踏："; ShowEquipment( 0, 3); cout << endl;
			cout <<"5.取消"; cout <<endl << endl;

			nInput = GetUserInput(1, 5);

			if( nInput == 5)
				oGameEnv.eState = player_adven;
			else if( pNpcs[0].MyEquipment[nInput-1] == -1 )
				SystemWarning("这没什么好看的。");
			else
			{
				oGameEnv.nCurrentEquipment = nInput -1;
				oGameEnv.eState = unequip_;
			}
			break;

		case unequip_:
			ShowGoodsDetail( pNpcs[0].MyEquipment[oGameEnv.nCurrentEquipment] );
			SystemOut("1.卸除	2.返回");

			nInput = GetUserInput(1, 2);
			if( nInput == 1 )
			{
				UnEquip( 0, oGameEnv.nCurrentEquipment );
				oGameEnv.eState = check_equipment;
			}
			else
				oGameEnv.eState = player_adven;

			break;

		case trading:
			ShowNpcName( oGameEnv.nCurrentNpc );
			cout <<"：欢迎光临，看看有什么需要的吧！"<< endl;
			SystemOut("1.购买	2.贩卖	3.取消");

			nInput = GetUserInput(1, 3);
			if (nInput == 1)
				oGameEnv.eState = buy_goods;
			else if( nInput == 2)
				oGameEnv.eState = sell_goods;
			else
				oGameEnv.eState = player_adven;
			break;

		case buy_goods:
			SystemHint("要买些什么？");
			PrintGoodsOfNpc( oGameEnv.nCurrentNpc );

			nInput = GetUserInput(1, pNpcs[oGameEnv.nCurrentNpc ].nGoodsNum + 1);

			if( nInput == pNpcs[oGameEnv.nCurrentNpc ].nGoodsNum + 1 )
				oGameEnv.eState = player_adven;
			else
			{
				BuyGoods( 0, pNpcs[oGameEnv.nCurrentNpc ].MyGoods[ nInput-1 ]);
				oGameEnv.eState = buy_goods;
			}
			break;

		case sell_goods:
			SystemHint("要卖什么？");
			PrintGoodsOfNpc( 0 );

			nInput = GetUserInput(1, pNpcs[0].nGoodsNum + 1);

			if( nInput == pNpcs[0].nGoodsNum + 1 )
				oGameEnv.eState = player_adven;
			else
			{
				SellGoods( 0, nInput -1 );
				oGameEnv.eState = sell_goods;
			}
			break;


		default:
			oGameEnv.eState = player_adven ;
		}
	}
}


/* 读入场景数据 */
bool CGame::LoadSpotsData(const char * pstFileName, const char cHeadFlag )
{
	short i;

	if( pSpots )
	{
		delete [] pSpots;
		pSpots = NULL;
	}

	CTextParser SpotsFile( pstFileName, cHeadFlag);	//open spots file
	nSpotsNum = SpotsFile.GetRecordNum();			//get spots number
	pSpots = new CSpot[ nSpotsNum ]; //malloc memory space to hold spots data

	//read data one by one
	for ( short nSpotsIndex = 0; nSpotsIndex < nSpotsNum; ++ nSpotsIndex )
	{
		SpotsFile.FindRecordHead();
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString( pSpots[ nSpotsIndex ].stName  );
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString( pSpots[ nSpotsIndex ].stScene );

		//exits data
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger( & pSpots[ nSpotsIndex ].nExitNum);
		SpotsFile.GotoNextLine();
		for ( i = 0; i < pSpots[nSpotsIndex].nExitNum; ++i )
			SpotsFile.ReadInteger( & pSpots[nSpotsIndex].nExits[i] );

		//goods data
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger( & pSpots[nSpotsIndex].nGoodsNum );
		SpotsFile.GotoNextLine();
		for( i = 0; i < pSpots[ nSpotsIndex ].nGoodsNum; ++ i)
			SpotsFile.ReadInteger( & pSpots[nSpotsIndex].nGoods[i] );

		//npcs data
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger( & pSpots[nSpotsIndex].nNpcNum );
		SpotsFile.GotoNextLine();
		for( i= 0; i < pSpots[ nSpotsIndex ].nNpcNum; ++i )
			SpotsFile.ReadInteger( & pSpots[nSpotsIndex].nNpcs[i] );

		//enter script and leave script
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger( &pSpots[nSpotsIndex].nEnterScript );
		SpotsFile.ReadInteger( &pSpots[nSpotsIndex].nLeaveScript );
	}
    return true;
}

//显示场景名称
void CGame::ShowSpotName(short nSpotIndex )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, SPOT_NAME_COLOR|0 );
	cout << pSpots[nSpotIndex].stName;
#else
    printf(SPOT_NAME_STRING, pSpots[nSpotIndex].stName);
#endif
}

/* 关于场景的详细信息 */
void CGame::ShowSpotDetail( short nSpotIndex  )
{
	short i;

	//show specified spot description
	ShowSpotName( nSpotIndex );
	cout <<endl;
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, DESCRIPTION_COLOR|0 );
	cout << pSpots[ nSpotIndex].stScene << endl;
#else
    printf(DESCRIPTION_STRING, pSpots[ nSpotIndex].stScene);
    cout << endl;
#endif

	//show those npcs in the spot
	if ( pSpots[nSpotIndex].nNpcNum )
	{
		cout << "这里的人物有：";
		for (  i = 0; i < pSpots[ nSpotIndex ].nNpcNum  ; ++ i)
		{
			ShowNpcName( pSpots[nSpotIndex].nNpcs[i]  );
			cout << "\t";
		}
		cout << endl;
	}

	// show the goods of the spot
	if ( pSpots[nSpotIndex].nGoodsNum )
	{
		cout << "这里有些可以拣取的东西：";
		for ( i =0; i < pSpots[nSpotIndex].nGoodsNum; ++ i)
		{
			ShowGoodsName( pSpots[nSpotIndex].nGoods[i] );
			cout << "\t";
		}
		cout << endl;
	}

	//show the exits of the spot
	if( pSpots[nSpotIndex].nExitNum )
	{
		cout << "这里可以通往：";
		for ( i = 0; i < pSpots[ nSpotIndex ].nExitNum ; ++ i)
		{
			ShowSpotName( pSpots[nSpotIndex].nExits[i] );
			cout << " ";
		}
		cout << endl;
	}

	//RunScripts( pSpots[nSpotIndex].nEnterScript );
}

/* show skill name */
void CGame::ShowSkillName(short nSkillIndex)
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, SKILL_NAME_COLOR|0 );
	cout << pSkills[nSkillIndex].stName;
#else
    printf(SKILL_NAME_STRING, pSkills[nSkillIndex].stName);
#endif
}

/* 显示技能详细信息 */
void CGame::ShowSkillDetail(short nSkillIndex )
{
	ShowSkillName( nSkillIndex );
	cout << endl;
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, DESCRIPTION_COLOR|0);
	cout << pSkills[nSkillIndex].stDesc << endl;
#else
    printf(DESCRIPTION_STRING, pSkills[nSkillIndex].stDesc);
    cout << endl;
#endif
	cout << "每升一级增加威力" << pSkills[nSkillIndex].nPower <<"。"<<endl;
}

/* 显示装备名称 */
void CGame::ShowEquipment(short nRoleIndex, short nLocation )
{
	if( pNpcs[nRoleIndex].MyEquipment[nLocation ] != -1 )
		ShowGoodsName( pNpcs[nRoleIndex].MyEquipment[nLocation] );
	else
		cout << "无";
}

/* 显示物品名称 */
void CGame::ShowGoodsName(short nGoodsIndex )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, ITEM_NAME_COLOR|0 );
	cout << pGoods[nGoodsIndex].stName;
#else
    printf(ITEM_NAME_STRING, pGoods[nGoodsIndex].stName);
#endif
}

/* 显示物品的详细信息 */
void CGame::ShowGoodsDetail( short nGoodsIndex)
{
	if( nGoodsIndex >= nGoodsNum )
	{
		SystemWarning("物品索引超出范围，游戏将退出！");
		iRunning =  false;
        return;
	}

	//物品描述
	ShowGoodsName( nGoodsIndex );
	cout << endl;
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, DESCRIPTION_COLOR|0 );
	cout << pGoods[ nGoodsIndex ].stDesc << endl;
#else
    printf(DESCRIPTION_STRING, pGoods[nGoodsIndex].stDesc);
    cout << endl;
#endif

	//物品的效果
	switch( pGoods[nGoodsIndex].nProperty ) {
	case 0:
		cout << "可以服用！";
		break;
	case 1:
		cout << "可以装备。";
		break;
	case 2:
		cout << "可以修习！";
        break;
    default:
        cout << "任务物品!";
	}

	if ( pGoods[nGoodsIndex].nAddHP )
		cout << "生命 + " << pGoods[nGoodsIndex].nAddHP << "。\t";
	if ( pGoods[nGoodsIndex].nAddMaxHP)
		cout <<"生命上限 + " << pGoods[nGoodsIndex].nAddMaxHP << "。\t";
	if ( pGoods[nGoodsIndex].nAddMP )
		cout <<"内力 + " << pGoods[nGoodsIndex].nAddMP <<"。\t";
	if ( pGoods[nGoodsIndex].nAddMaxMP)
		cout <<"内力上限 + " << pGoods[nGoodsIndex].nAddMaxMP <<"。\t";
	if ( pGoods[nGoodsIndex].nAddAttack)
		cout <<"攻击 + "<< pGoods[nGoodsIndex].nAddAttack <<"。\t";
	if ( pGoods[nGoodsIndex].nAddDefence)
		cout <<"护体 + " << pGoods[nGoodsIndex].nAddDefence <<"。\t";
	if ( pGoods[nGoodsIndex].nAddSpeed )
		cout <<"速度 + " << pGoods[nGoodsIndex].nAddSpeed <<"。\t";
	if ( pGoods[nGoodsIndex].nSkillIndex != -1 )
	{
		cout << "记载有武功：";
		ShowSkillName( pGoods[nGoodsIndex].nSkillIndex );
		cout <<"。";
	}

	cout << endl;
}

//显示任务
void CGame::ShowQuests(bool bCompleted )
{
	short i, n = 0;

	if ( bCompleted )
	{
		cout << "已完成的任务："<<endl;

		for( i = 0; i < nQuestsNum; ++i )
		{
			if ( pQuests[i].nFlagValue == 2 )
			{
				n++;
#ifdef _WIN32_
                SetConsoleTextAttribute( hConsoleHandle, QUEST_COLOR|0 );
				cout <<"[" << n << "] ";
                cout << pQuests[i].stQuestDesc << endl;
#else
                cout <<"[" << n << "]";
                printf(QUEST_STRING, pQuests[i].stQuestDesc);
                cout << endl;
#endif
			}
		}
	}

	else
	{
		cout << "未完成的任务："<<endl;

		for( i = 0; i < nQuestsNum; ++i )
		{
			if ( pQuests[i].nFlagValue == 1 )
			{
				n++;
#ifdef _WIN32_
                SetConsoleTextAttribute( hConsoleHandle, QUEST_COLOR|0 );
				cout <<"[" << n << "] ";
                cout << pQuests[i].stQuestDesc << endl;
#else
				cout <<"[" << n << "] ";
                printf(QUEST_STRING, pQuests[i].stQuestDesc);
                cout << endl;
#endif
			}
		}
	}

	if( !n )
		cout << "无。"<< endl;

	SystemPause();
}

/* load Npcs data */
bool CGame::LoadNpcsData( char * pstFileName, char cHeadFlag )
{
	short i;

	if( pNpcs )
	{
		delete [] pNpcs;
		pNpcs = NULL;
	}

	CTextParser NpcsFile( pstFileName, cHeadFlag);	//open npcs file
	nNpcsNum = NpcsFile.GetRecordNum();				//get npcs number
	pNpcs = new CRole[ nNpcsNum ];	//malloc memory space to hold npcs data

	//read data one by one
	for ( short nNpcsIndex = 0; nNpcsIndex < nNpcsNum; ++ nNpcsIndex )
	{
		NpcsFile.FindRecordHead();

		//role description
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString( pNpcs[ nNpcsIndex ].stName  );
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString( pNpcs[ nNpcsIndex ].stDescription  );

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( &( pNpcs[nNpcsIndex].nExps[0]) );	//Exp for sword
		NpcsFile.ReadInteger( &( pNpcs[nNpcsIndex].nExps[1]) );	//Exp for blade
		NpcsFile.ReadInteger( &( pNpcs[nNpcsIndex].nExps[2]) );	//Exp for long
		NpcsFile.ReadInteger( &( pNpcs[nNpcsIndex].nExps[3]) );	//Exp for hand
        UpdateNick(nNpcsIndex); //create nick

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex ].nHP )  );	//hp
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex].nMaxHP ));	//maxHP
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex].nMP ) );	//mp
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex].nMaxMP));	//maxMp

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex ].nAttack));	//attack
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex ].nDefend));	//defend
		NpcsFile.ReadInteger( &( pNpcs[ nNpcsIndex ].nSpeed ));	//speed

		//read skill data
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( & (pNpcs[ nNpcsIndex ].nSkillNum ));	//skill number
		NpcsFile.GotoNextLine();
		for ( i = 0; i < pNpcs[ nNpcsIndex].nSkillNum ; i++ )
		{
			NpcsFile.ReadInteger( &(pNpcs[nNpcsIndex].MySkills[i].nSkillIndex) );
			NpcsFile.ReadInteger( &(pNpcs[nNpcsIndex].MySkills[i].nSkillLevel) );
			NpcsFile.ReadInteger( &(pNpcs[nNpcsIndex].MySkills[i].nTimes) );
		}

		//read goods data
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( & (pNpcs[ nNpcsIndex ].nGoodsNum) );	//goods number
		NpcsFile.GotoNextLine();
		for ( i = 0; i < pNpcs[ nNpcsIndex ].nGoodsNum; i++ )
		{
			NpcsFile.ReadInteger( & (pNpcs[nNpcsIndex].MyGoods[i]));
		}

		//equipment
		NpcsFile.GotoNextLine();
		for( i = 0; i < 4; ++ i)
			NpcsFile.ReadInteger( &( pNpcs[nNpcsIndex].MyEquipment[i]) );

		//money
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( & ( pNpcs[nNpcsIndex].nMoney) );

		// related script data
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger( & (pNpcs[nNpcsIndex].nTalkScript) ); 	//talk script index
		NpcsFile.ReadInteger( & (pNpcs[nNpcsIndex].nFailScript) );	//fail script index
	}
    return true;
}

/* 暂停游戏，等用户输入 */
void CGame::SystemPause()
{
	getch();
    cout << endl;
}

/* 系统信息输出 */
void CGame::SystemOut(const char * stOutString )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, SYSTEM_COLOR|0);
	cout << stOutString << endl << endl;
#else
    printf(SYSTEM_STRING, stOutString);
    cout <<endl << endl;
#endif
}

/* 提示信息输出 */
void CGame::SystemHint(const char * stHintString )
{
    cout << endl;
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, SYSTEM_HINT_COLOR|0);
	cout << stHintString << endl << endl;
#else
    printf(SYSTEM_HINT_STRING, stHintString);
    cout << endl << endl;
#endif
}

/* 警告信息输出，并暂停 */
void CGame::SystemWarning(const char * stWarningString )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, WARNING_COLOR|0);
	cout << stWarningString << endl;
#else
    printf(WARNING_STRING, stWarningString);
    cout << endl;
#endif
	SystemPause();
}

//显示角色姓名
void CGame::ShowNpcName( short nNpcIndex )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, NPC_NAME_COLOR|0);
	cout << pNpcs[nNpcIndex].stName;
#else
    printf(NPC_NAME_STRING, pNpcs[nNpcIndex].stName);
#endif
}

//显示角色绰号
void CGame::ShowNickName( short nNpcIndex )
{
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, NPC_NAME_COLOR|0);
	cout << pNpcs[nNpcIndex].stNick;
#else
    printf(NICK_NAME_STRING, pNpcs[nNpcIndex].stNick);
#endif
}

void CGame::UpdateNick(short nRoleIdx){
    short nMaxIdx = -1;
    short nMaxExp = -1;
    for (short i = 0; i < SKILL_KIND; ++i){
        if (pNpcs[nRoleIdx].nExps[i] > nMaxExp){
            nMaxExp = pNpcs[nRoleIdx].nExps[i];
            nMaxIdx = i;
        }
    }
    strcpy(pNpcs[nRoleIdx].stNick, stSkillNicks[nMaxIdx]);
    for (short i = SKILL_LVL -1; i >=0; --i){
        if (nMaxExp >= nNickExp[i]){
            strcat(pNpcs[nRoleIdx].stNick, stLvlNicks[i]);
            break;
        }
    }
}

void CGame::ShowNpcState(short nNpcIndex){
	ShowNpcName( nNpcIndex );
    cout << " [";
    ShowNickName( nNpcIndex );
    cout << "] " << endl;
    cout << "( 生命：" << pNpcs[nNpcIndex].nHP << "/" << pNpcs[nNpcIndex].nMaxHP
         << "； 内力：" << pNpcs[nNpcIndex].nMP << "/" << pNpcs[nNpcIndex].nMaxMP
         << "； " << pNpcs[nNpcIndex].nMoney << "文 )" << endl;
}

//显示角色描述
void CGame::ShowNpcDetail(short nNpcIndex)
{
	ShowNpcName( nNpcIndex );
    cout << " [";
    ShowNickName( nNpcIndex );
    cout << "] " << endl;

#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pNpcs[nNpcIndex].stDescription << endl;
#else
    printf(DESCRIPTION_STRING, pNpcs[nNpcIndex].stDescription);
    cout << endl;
#endif
	cout << "头戴: ";
	ShowEquipment( nNpcIndex, 0 );
	cout << endl;
	cout << "身着: ";
	ShowEquipment( nNpcIndex, 1);
	cout << endl;
	cout << "手持: ";
	ShowEquipment( nNpcIndex, 2 );
	cout << endl;
	cout << "脚踏: ";
	ShowEquipment( nNpcIndex, 3 );
	cout << endl ;

}

/* 显示角色的详细信息 */
void CGame::ShowNpcDetailX( short nNpcIndex )
{
	ShowNpcDetail( nNpcIndex );

#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, ITEM_NAME_COLOR| 0);
#endif

	cout << "银两: " << pNpcs[nNpcIndex].nMoney << endl;
	cout << endl;

#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, DESCRIPTION_COLOR|0);
#endif

    for (short i = 0; i < SKILL_KIND; ++i){
        cout << stSkillNicks[i] << "修为  ："
             << pNpcs[nNpcIndex].nExps[i] << endl;
    }

	cout << "生命    ："<< pNpcs[nNpcIndex].nHP
		<< "\t/ " << pNpcs[nNpcIndex].nMaxHP << endl;
	cout << "内力    ："<< pNpcs[nNpcIndex].nMP
		<< "\t/ "<< pNpcs[nNpcIndex].nMaxMP << endl;

	cout << "攻击    ："<< pNpcs[nNpcIndex].nAttack << "\t +"
		<< GetAddedAttack( nNpcIndex ) << endl;
	cout << "防御    ："<< pNpcs[nNpcIndex].nDefend << "\t +"
		<< GetAddedDefend( nNpcIndex ) << endl;
	cout << "身法    ："<< pNpcs[nNpcIndex].nSpeed << "\t +"
         << GetAddedSpeed( nNpcIndex ) << endl << endl;
}

//读取资源文件名
bool CGame::LoadResFileName(const char* pstFileName, const char cHeadFlag)
{
	CTextParser ResNameFile( pstFileName, cHeadFlag );
	ResNameFile.FindRecordHead();

	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stSkillFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stGoodsFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stRoleFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stSpotFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stQuestFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.stScriptFile);
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stMapFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stLogoFile );
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString( oResFiles.stNickFile );

    return true;
}

void CGame::LoadNickData(const char * pstFileName, const char cHeadFlag){
    CTextParser NickFile( pstFileName, cHeadFlag);
    NickFile.FindRecordHead();
    NickFile.GotoNextLine();
    short i;
    for (i = 0; i < SKILL_KIND; ++i)
        NickFile.ReadString( stSkillNicks[i]);

    NickFile.GotoNextLine();
    for (i = 0; i < SKILL_LVL; ++i)
        NickFile.ReadString( stLvlNicks[i]);

    NickFile.GotoNextLine();
    for (i = 0; i < SKILL_LVL; ++i)
        NickFile.ReadInteger( &nNickExp[i]);
}

/* load skills data */
bool CGame::LoadSkillsData(const char * pstFileName,  const char cHeadFlag)
{
	if( pSkills )
	{
		delete [] pSkills;
		pSkills = NULL;
	}

	CTextParser SkillsFile( pstFileName, cHeadFlag );
	nSkillsNum = SkillsFile.GetRecordNum();
	pSkills = new CSkill[nSkillsNum ];
    if (!pSkills){
        SystemWarning("为技能分配内存失败，游戏将退出！");
        return false;
    }

	for (short nSkillIndex = 0; nSkillIndex < nSkillsNum; ++ nSkillIndex )
	{
		SkillsFile.FindRecordHead();
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString( pSkills[nSkillIndex].stName );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString( pSkills[nSkillIndex].stDesc );

		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nPower ));
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nLvlUp) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nMpCost));
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nSklCls) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nPreExp) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddExp) );

		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddHp) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddMp) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddAttack) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddDef) );
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger( &(pSkills[nSkillIndex].nAddSpeed) );

	}

    return true;
}

/* load goods data */
bool CGame::LoadGoodsData(char * pstFileName, char cHeadFlag )
{
	if(pGoods)
	{
		delete [] pGoods;
		pGoods = NULL;
	}

	CTextParser GoodsFile( pstFileName, cHeadFlag);
	nGoodsNum = GoodsFile.GetRecordNum();
	pGoods = new CGoods[nGoodsNum];
	assert(pGoods);

	for( short nGoodsIndex = 0; nGoodsIndex < nGoodsNum; nGoodsIndex++)
	{
		GoodsFile.FindRecordHead();
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString( pGoods[nGoodsIndex].stName );			//name
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString( pGoods[nGoodsIndex].stDesc );			//description

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nProperty);	//eat,equip or learn?

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddAttack);	//add attack
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddDefence);	//add defence
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddSpeed);	//add speed

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddHP);		//add hp
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddMaxHP);	//add max hp
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddMP);		//add mp
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nAddMaxMP);	//add max mp

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nSkillIndex);	//which skill to learn

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nLocation );	//location:head,foot...

		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger( & pGoods[nGoodsIndex].nCost );		//value
	}
    return true;
}

/* 装载任务数据 */
bool CGame::LoadQuestData(char * pstFileName, char cHeadFlag )
{
	if ( pQuests )
	{
		delete [] pQuests;
		pQuests = NULL;
	}

	CTextParser QuestsFile( pstFileName, cHeadFlag);
	nQuestsNum = QuestsFile.GetRecordNum();
	pQuests = new CQuest[nQuestsNum];
	if (!pQuests){
        return false;
    }

	for ( short nQuestIndex = 0; nQuestIndex < nQuestsNum; ++ nQuestIndex)
	{
		QuestsFile.FindRecordHead();
		QuestsFile.GotoNextLine();
		QuestsFile.ReadString( pQuests[nQuestIndex].stQuestDesc );
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger( &pQuests[nQuestIndex].nRoleIndex );
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger( &pQuests[nQuestIndex].nMapIndex );
        pQuests[nQuestIndex].nFlagValue = 0;
	}
    return true;
}



void CGame::RandomDrop(short nRoleIdx, short nGoodsIdx, short nProb){
    short nRan = rand()%100;
    if (nRan <= nProb){
        AddGoodsToSpot(oGameEnv.nCurrentMap, nGoodsIdx);
        ShowNpcName(nRoleIdx);
        cout << "掉落了";
        ShowGoodsName(nGoodsIdx);
        cout << endl;
        SystemPause();
    }
}


//初始化战斗
void CGame::InitFight(short nNpcIndex )
{
	oGameEnv.nCurrentEnemy = nNpcIndex;
    oGameEnv.nCurrentNpc = nNpcIndex;

	oActSlot[0].nRoleID = 0;
	oActSlot[0].nVirtualSpeed = pNpcs[0].nSpeed + GetAddedSpeed(0);
	oActSlot[1].nRoleID = oGameEnv.nCurrentEnemy;
	oActSlot[1].nVirtualSpeed = pNpcs[ oGameEnv.nCurrentEnemy].nSpeed
		+ GetAddedSpeed( oGameEnv.nCurrentEnemy );

	oGameEnv.bFighting = true;
	oGameEnv.eState = fighting;
}

//战斗结束
void CGame::EndFight(short nNpcIndex )
{
	//显示消息
	if( nNpcIndex == 0 )
		cout <<"你战胜了";
	else
		cout <<"你输给了";
	ShowNpcName(oGameEnv.nCurrentEnemy);
	cout << "！" << endl;
	SystemPause();

	//善后
	oGameEnv.bFighting = false;
	if( nNpcIndex == 0 )
	{
        /*
		short exp = Experience(nNpcIndex, oGameEnv.nCurrentEnemy );
		ShowNpcName(nNpcIndex);
		cout <<"得到" << exp << "点经验。" <<endl;
		pNpcs[nNpcIndex].nExp += exp;
		UpdateLevel( nNpcIndex );
		SystemPause();
		*/
		pScriptEngine->RunScripts( pNpcs[oGameEnv.nCurrentEnemy].nFailScript, this );	//运行敌人失败的脚本
		Refresh( oGameEnv.nCurrentEnemy );

		if( oGameEnv.bFighting )
			oGameEnv.eState = fighting;
		else
			oGameEnv.eState = player_adven;
	}
	else
	{
		pScriptEngine->RunScripts( pNpcs[0].nFailScript, this );
		oGameEnv.eState = system_menu;
	}
}

short CGame::DecideWhoAct()
{
	if ( oActSlot[0].nVirtualSpeed >= oActSlot[1].nVirtualSpeed)
	{
		oActSlot[1].nVirtualSpeed += ( pNpcs[oGameEnv.nCurrentEnemy].nSpeed
			+ GetAddedSpeed( oGameEnv.nCurrentEnemy ) );
		return oActSlot[0].nRoleID;
	}
	else
	{
		oActSlot[0].nVirtualSpeed += pNpcs[0].nSpeed + GetAddedSpeed( 0 );
		return oActSlot[1].nRoleID;
	}
}

//选择用什么技能
short CGame::UseWhichSkill(short nRoleIndex )
{
	short nSelectedSkill = -1;
	short nSkillPower = 0;
	short i;

	for( i = 0; i < pNpcs[nRoleIndex].nSkillNum;  ++i )
	{
		short nPower = SkillDamage( nRoleIndex, i );
		if (nSkillPower < nPower &&
			pNpcs[nRoleIndex].nMP >= SkillMpCost(nRoleIndex, i) )
		{
			nSkillPower = nPower;
			nSelectedSkill = i ;
		}
	}

	return nSelectedSkill;
}

//逃跑
void CGame::EscapeOut(short nRoleIndex, short nEnemyIndex)
{
	short r = rand()%100;
	if( pNpcs[nRoleIndex].nSpeed + GetAddedSpeed( nRoleIndex ) <
		pNpcs[nEnemyIndex].nSpeed + GetAddedSpeed( nEnemyIndex) )
	{
		if( r < 50 )
		{
			ShowNpcName( nRoleIndex );
			cout << "逃离了战斗！" << endl;
			oGameEnv.eState = player_adven;
			oGameEnv.bFighting = false;
		}
		else
		{
			ShowNpcName( nRoleIndex);
			cout << "逃跑失败！" << endl;
			oGameEnv.eState = fighting;
		}
	}
	else
	{
		if( r < 90 )
		{
			ShowNpcName( nRoleIndex );
			cout << "逃离了战斗！" << endl;
			oGameEnv.eState = player_adven;
			oGameEnv.bFighting = false;
		}
		else
		{
			ShowNpcName( nRoleIndex);
			cout << "逃跑失败！" << endl;
			oGameEnv.eState = fighting;
		}
	}
	SystemPause();
}

//指定的角色施展指定的技能
void CGame::Act(short nRoleIndex, short nSkillIndex , short nEnemyIndex )
{
	short nDamage;
#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, SYSTEM_COLOR|0 );
#endif

	//普通攻击
	if( nSkillIndex == -1 )
	{
		nDamage = Damage( nRoleIndex, nEnemyIndex );
        ShowNpcName(nRoleIndex);
        cout << "攻击";
        ShowNpcName(nEnemyIndex);
        cout<< "。 "<<endl;
	}

	//使用技能攻击
	else
	{
		pNpcs[nRoleIndex].nMP -= SkillMpCost( nRoleIndex, nSkillIndex );
		nDamage = Damage(nRoleIndex, nEnemyIndex) + SkillDamage( nRoleIndex, nSkillIndex);
		ShowNpcName(nRoleIndex);
		cout << "使用";
        ShowSkillName(pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex);
        cout<< "攻击";
        ShowNpcName(nEnemyIndex);
        cout<< "。 "<<endl;
        if (nRoleIndex == 0)
            UpdateSkillLevel(nRoleIndex, nSkillIndex);
	}

    ShowNpcName(nEnemyIndex);
	cout << "受到" <<nDamage << "的伤害。";

	//攻击之后
	pNpcs[nEnemyIndex].nHP -= nDamage;
	pNpcs[nEnemyIndex].nHP =
		pNpcs[nEnemyIndex].nHP > 0 ? pNpcs[nEnemyIndex].nHP: 0;

    SystemPause();
	if (pNpcs[ nEnemyIndex ].nHP == 0)
		EndFight( nRoleIndex );
	else
	{
		oGameEnv.eState = fighting;
	}
}

//向指定的地图添加npc
bool CGame::AddNpc(short nSpotIndex, short nNpcIndex )
{
	assert(nSpotIndex >=0 && nSpotIndex < nSpotsNum &&
			nNpcIndex >= 0 && nNpcIndex < nNpcsNum );

	if( pSpots[nSpotIndex].nNpcNum == 9 )
	{
		cout << "该地图中npc已满，添加npc错误！"<<endl;
		return false;
	}

	pSpots[nSpotIndex].nNpcs[ pSpots[nSpotIndex].nNpcNum ] = nNpcIndex;
	pSpots[nSpotIndex].nNpcNum += 1;
	return true;
}

//从指定的地图移除npc
bool CGame::RemoveNpc(short nSpotIndex, short nNpcIndex )
{
	assert(nSpotIndex >=0 && nSpotIndex < nSpotsNum &&
		nNpcIndex >= 0 && nNpcIndex < nNpcsNum );

	if ( pSpots[nSpotIndex].nNpcNum == 0 )	return false;

	//找到这个npc是该地图中的第几个
	short nNpcLocation;
	for( nNpcLocation= 0; nNpcLocation < pSpots[nSpotIndex].nNpcNum; ++ nNpcLocation)
	{
		if ( pSpots[nSpotIndex].nNpcs[nNpcLocation] == nNpcIndex )
			break;
	}
	if( nNpcLocation == pSpots[nSpotIndex].nNpcNum )	return false;

	//如果这个npc不是最后一个，把这个npc之后的npc前移
	if (nNpcLocation < pSpots[nSpotIndex].nNpcNum - 1 )
	{
		short i;
		for ( i = nNpcLocation; i < pSpots[nSpotIndex].nNpcNum - 1; ++i )
		{
			pSpots[nSpotIndex].nNpcs[i] = pSpots[nSpotIndex].nNpcs[i+1];
		}
	}
	pSpots[nSpotIndex].nNpcNum --;

	return true;
}

void CGame::AddMoney(short nRoleIdx, short nMoney){
    pNpcs[nRoleIdx].nMoney += nMoney;
    ShowNpcName(nRoleIdx);
    cout << "得到金钱" << nMoney << "文。" << endl;
    SystemPause();
}

bool CGame::DelMoney(short nRoleIdx, short nMoney){
    if (pNpcs[nRoleIdx].nMoney >= nMoney){
        pNpcs[nRoleIdx].nMoney -= nMoney;
        ShowNpcName(nRoleIdx);
        cout << "失去金钱" << nMoney << "文。" << endl;
        SystemPause();
        return true;
    }
    else {
        return false;
    }
}

//增加物品, visible表示是否显示信息
bool CGame::AddGoods(short nRoleIndex, short nGoodsIndex, bool bVisible )
{
	assert( nGoodsIndex >=0 && nGoodsIndex <= nGoodsNum );

	if ( bVisible )
	{
		ShowNpcName( nRoleIndex );
		cout << "得到";
		ShowGoodsName( nGoodsIndex );
		cout << "。"<<endl;
		SystemPause();
	}

	if ( pNpcs[nRoleIndex].nGoodsNum == MAX_GOODS_NUM )
	{
		ShowNpcName( nRoleIndex );
		cout << "的行囊已满，";
		ShowGoodsName( nGoodsIndex );
		cout << "掉在了地上。"<< endl;
		SystemPause();

		AddGoodsToSpot( oGameEnv.nCurrentMap , nGoodsIndex );
		return false;
	}
	else
	{
		pNpcs[nRoleIndex].nGoodsNum ++ ;
		pNpcs[nRoleIndex].MyGoods[ pNpcs[nRoleIndex].nGoodsNum -1 ] = nGoodsIndex;
		return true;
	}
}

//从角色身上移除物品, nGoodsLocation是指该物品在玩家行囊中的位置
bool CGame::RemoveGoods(short nRoleIndex, short nGoodsLocation, bool bVisible )
{
	if( nGoodsLocation < 0 || nGoodsLocation >= pNpcs[nRoleIndex].nGoodsNum )
		return false;

	if( bVisible )
	{
		ShowNpcName(nRoleIndex);
		cout << "失去";
		ShowGoodsName( pNpcs[nRoleIndex].MyGoods[nGoodsLocation] );
		cout << "。" << endl;
		SystemPause();
	}

    short i;
	for ( i = nGoodsLocation; i < pNpcs[nRoleIndex].nGoodsNum - 1; ++ i)
		pNpcs[nRoleIndex].MyGoods[i] = pNpcs[nRoleIndex].MyGoods[ i+1 ];
	pNpcs[nRoleIndex].nGoodsNum --;

	return true;
}

//检查某种物品在玩家身上的位置，如果没有返回-1。
short CGame::HaveGoods(short nRoleIndex, short nGoodsIndex )
{
	short nLoc;
	for( nLoc = 0; nLoc < pNpcs[nRoleIndex].nGoodsNum; ++ nLoc)
	{
		if ( pNpcs[nRoleIndex].MyGoods[nLoc] == nGoodsIndex )
			break;
	}

	if ( nLoc == pNpcs[nRoleIndex].nGoodsNum )
		return -1;
	else
		return nLoc;
}

short CGame::NumOfGoods(short nRoleIdx, short nGoodsIdx){
    short nNumber = 0;
    for (short i = 0; i < pNpcs[nRoleIdx].nGoodsNum; ++ i){
        if (pNpcs[nRoleIdx].MyGoods[i] == nGoodsIdx)
            nNumber ++;
    }
    return nNumber;
}

//检查玩家是否有某种技能
bool CGame::HaveSkill(short nRoleIndex, short nSkillIndex )
{
	short nLoc;
	for( nLoc = 0; nLoc < pNpcs[ nRoleIndex ].nSkillNum; ++nLoc )
	{
		if( pNpcs[nRoleIndex].MySkills[nLoc].nSkillIndex == nSkillIndex )
			return true;
	}

	return false;
}

//增加技能
bool CGame::AddSkill(short nRoleIndex, short nSkillIndex )
{
	if ( nSkillIndex < 0 || nSkillIndex >= nSkillsNum )
		return false;

    if( pNpcs[nRoleIndex].nSkillNum == MAX_SKILL_NUM ){
        ShowNpcName(nRoleIndex);
        SystemWarning( "技能数达到上限，不能学习新的技能！");
        cout << endl;
        return false;
    }
	if ( HaveSkill( nRoleIndex, nSkillIndex) )
	{
		ShowNpcName( nRoleIndex );
		SystemWarning ( "已有这种技能！" );
        cout << endl;
        return false;
	}

    short nSkillClass = pSkills[nSkillIndex].nSklCls;

    if(pNpcs[nRoleIndex].nExps[nSkillClass] < pSkills[nSkillIndex].nPreExp){
        ShowNpcName( nRoleIndex );
        SystemWarning ( "的修为还不能领悟这种武功！");
        cout << endl;
        return false;
    }

	pNpcs[ nRoleIndex ].MySkills[ pNpcs[nRoleIndex].nSkillNum ].nSkillIndex
        = nSkillIndex;
	pNpcs[ nRoleIndex ].MySkills[ pNpcs[nRoleIndex].nSkillNum ].nSkillLevel = 1;
	pNpcs[ nRoleIndex ].MySkills[ pNpcs[nRoleIndex].nSkillNum ].nTimes = 0;
	pNpcs[nRoleIndex].nSkillNum ++;
	ShowNpcName( nRoleIndex );
	cout << "习得";
	ShowSkillName( nSkillIndex );
	cout <<"！" << endl;
	SystemPause();
	return true;
}

//从物品中学习技能，不用检查物品是不是可以修习，这会在usegoods函数中检查。
void CGame::LearnSkill(short nRoleIndex, short nGoodsIndex)
{
    AddSkill( nRoleIndex, pGoods[nGoodsIndex].nSkillIndex );
}

void CGame::SellGoods(short nRoleIndex, short nGoodsLocation)
{
	ShowNpcName( nRoleIndex );
	cout<<"卖掉了";
	ShowGoodsName( pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
	cout <<"。"<<endl;
	SystemPause();

	pNpcs[nRoleIndex].nMoney +=
		pGoods[pNpcs[nRoleIndex].MyGoods[nGoodsLocation] ].nCost /2;
	RemoveGoods( nRoleIndex, nGoodsLocation, false );

}

void CGame::BuyGoods(short nRoleIndex, short nGoodsIndex )
{
	if( pNpcs[nRoleIndex].nMoney > pGoods[nGoodsIndex].nCost )
	{
		ShowNpcName( nRoleIndex );
		cout << "购买了";
		ShowGoodsName( nGoodsIndex );
		cout << "。"<<endl;

		AddGoods( nRoleIndex, nGoodsIndex, false );
		pNpcs[nRoleIndex].nMoney -= pGoods[nGoodsIndex].nCost;
	}
	else
	{
		ShowNpcName( oGameEnv.nCurrentNpc );
		cout <<": 你的钱不够。" << endl;
	}

	SystemPause();
}

//服用物品。不用检查物品是否可以服用，这在use goods函数中检查
void CGame::Eat(short nRoleIndex, short nGoodsIndex )
{
	ShowNpcName( nRoleIndex );
	cout << "服用了";
	ShowGoodsName( nGoodsIndex );
	cout <<"。" << endl;
	SystemPause();

	if( pGoods[nGoodsIndex].nAddMaxHP )
	{
		cout << "生命上限：" << pNpcs[nRoleIndex].nMaxHP << "\t---->\t";
		pNpcs[nRoleIndex].nMaxHP += pGoods[nGoodsIndex].nAddMaxHP;
		cout << pNpcs[nRoleIndex].nMaxHP << endl;
	}

	if( pGoods[nGoodsIndex].nAddHP )
	{
		cout << "当前生命：" << pNpcs[nRoleIndex].nHP << "\t---->\t";
		pNpcs[nRoleIndex].nHP += pGoods[nGoodsIndex].nAddHP;
		if (pNpcs[nRoleIndex].nHP > pNpcs[nRoleIndex].nMaxHP )
			pNpcs[nRoleIndex].nHP = pNpcs[nRoleIndex].nMaxHP;
		cout << pNpcs[nRoleIndex].nHP << endl;
	}

	if( pGoods[nGoodsIndex].nAddMaxMP )
	{
		cout << "内力上限：" << pNpcs[nRoleIndex].nMaxMP << "\t---->\t";
		pNpcs[nRoleIndex].nMaxMP += pGoods[nGoodsIndex].nAddMaxMP;
		cout << pNpcs[nRoleIndex].nMaxMP << endl;
	}

	if( pGoods[nGoodsIndex].nAddMP )
	{
		cout << "当前内力：" << pNpcs[nRoleIndex].nMP << "\t---->\t";
		pNpcs[nRoleIndex].nMP += pGoods[nGoodsIndex].nAddMP;
		if (pNpcs[nRoleIndex].nMP > pNpcs[nRoleIndex].nMaxMP )
			pNpcs[nRoleIndex].nMP = pNpcs[nRoleIndex].nMaxMP;
		cout << pNpcs[nRoleIndex].nMP << endl;
	}

	SystemPause();
}

//装备
void CGame::Equip(short nRoleIndex, short nGoodsLocation )
{
	short oldAttack, oldDefend, oldSpeed;
	oldAttack = GetAddedAttack( nRoleIndex );
	oldDefend = GetAddedDefend( nRoleIndex );
	oldSpeed = GetAddedSpeed( nRoleIndex );

	short nIndex = pNpcs[nRoleIndex].MyGoods[nGoodsLocation];
	UnEquip( nRoleIndex, pGoods[ nIndex ].nLocation );

	ShowNpcName( nRoleIndex );
	cout << "装备了";
	ShowGoodsName( pNpcs[nRoleIndex].MyGoods[nGoodsLocation] );
	cout <<"。"<<endl;
	SystemPause();

	pNpcs[nRoleIndex].MyEquipment[ pGoods[ nIndex ].nLocation ] = nIndex;
	RemoveGoods( nRoleIndex, nGoodsLocation, false );

	cout << "攻击："<< pNpcs[nRoleIndex].nAttack << " + " << oldAttack
		<< "\t---->\t"
		<< pNpcs[nRoleIndex].nAttack << " + " << GetAddedAttack(nRoleIndex)
		<< "\t(" << GetAddedAttack( nRoleIndex ) - oldAttack <<")" << endl;

	cout <<"护体："<< pNpcs[nRoleIndex].nDefend << " + " << oldDefend
		<< "\t---->\t"
		<< pNpcs[nRoleIndex].nDefend << " + " << GetAddedDefend(nRoleIndex)
		<< "\t(" << GetAddedDefend(nRoleIndex) - oldDefend <<")" << endl;

	cout <<"敏捷："<< pNpcs[nRoleIndex].nSpeed << " + " << oldSpeed
		<< "\t---->\t"
		<< pNpcs[nRoleIndex].nSpeed << " + " << GetAddedSpeed(nRoleIndex)
		<< "\t(" << GetAddedSpeed(nRoleIndex) - oldSpeed << ")" << endl;

	SystemPause();
}

//移除装备
void CGame::UnEquip(short nRoleIndex, short nLocation )
{
	short nGoosIndex = pNpcs[nRoleIndex].MyEquipment[ nLocation ];
	if ( nGoosIndex == -1 )
		return;

	ShowNpcName( nRoleIndex );
	cout<< "卸除了";
	ShowGoodsName( nGoosIndex );
	cout <<"。" << endl;
	SystemPause();

	AddGoods( nRoleIndex, nGoosIndex, false );
	pNpcs[nRoleIndex].MyEquipment[nLocation] = -1;
}

//使用物品
void CGame::UseGoods(short nRoleIndex, short nGoodsLocation )
{
	switch( pGoods[ pNpcs[nRoleIndex].MyGoods[nGoodsLocation] ].nProperty )
	{
	case 0:	//服用
		Eat( nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation] );
		RemoveGoods( nRoleIndex, nGoodsLocation, false);
		break;
	case 1: //装备
		Equip( nRoleIndex, nGoodsLocation );
		break;
    case 2:  //学习
		LearnSkill( nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation] );
        break;
	default:	//任务物品
        SystemWarning("不知道怎么使用这种东西！");
        cout << endl;
	}
}

//捡起物品。这里nGoodsLocation是指这个物品在场景物品中的位置
void CGame::PickGoods(short nRoleIndex, short nGoodsLocation )
{
	ShowNpcName( nRoleIndex );
	if( pNpcs[nRoleIndex].nGoodsNum == MAX_GOODS_NUM )
		cout<<"的行囊已满，放不下任何东西了。" << endl;
	else
	{
		cout << "拣起";
		ShowGoodsName( pSpots[oGameEnv.nCurrentMap].nGoods[nGoodsLocation]);
		cout << "。"<<endl;
		AddGoods( nRoleIndex, pSpots[oGameEnv.nCurrentMap].nGoods[nGoodsLocation],
			false);
		RmGoodsFromSpot( oGameEnv.nCurrentMap, nGoodsLocation );
	}

	SystemPause();
}

//从场景中去除某样物品
void CGame::RmGoodsFromSpot(short nSpotIndex, short nGoodsLocation )
{
    short i;
	for( i = nGoodsLocation; i < pSpots[nSpotIndex].nGoodsNum - 1; ++ i)
		pSpots[nSpotIndex].nGoods[i] = pSpots[ nSpotIndex ].nGoods[i+1];
	pSpots[nSpotIndex].nGoodsNum --;
}

//增加物品到场景中
void CGame::AddGoodsToSpot(short nSpotIndex, short nGoodsIndex )
{
	if( pSpots[nSpotIndex].nGoodsNum == MAX_GOODS_NUM )
	{
        short i;
		for( i = 0; i < pSpots[nSpotIndex].nGoodsNum - 1; ++ i)
			pSpots[nSpotIndex].nGoods[i] = pSpots[nSpotIndex].nGoods[i+1];
		pSpots[nSpotIndex].nGoods[i] = nGoodsIndex;
	}
	else
	{
		pSpots[nSpotIndex].nGoods[ pSpots[nSpotIndex].nGoodsNum ] = nGoodsIndex;
		pSpots[nSpotIndex].nGoodsNum ++;
	}
}

//计算战胜敌人后得到的经验
short CGame::Experience(short nRoleIndex, short nEnemyIndex )
{
	return 0;
}

//更新角色等级
void CGame::UpdateLevel( short nRoleIndex )
{
	//如果经验还不超过100%，返回
    /*
	if ( pNpcs[nRoleIndex].nExp < 100 )	return;

#ifdef _WIN32_
	SetConsoleTextAttribute( hConsoleHandle, ITEM_NAME_COLOR|0 );
#endif
	cout << pNpcs[nRoleIndex].stName << "级别提升！" << endl;
	SystemPause();

	cout << "级别：   "<<pNpcs[nRoleIndex].nLevel << "\t---->\t";
	pNpcs[nRoleIndex].nLevel ++;
	cout << pNpcs[nRoleIndex].nLevel << endl;

	cout <<"生命上限："<<pNpcs[nRoleIndex].nMaxHP << "\t---->\t";
	pNpcs[nRoleIndex].nMaxHP = pNpcs[nRoleIndex].nMaxHP +
		pNpcs[nRoleIndex].nLevel + rand()%6;
	cout << pNpcs[nRoleIndex].nMaxHP << endl;

	cout <<"内力上限："<<pNpcs[nRoleIndex].nMaxMP << "\t---->\t";
	pNpcs[nRoleIndex].nMaxMP = pNpcs[nRoleIndex].nMaxMP +
		pNpcs[nRoleIndex].nLevel + rand()%6;
	cout << pNpcs[nRoleIndex].nMaxMP << endl;

	cout <<"攻击    ："<<pNpcs[nRoleIndex].nAttack << "\t---->\t";
	pNpcs[nRoleIndex].nAttack = pNpcs[nRoleIndex].nAttack +
		pNpcs[nRoleIndex].nLevel/5 + rand()%2;
	cout << pNpcs[nRoleIndex].nAttack << endl;

	cout <<"护体    ："<<pNpcs[nRoleIndex].nDefend << "\t---->\t";
	pNpcs[nRoleIndex].nDefend = pNpcs[nRoleIndex].nDefend +
		(short)((float)pNpcs[nRoleIndex].nMaxHP * 12 / 100 );
	cout << pNpcs[nRoleIndex].nDefend << endl;

	cout <<"敏捷    ："<<pNpcs[nRoleIndex].nSpeed << "\t---->\t";
	pNpcs[nRoleIndex].nSpeed = pNpcs[nRoleIndex].nSpeed +
		pNpcs[nRoleIndex].nLevel/5 + rand()%2;
	cout << pNpcs[nRoleIndex].nSpeed << endl;

	Refresh( nRoleIndex );
	pNpcs[nRoleIndex].nExp =
		(short)((float)(pNpcs[nRoleIndex].nExp - 100) / pNpcs[nRoleIndex].nLevel);

	SystemPause();
    */
}

//计算伤害
short CGame::Damage(short nRoleIndex, short nEnemyIndex )
{
	short nAtk = pNpcs[nRoleIndex].nAttack + GetAddedAttack(nRoleIndex);
	short nDef = pNpcs[nEnemyIndex].nDefend + GetAddedDefend( nEnemyIndex );

	if ( nAtk > nDef )
		return  (nAtk - nDef + rand()%3) ;
	else
		return rand()%3 ;
    //return (int)(nAtk * (float)(100-nDef) / 100) + rand()%3;
}

//计算技能伤害
short CGame::SkillDamage(short nRoleIndex, short nSkillIndex )
{
	return (pSkills[ pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex ].nPower
            * pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel);
}

//技能消耗的内力
short CGame::SkillMpCost(short nRoleIndex, short nSkillIndex )
{
	return pSkills [ pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex ].nMpCost
		* pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel;
}

//技能升级
void CGame::SkillLevelUp(short nRoleIndex, short nSkillIndex )
{
	pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel ++;
	pNpcs[nRoleIndex].MySkills[nSkillIndex].nTimes = 0;
    short nHpAdded =
        pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel *
        pSkills[pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex].nAddHp;
    pNpcs[nRoleIndex].nMaxHP += nHpAdded;
    short nMpAdded =
        pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel *
        pSkills[pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex].nAddMp;
    pNpcs[nRoleIndex].nMaxMP += nMpAdded;
    short nAtkAdded =
        pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel *
        pSkills[pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex].nAddAttack;
    pNpcs[nRoleIndex].nAttack += nAtkAdded;
    short nDefAdded =
        pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel *
        pSkills[pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex].nAddDef;
    pNpcs[nRoleIndex].nDefend += nDefAdded;
    short nSpeedAdded =
        pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel *
        pSkills[pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex].nAddSpeed;
    pNpcs[nRoleIndex].nSpeed += nSpeedAdded;

    cout << endl;
	ShowNpcName( nRoleIndex );
	cout << "的" ;
	ShowSkillName( pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex );
	cout << "升到第" << pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel
		<<"重！"<<endl;
    cout << "生命上限上升" << nHpAdded << "；" << endl;
    cout << "内力上限上升" << nMpAdded << "；" << endl;
    cout << "攻击上升" << nAtkAdded << "；" << endl;
    cout << "防御上升" << nDefAdded << "；" << endl;
    cout << "身法上升" << nSpeedAdded << "；" << endl;

	SystemPause();
}

//更新技能等级
void CGame::UpdateSkillLevel(short nRoleIndex, short nSkillIndex )
{
    short global_skill_index = pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillIndex;
    short local_skill_level = pNpcs[nRoleIndex].MySkills[nSkillIndex].nSkillLevel;

    short nExpAdded = local_skill_level * pSkills[global_skill_index].nAddExp;
    pNpcs[nRoleIndex].nExps[ pSkills[global_skill_index].nSklCls] += nExpAdded;
    cout << stSkillNicks[pSkills[nSkillIndex].nSklCls] << "修为增加"
         << nExpAdded << endl;
    UpdateNick(nRoleIndex);

	if ( local_skill_level == MAX_SKILL_LEVEL )
		return;

	pNpcs[ nRoleIndex ].MySkills[nSkillIndex].nTimes ++;
    if ( pNpcs[nRoleIndex].MySkills[nSkillIndex].nTimes ==
         local_skill_level * pSkills[ global_skill_index].nLvlUp){
        SkillLevelUp( nRoleIndex, nSkillIndex );
    }
}

void CGame::Refresh(short nRoleIndex )
{
	pNpcs[nRoleIndex].nHP = pNpcs[nRoleIndex].nMaxHP;
	pNpcs[nRoleIndex].nMP = pNpcs[nRoleIndex].nMaxMP;
}

short CGame::GetAddedAttack( short nRoleIndex )
{
	short nAdd = 0;
    short i;
	for( i =0; i < 4; ++ i )
	{
		if( pNpcs[nRoleIndex].MyEquipment[i] != -1 )
			nAdd += pGoods[ pNpcs[nRoleIndex].MyEquipment[i] ].nAddAttack ;
	}

	return nAdd;
}

short CGame::GetAddedDefend(short nRoleIndex )
{
	short nAdd = 0;
    short i;
	for( i =0; i < 4; ++ i )
	{
		if( pNpcs[nRoleIndex].MyEquipment[i] != -1 )
			nAdd += pGoods[ pNpcs[nRoleIndex].MyEquipment[i] ].nAddDefence ;
	}

	return nAdd;
}


short CGame::GetAddedSpeed(short nRoleIndex )
{
	short nAdd = 0;
    short i;
	for( i =0; i < 4; ++ i )
	{
		if( pNpcs[nRoleIndex].MyEquipment[i] != -1 )
			nAdd += pGoods[ pNpcs[nRoleIndex].MyEquipment[i] ].nAddSpeed ;
	}

	return nAdd;
}
