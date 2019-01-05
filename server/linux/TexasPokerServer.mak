# Makefile - TexasPokerServer

ifndef CFG
CFG=Debug
endif
CC=gcc
CFLAGS=-m64 -fpic
CXX=g++
CXXFLAGS=$(CFLAGS)
ifeq "$(CFG)" "Debug"
CFLAGS+=  -W  -O0 -fexceptions -g  -fno-inline   -D_DEBUG -D_CONSOLE  -I ./ -I ../src/ -I ../src/Online -I ../../src/XGame -I ../../src/TexasPoker
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
LDFLAGS+= 
LIBS+=-lstdc++ -lm -lpthread 
endif
ifeq "$(CFG)" "Release"
CFLAGS+=  -W  -O2 -fexceptions -g  -fno-inline   -DNDEBUG -D_CONSOLE  -I ./ -I ../src/ -I ../src/Online -I ../../src/XGame -I ../../src/TexasPoker 
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
LDFLAGS+= 
LIBS+=-lstdc++ -lm -lpthread 
endif
ifeq "$(CFG)" "Special"
CFLAGS+=  -W  -O0 -fexceptions -g  -fno-inline   -D_DEBUG -D_CONSOLE -DSPECIAL_EDITION  -I ./ -I ../src/ -I ../src/Online -I ../../src/XGame -I ../../src/TexasPoker
#CFLAGS+=  -W  -O2 -fexceptions -g  -fno-inline   -DNDEBUG -D_CONSOLE -DSPECIAL_EDITION  -I ./ -I ../src/ -I ../src/Online -I ../../src/XGame -I ../../src/TexasPoker 
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
LDFLAGS+= 
LIBS+=-lstdc++ -lm -lpthread 
endif
ifndef TARGET
TARGET=TexasPokerServer.exe
endif
.PHONY: all
all: $(TARGET)

%.o: %.c
#	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

%.res: %.rc
	$(RC) $(CPPFLAGS) -o $@ -i $<

SRC= \
	./predefine.h \
	./stdafx.h

TEXASPOKER= \
	../../src/TexasPoker/CardSet.cpp \
	../../src/TexasPoker/CardSet.h \
	../../src/TexasPoker/StringUtil.cpp \
	../../src/TexasPoker/StringUtil.h \
	../../src/TexasPoker/CSCommonDefine.h \
	../../src/TexasPoker/MessageDefine.h

TEXASPOKER_SERVER= \
	../../src/TexasPoker/Server/Server.cpp \
	../../src/TexasPoker/Server/Server.h \
	../../src/TexasPoker/Server/GameClearMsg.cpp \
	../../src/TexasPoker/Server/GameClearMsg.h \
	../../src/TexasPoker/Server/ServerCard.cpp \
	../../src/TexasPoker/Server/ServerCard.h \
	../../src/TexasPoker/Server/ServerChipPool.cpp \
	../../src/TexasPoker/Server/ServerChipPool.h \
	../../src/TexasPoker/Server/ServerPlayer.cpp \
	../../src/TexasPoker/Server/ServerPlayer.h

TEXASPOKER_SERVER_AI= \
	../../src/TexasPoker/Server/AI/AIConfigInfo.cpp \
	../../src/TexasPoker/Server/AI/AIConfigInfo.h

TEXASPOKER_ITEM= \
	../../src/TexasPoker/Item/Item.cpp \
	../../src/TexasPoker/Item/Item.h

XGAME= \
	../../src/XGame/XApplication.cpp \
	../../src/XGame/XApplication.h

XGAME_BASE= \
	../../src/XGame/Base/XEventCenter.cpp \
	../../src/XGame/Base/XEventCenter.h \
	../../src/XGame/Base/XInvalidObjectPool.cpp \
	../../src/XGame/Base/XInvalidObjectPool.h \
	../../src/XGame/Base/XMath.cpp \
	../../src/XGame/Base/XMath.h \
	../../src/XGame/Base/XObject.cpp \
	../../src/XGame/Base/XObject.h \
	../../src/XGame/Base/XPoint.cpp \
	../../src/XGame/Base/XPoint.h \
	../../src/XGame/Base/XRect.cpp \
	../../src/XGame/Base/XRect.h \
	../../src/XGame/Base/XScheduleMgr.cpp \
	../../src/XGame/Base/XScheduleMgr.h \
	../../src/XGame/Base/XSize.cpp \
	../../src/XGame/Base/XSize.h \
	../../src/XGame/Base/XTime.cpp \
	../../src/XGame/Base/XTime.h

XGAME_COMMON= \
	../../src/XGame/Common/XAssert.h \
	../../src/XGame/Common/XDefine.h \
	../../src/XGame/Common/XLog.cpp \
	../../src/XGame/Common/XLog.h

XGAME_CONNECTION= \
	../../src/XGame/Connection/XConnection.cpp \
	../../src/XGame/Connection/XConnection.h \
	../../src/XGame/Connection/XConnectionObserver.h \
	../../src/XGame/Connection/XLocalConnection.cpp \
	../../src/XGame/Connection/XLocalConnection.h \
	../../src/XGame/Connection/XMemoryBuffer.cpp \
	../../src/XGame/Connection/XMemoryBuffer.h \
	../../src/XGame/Connection/XMessage.cpp \
	../../src/XGame/Connection/XMessage.h \
	../../src/XGame/Connection/XSMessage.cpp \
	../../src/XGame/Connection/XSMessage.h \
	../../src/XGame/DevUtil/MD5.c \
	../../src/XGame/DevUtil/MD5.h \
	../../src/XGame/Connection/XNullConnection.cpp \
	../../src/XGame/Connection/XNullConnection.h \
	../../src/XGame/Connection/XSocketConnection.cpp \
	../../src/XGame/Connection/XSocketConnection.h \
	../../src/XGame/Connection/XSocketConnectionTCP.cpp \
	../../src/XGame/Connection/XSocketConnectionTCP.h \
	../../src/XGame/Connection/XSocketConnectionUDP.cpp \
	../../src/XGame/Connection/XSocketConnectionUDP.h

