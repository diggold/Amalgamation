#pragma once


enum class GameStates
{
	INIT, MENU, LOBBY, LOADING, INGAME, PAUSE, NUMSTATES
};


enum class EnumGameDelta
{
	NOTCHANGED, ENTEREDTHISFRAME, EXITTHISFRAME,
};


enum class ServerStates
{
	INIT, LOBBY, LOADING, INGAME, RESULT, NUMSTATES
};
