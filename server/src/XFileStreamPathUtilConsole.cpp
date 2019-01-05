/*
*   File name: XFileStreamPathUtilConsole.cpp
*
*  Created on: 2012.10.10
*      Author: CaiZhaoDong
*     Version: 1.0
*  Contact me: cichiy08110@hotmail.com
* Description: Console implementation for file path utility
*/

#include "stdafx.h"
#include "IO/XFileStream.h"

extern char g_pLaunchPath[];

void XFileStream::InitPathUtil()
{
	//resource path
	strcpy(m_pResourceDir, g_pLaunchPath);
	strcat(m_pResourceDir, "/data/");

	//save file path
	strcpy(m_pSaveDir, g_pLaunchPath);
	strcat(m_pSaveDir, "/save/");
	//CreateDirectory(m_pSaveDir, NULL);

	//temp file path
	strcpy(m_pTempDir, g_pLaunchPath);
	strcat(m_pTempDir, "/temp/");
	//CreateDirectory(m_pTempDir, NULL);

	//config file path
	strcpy(m_pConfigDir, g_pLaunchPath);
	strcat(m_pConfigDir, "/config/");
}