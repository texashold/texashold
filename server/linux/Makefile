# Makefile - TexasPokerServer

.PHONY: all
all: \
 	TexasPokerServer


.PHONY: TexasPokerServer
TexasPokerServer:
	$(MAKE) -f TexasPokerServer.mak
	
.PHONY: debug
debug:
	$(MAKE) -f TexasPokerServer.mak -e CFG=Debug
	
.PHONY: release
release:
	$(MAKE) -f TexasPokerServer.mak -e CFG=Release
	
.PHONY: special
special:
	$(MAKE) -f TexasPokerServer.mak -e CFG=Special
	cp TexasPokerServer.exe -f ../../bin/TexasPokerServerSpecial

.PHONY: clean
clean:
	$(MAKE) -f TexasPokerServer.mak clean

.PHONY: depends
depends:
	$(MAKE) -f TexasPokerServer.mak depends

