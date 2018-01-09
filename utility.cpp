/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

#include "utility.hpp"
#include <DxLib.h>
namespace legacy {
	int MessageBoxYesNo(LPCTSTR lpText) noexcept
	{
		return MessageBox(
			DxLib::GetMainWindowHandle(),
			lpText,
			"ゲームリンクス制作のノベルゲームエンジン「LINKS」",
			MB_YESNO
		);
	}
	int MessageBoxOk(LPCTSTR lpText) noexcept
	{
		return MessageBox(
			DxLib::GetMainWindowHandle(),
			lpText,
			"ゲームリンクス制作のノベルゲームエンジン「LINKS」",
			MB_OK
		);
	}
}

//画面クリア処理関数
void SCREEN_CLEAR() noexcept {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	SetDrawScreen(DX_SCREEN_FRONT);
}

bool DrawLoopController() noexcept {
	return -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen();
}