XGAME_IO= \
	../../src/XGame/IO/XFileStream.cpp \
	../../src/XGame/IO/XFileStream.h \
	../../src/XGame/IO/XMemoryStream.cpp \
	../../src/XGame/IO/XMemoryStream.h \
	../../src/XGame/IO/XStream.cpp \
	../../src/XGame/IO/XStream.h

XGAME_HTTP= \
	../../src/XGame/Http/XHttpConnection.cpp \
	../../src/XGame/Http/XHttpConnection.h

TEXASPOKERSERVER= \
	../src/GameController.cpp \
	../src/GameController.h \
	../src/WordFilter.cpp \
	../src/WordFilter.h \
	../main.cpp \
	../src/XConsoleApplication.cpp \
	../src/XConsoleApplication.h \
	../src/XFileStreamPathUtilConsole.cpp

TEXASPOKERSERVER_ONLINE= \
	../src/Online/OnlineCloseRoom.cpp \
	../src/Online/OnlineCloseRoom.h \
	../src/Online/OnlineDefines.h \
	../src/Online/OnlineGameClear.cpp \
	../src/Online/OnlineGameClear.h \
	../src/Online/OnlineGetIGTInfo.cpp \
	../src/Online/OnlineGetIGTInfo.h \
	../src/Online/OnlineGetPlayerItem.cpp \
	../src/Online/OnlineGetPlayerItem.h \
	../src/Online/OnlineGetTotalGamePlayerNum.cpp \
	../src/Online/OnlineGetTotalGamePlayerNum.h \
	../src/Online/OnlineGiveChip.cpp \
	../src/Online/OnlineGiveChip.h \
	../src/Online/OnlineKeepAlive.cpp \
	../src/Online/OnlineKeepAlive.h \
	../src/Online/OnlineKickPlayer.cpp \
	../src/Online/OnlineKickPlayer.h \
	../src/Online/OnlineLockRoom.cpp \
	../src/Online/OnlineLockRoom.h \
	../src/Online/OnlinePlayerSit.cpp \
	../src/Online/OnlinePlayerSit.h \
	../src/Online/OnlinePlayerStand.cpp \
	../src/Online/OnlinePlayerStand.h \
	../src/Online/OnlineRequestManager.cpp \
	../src/Online/OnlineRequestManager.h \
	../src/Online/OnlineSelectAIPlayerSit.cpp \
	../src/Online/OnlineSelectAIPlayerSit.h \
	../src/Online/OnlineStartConfirm.cpp \
	../src/Online/OnlineStartConfirm.h \
	../src/Online/OnlineUnlockRoom.cpp \
	../src/Online/OnlineUnlockRoom.h \
	../src/Online/OnlineTournamentSit.cpp \
	../src/Online/OnlineTournamentSit.h \
	../src/Online/OnlineTournamentExit.cpp \
	../src/Online/OnlineTournamentExit.h \
	../src/Online/OnlineTournamentStart.cpp \
	../src/Online/OnlineTournamentStart.h \
	../src/Online/OnlineTournamentClear.cpp \
	../src/Online/OnlineTournamentClear.h \
	../src/Online/OnlineTranslate.cpp \
	../src/Online/OnlineTranslate.h \
	../src/Online/OnlineUpdatePlayerNum.cpp \
	../src/Online/OnlineUpdatePlayerNum.h  \
	../src/Online/OnlineUpdatePlayerInfo.cpp \
	../src/Online/OnlineUpdatePlayerInfo.h	\
	../src/Online/OnlineChangeDealer.cpp \
	../src/Online/OnlineChangeDealer.h

TEXASPOKERSERVER_ONLINE_COMMON= \
	../src/Online/Common/OnlineComponent.cpp \
	../src/Online/Common/OnlineComponent.h \
	../src/Online/Common/OnlineObserver.h \
	../src/Online/Common/ServerAddr.cpp \
	../src/Online/Common/ServerAddr.h \
	../src/Online/Common/OnlineTryInvalidAddr.cpp \
	../src/Online/Common/OnlineTryInvalidAddr.h

TEXASPOKERSERVER_ONLINE_JSON= \
	../src/Online/jsoncpp/src/json_reader.cpp \
	../src/Online/jsoncpp/src/json_value.cpp \
	../src/Online/jsoncpp/src/json_writer.cpp

SRCS=$(SRC) $(TEXASPOKER) $(TEXASPOKER_SERVER) $(TEXASPOKER_SERVER_AI) $(TEXASPOKER_ITEM) $(XGAME) $(XGAME_BASE) $(XGAME_COMMON) $(XGAME_CONNECTION) $(XGAME_IO) $(XGAME_HTTP) $(TEXASPOKERSERVER) $(TEXASPOKERSERVER_ONLINE) $(TEXASPOKERSERVER_ONLINE_COMMON) $(TEXASPOKERSERVER_ONLINE_JSON) 

OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(notdir $(OBJS)) $(LIBS)

.PHONY: clean
clean:
	-rm -f -v $(notdir $(OBJS)) $(TARGET) TexasPokerServer.dep

.PHONY: depends
depends:
	-$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(filter %.c %.cc %.cpp %.cxx,$(SRCS)) > TexasPokerServer.dep

-include TexasPokerServer.dep

