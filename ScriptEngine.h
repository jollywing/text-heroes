
#ifndef __SCRIPT_ENGINE__
#define __SCRIPT_ENGINE__

#include <stack>

#define SCRIPT_NAME_LEN 36
#define SCRIPT_LINE_LEN 512
#define SCRIPT_NUM_BUF_LEN 9
#define SCRIPT_VAR_BUF_LEN 32
#define SCRIPT_CMD_LEN 24
#define VAR_COUNT 64

class CGame;

/* a script line struct */
struct CScriptLine
{
	short nScriptIndex;
	char stScriptLine[ 256 ];
};

/* variable */
struct CVar
{
	char szName[32];
	short nValue;
};


class ScriptEngine
{
  public:
    ScriptEngine();
    ~ScriptEngine();
	bool LoadScripts( char * pstFileName, char cHeadFlag);	// load scripts
    bool FindScripts( short nScriptsID );	//find scripts whose id is identical to that given
	void RunScripts( short nScriptID, CGame *pGameEngine );	// run scripts

  private:
	void ReadSubString( char * stString, char * stSubString );
	void ExecuteScriptLine( char * stScriptLine, CGame *pGameEngine );
    char GotoBranchEnd();
    char GotoNextBranch();
    bool Expression();



    void ClearVars();
    short GetVar( const char * szName );
    bool SetVar( const char * szName, short nValue);
    bool IsVarExist( char * szName );
    bool CreateVar( char * szName, short nValue = 0);


    CScriptLine * pScripts;
    CGame * pGameEngine;
	short nScriptsNumber;
	short nScriptLineNumber;	//script lines' number
	short nCurLineIndex;		//current scripts Index
	short nIndexInCurLine;		// the index in current script line
    bool iRunningScripts;

    stack<char> branchStack;
    CVar vars[VAR_COUNT];

};

#endif
