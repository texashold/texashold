@echo off
echo **************************************
echo *** Generate make file  
echo **************************************

sln2mak.exe ../win32/TexasPokerServer.sln
copy /y ..\win32\TexasPokerServer.mak ..\linux\TexasPokerServer.mak
copy /y ..\win32\Makefile            ..\linux\Makefile
pause