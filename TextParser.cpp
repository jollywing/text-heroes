
#include "TextParser.h"

/* construct object with given file name
   and calculate records number */
CTextParser::CTextParser(const char * stFileName, const char cHeadFlag)
{
	//open given file
	pParserFile.open( stFileName );
	if( !pParserFile )
	{
		cerr << stFileName << "open failed!" << endl;
		exit(1);
	}

	//get the number of records
	cRecordHead = cHeadFlag;
	nRecordNum = 0;
	while(FindRecordHead( ))
	{	
		++ nRecordNum;		
	}

	// move pointer to file head
	GotoFileHead();

}

CTextParser::~CTextParser()
{
	// disconnect file and file handle
	//if ( pParserFile.is_open())
	pParserFile.close();
}

/* get record number */
short CTextParser::GetRecordNum()
{
	return nRecordNum;
}

/* find where a record begin */
bool CTextParser::FindRecordHead( )
{
	//read char until cRecordHead is readed
	char c;
	while (!pParserFile.eof())
	{
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
		if (c == cRecordHead)
			return true;
	}

	return false;
}

/* get a string from current file */
void CTextParser::ReadString( char * stString )
{
	//read char, place it into stString, until eof or '/n'
	char c;
	int index = 0;
	pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	
	while ((c == ' ' || c == '\t') && !pParserFile.eof())
	{
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	}
	while ( c != ' ' && c != '\t'  && !pParserFile.eof() )
	{
		stString[index] = c;
		index++;
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));		
	}
	stString[index] = '\0';
}

/* get a integer */
void CTextParser::ReadInteger( short * nInt )
{
	char c;
	char stInteger[10];
	short nIndex = 0;

	//ignore front ' '
	pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	while ( (c == ' ' || c == '\t') && !pParserFile.eof())
	{
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	}

	//read string until ' ' 
	while ( c != ' ' && c != '\t')
	{
		stInteger[ nIndex ] = c;
		nIndex ++;
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	}

	stInteger[ nIndex ] = '\0';

	*nInt = atoi(stInteger);
}

/* goto next line */
void CTextParser::GotoNextLine()
{
	char c;
	pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	while( c != '\n')
	{
		pParserFile.read(reinterpret_cast<char *>(&c), sizeof(c));
	}

}

/* get a  line */
bool CTextParser::GetWholeLine( char * stString )
{
	char c;

	if ( pParserFile.eof() )
		return false;

	// ignore space line
	do {
		pParserFile.read( reinterpret_cast<char *>(&c), sizeof(c));
	} while( c == '\n' && !pParserFile.eof() );

	if ( pParserFile.eof() )
		return false;

	short nIndex = 0;

	// read whole line
	do {
		stString[ nIndex ] = c;
		nIndex ++ ;
		pParserFile.read ( reinterpret_cast < char * >( &c ), sizeof( c ));
	} while( c != '\n' && !pParserFile.eof() );

	stString[ nIndex ] = '\0';

	return true;

}


/* go to file head */
void CTextParser::GotoFileHead()
{
	pParserFile.clear();
	pParserFile.seekg(0, ios::beg);
}

