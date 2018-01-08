﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#ifndef LINKS_SAVE_HPP_
#define LINKS_SAVE_HPP_

#include <cstdint>
#include "keystate.hpp"

extern int SAVE_CHOICE;

void SAVEDATA_DELETE(KeyState& key);
int SAVEDATA_LOAD(KeyState& key);
void SAVEDATA_SAVE(KeyState& key);

void SAVESNAP();

void SCRIPT_OUTPUT_CHOICE_LOOP_SAVESNAP();
void Mouse_Move_SAVE(int MouseY);

void NotifySaveSnap();
void NotifyDisableSaveSnap();

//通常セーブデータ
struct alignas(4) SaveData_t {
	std::int32_t ENDFLAG;    //ENDFLAG
	std::int32_t SP;			//行数
	std::int32_t CP;			//文字位置
	std::int32_t CHAR;		//立ち絵情報
	std::int32_t BG;			//背景画像情報
	std::int32_t BGM;		//BGM情報
	std::int32_t SAVE_CHOICE;//選択肢画面でのセーブ情報
};

#endif
