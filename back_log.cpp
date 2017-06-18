﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#include "back_log.hpp"
#include "DEF.h"
#include "utility.hpp"
#include "keystate.hpp"
#include <DxLib.h>
#include "fmt/fmt/format.h"
#include "GAME.h"

namespace {
	int BACKLOG_HANDLE, BACKLOG[11], BACKLOG_BACKGROUND;
	int LOG = 1, BACKLOG_COUNT = 1;

	//バックログ参照メッセージ
	int backLogMessage() noexcept {
		return MessageBoxYesNo("バックログ画面に移行しますか？");
	}

	//バックログ(キー操作関連)
	void BACKLOG_KEY_MOVE(KeyState& key) noexcept {
		key.update();
		//バックログ（キー操作関連）
		if (LOG != 10 && key.up() || LOG != 10 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			LOG++;
			key.flush();
		}
		if (LOG != 1 && key.down() || LOG != 1 && (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			LOG--;
			key.flush();
		}
	}

	//バックログの描画関数
	void BACKLOG_SCREENSHOT_DRAW() noexcept {
		if (0 < LOG && LOG <= 10) {
			DrawGraph(0, 0, BACKLOG[LOG - 1], TRUE);
			try {
				DrawString(0, 450, fmt::format("バックログ{0}", LOG).c_str(), Cr);
			}
#if defined(_MSC_VER) && defined(_DEBUG)
			catch (const std::exception& er) {
				//例外が投げられた場合特にできることがないので握りつぶす
				OutputDebugStringA(er.what());
			}
#endif
			catch (...) {}
		}
	}
}
//バックログ参照
void BACKLOG_DRAW(KeyState& key) noexcept {
	//バックログ参照メッセージ
	if (backLogMessage() == IDYES) {

		WaitTimer(600);//キー判定消去待ち目的ではない

		LOG = 1;

		while (ProcessMessage() == 0) {

			//バックログ（キー操作関連）
			BACKLOG_KEY_MOVE(key);

			//バックログの描画関数
			BACKLOG_SCREENSHOT_DRAW();

			//画面クリア処理
			SCREEN_CLEAR();

			//終了処理
			key.update();
			if (key.enter() || (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				ClearDrawScreen();

				DeleteGraph(BACKLOG[0]);

				key.flush();

				//ショートカットキー時の事後処理
				SHORTCUT_KEY_DRAW();
				break;
			}
		}
	}
}

//バックログ取得関数
void BACKLOG_GET() noexcept {
	if (0 < BACKLOG_COUNT) {
		const int BacklogCount = (10 < BACKLOG_COUNT) ? 10 : BACKLOG_COUNT;
		if (1 < BacklogCount) {
			BACKLOG_BACKGROUND = BACKLOG[1];
			for (int i = BacklogCount - 1; 1 < i; --i) {
				BACKLOG[i] = DerivationGraph(0, 0, 640, 480, BACKLOG[i - 1]);
			}
			BACKLOG[1] = LoadGraph("DATA/BACKLOG/BACKLOG1.png");
			SaveDrawScreen(0, 0, 640, 480, "DATA/BACKLOG/BACKLOG1.png");
		}
		else {
			SaveDrawScreenToPNG(0, 0, 640, 480, "DATA/BACKLOG/BACKLOG1.png");
		}
		BACKLOG_HANDLE = BACKLOG[0] = LoadGraph("DATA/BACKLOG/BACKLOG1.png");
	}
}

void incrementBackLogCount() noexcept
{
	++BACKLOG_COUNT;
}
