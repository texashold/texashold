/**
 * The const definition for Online system.
 */

#ifndef _ONLINE_DEFINE_
#define _ONLINE_DEFINE_

//Online error definition
enum eOnlineError{
	//Protocol error
	OnlinePErrorUnknown          = 1001,
	OnlinePErrorNetWork          = 1002,
	OnlinePErrorBadResponse      = 1003,
	OnlinePErrorLoseConnection   = 1004,
	OnlinePErrorTimeOut          = 1005,


	//Function error
	OnlineNoError                = 1,
};

//Function definition
enum eOnlineFunction{
	OnlineFunctionTest                 = 0,
	OnlineFunctionStartConfirm,
	OnlineFunctionKeepAlive,
	OnlineFunctionCloseRoom,
	OnlineFunctionPlayerSit,
	OnlineFunctionPlayerStand,
	OnlineFunctionGameClear,
	OnlineFunctionSelectAIPlayerSit,
	OnlineFunctionGetTotalGamePlayerNum,
	OnlineFunctionGiveChip,
	OnlineFunctionGetPlayerItem,
	OnlineFunctionGetIGTInfo,
	OnlineFunctionKickPlayer,
	OnlineFunctionLockRoom,
	OnlineFunctionUnlockRoom,
	OnlineFunctionTournamentSit,
	OnlineFunctionTournamentExit,
	OnlineFunctionTournamentStart,
	OnlineFunctionTournamentClear,
	OnlineFunctionUpdatePlayerNum = 23,
	OnlineFunctionUpdatePlayerInfo = 24,
	OnlineFunctionChangeDealer = 25,
	OnlineFunctionTranslate = 26,
};

#endif//_ONLINE_DEFINE_