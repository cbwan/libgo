// FuegoWin.cpp : Defines the entry point for the console application.
//

#include "SgSystem.h"

#include "FuegoMainEngine.h"
#include "FuegoMainUtil.h"
#include "GoInit.h"
#include "GoBoard.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgInit.h"
#include "SgPlatform.h"

#include <iostream>
using namespace std;


int main(int argc, char* argv[])
{
/*	options.m_programPath = argv[0];
    SgPlatform::SetProgramDir(GetProgramDir(argv[0]));
    SgPlatform::SetTopSourceDir(GetTopSourceDir());*/

	SgInit();
	GoInit();
	FuegoMainEngine engine(19);

	const GoBoard& board = engine.Board();

	engine.Play( SG_BLACK, SgPointUtil::Pt(1,1));
	//board.Play( SgPointUtil::Pt(1,1) );
	cout << "Legal (X): " << board.IsLegal( SgPointUtil::Pt(1,1) ) << endl;
	cout << "Legal (O): " << board.IsLegal( SgPointUtil::Pt(1,2) ) << endl;

	// White plays right
	engine.Play( SG_WHITE, SgPointUtil::Pt(1,2) );

	// Black moves away
	engine.Play( SG_BLACK, SgPointUtil::Pt(1,6) );

	// White eats Black
	engine.Play( SG_WHITE, SgPointUtil::Pt(2,1) );

	// Black can't play here
	cout << "Legal (X): " << board.IsLegal( SgPointUtil::Pt(1,1) ) << endl;

	// Black moves away
	engine.Play( SG_BLACK, SgPointUtil::Pt(1,7) );

	// White plays here
	cout << "Legal (O): " << board.IsLegal( SgPointUtil::Pt(1,1) ) << endl;

	// White takes Black's position
	engine.Play( SG_BLACK, SgPointUtil::Pt(1,1) );

	float komi = board.Rules().Komi().ToFloat();
    float score = GoBoardUtil::Score(board, komi);

	cout << "Score: " << score << endl;
	/*

	const GoBoard& board = engine.Board();

	//engine.Board().Play( (SgPoint)0 ); //, SG_BLACK );
	
	GtpCommand cmd("b A1");

	engine.CmdPlay(cmd);

	//board

	//engine.Play( SG_BLACK, SgPointUtil::Pt(0,0) );
	*/


	GoFini();

	return 0;
}

