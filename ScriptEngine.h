
#ifndef __SCRIPT_ENGINE__
#define __SCRIPT_ENGINE__

class CGame;

/* a script line struct */
struct CScriptLine
{
	short nScriptIndex;
	char stScriptLine[ 256 ];
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

    CScriptLine * pScripts;
    CGame * pGameEngine;
	short nScriptsNumber;
	short nScriptLineNumber;	//script lines' number
	short nCurLineIndex;		//current scripts Index
	short nIndexInCurLine;		// the index in current script line
    bool iRunningScripts;
};

#endif
