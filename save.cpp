﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

#include "save.hpp"
#include "DEF.h"
#include "utility.hpp"
#include "resource_manager.hpp"
#include "keystate.hpp"
#include "scoped_screen.hpp"
#include "GAME.h"
#include <DxLib.h>
#include <chrono>
#include <thread>

int SAVE_CHOICE = 0;
static int SAVESNAP1, SAVESNAP2, SAVESNAP3, SAVETITLE;
static int SAVESNAP_HANDLE1 = 0, SAVESNAP_HANDLE2 = 0, SAVESNAP_HANDLE3 = 0, SAVESNAP_CHOICE = 0;

static int SAVE_y = save_base_pos_y;

void setSaveSnapChoice(bool b) {
	SAVESNAP_CHOICE = b;
}

namespace {
	//セーブデータ一覧描画
	void SAVEDATA_DRAW() {

		//スクリーンショット描画
		DrawRotaGraph(saved_snap_draw_pos_x, save_base_pos_y, 0.2f, 0, SAVESNAP1, TRUE);
		DrawRotaGraph(saved_snap_draw_pos_x, save_base_pos_y * 2, 0.2f, 0, SAVESNAP2, TRUE);
		DrawRotaGraph(saved_snap_draw_pos_x, save_base_pos_y * 3, 0.2f, 0, SAVESNAP3, TRUE);

		//セーブデータ名描画
		DrawString(save_name_pos_x, save_base_pos_y, "セーブデータ1", Cr);
		DrawString(save_name_pos_x, save_base_pos_y * 2, "セーブデータ2", Cr);
		DrawString(save_name_pos_x, save_base_pos_y * 3, "セーブデータ3", Cr);

		DrawString(save_name_pos_x - cursor_move_unit, save_base_pos_y * 4, "戻る", Cr);
		static_assert(0 < (save_name_pos_x - cursor_move_unit), "error");
	}

	//セーブデータスクリーンショット読込
	void SAVEDATA_SCREENSHOT_READ() {

		//セーブ時のスクリーンショット読込
		SAVESNAP1 = LoadGraph("DATA/SAVE/SAVESNAP1.png");
		SAVESNAP2 = LoadGraph("DATA/SAVE/SAVESNAP2.png");
		SAVESNAP3 = LoadGraph("DATA/SAVE/SAVESNAP3.png");
		SAVETITLE = LoadGraph("DATA/BACKGROUND/SAVE.png");

		WaitTimer(600);//キー判定消去待ち目的ではない(セーブデータ画面の描画遅延処理)
	}

	//セーブ後のメッセージ
	template<typename ExectorType>
	void SAVE_MESSAGE(KeyState& key, ExectorType t) noexcept {
		MessageBoxOk("セーブしました！", key, t);
	}
	//セーブ後の処理(サウンドノベル風)
	void SAVE_SOUNDNOVEL() {

		//サウンドノベル風描画時の処理
		SOUNDNOVEL();

		SAVE_CHOICE = 0;

		GAMEMENU_COUNT = true;
	}

	//セーブ後の処理(ウインドウ風)
	void SAVE_WINDOWNOVEL() {

		//ウインドウ風描画時の処理
		WINDOWNOVEL();

		SAVE_CHOICE = 0;

		GAMEMENU_COUNT = true;
	}

	static int CreateSaveData(int* SaveSnapHandle, const char* Message, const char* ImagePath, const char* SaveDataPath, KeyState& key) {
		//TODO: このif文は呼び出し元のループに持っていく
		if (IDYES == MessageBoxYesNo(Message, key, KeyState::Executor::none, KeyState::Executor::flush_update)) {
			//セーブデータ１用のスクリーンショット取得変数
			*SaveSnapHandle = 1;

			//選択肢画面でのセーブ処理
			if (SAVESNAP_CHOICE != 0) {
				scoped_screen screen(DX_SCREEN_BACK);
				//TODO: これはなんなのか突き止める。SAVESNAP_CHOICEってフラグなのかハンドルなのか・・・
				DrawGraph(0, 0, SAVESNAP_CHOICE, TRUE);
				SaveDrawScreenToPNG(0, 0, 640, 480, ImagePath, 0);
				SAVESNAP_CHOICE = 0;
				*SaveSnapHandle = 0;
			}

			//セーブデータの作成
			SaveData_t Data = { EndFlag, SP, 0, charactor.activeResource(), background.activeResource(), backgroundMusic.activeResource(), SAVE_CHOICE };
			FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
			const errno_t er = fopen_s(&fp, SaveDataPath, "wb");
			if (0 != er || nullptr == fp) {
				return 0;
			}
#else
			fp = fopen(SaveDataPath, "wb");//バイナリファイルを開く
			if (nullptr == fp) {//エラーが起きたらnullptrを返す
				return 0;
			}
#endif
			fwrite(&Data, sizeof(Data), 1, fp); // SaveData_t構造体の中身を出力
			fclose(fp);
			//セーブ後のメッセージ
			SAVE_MESSAGE(key, KeyState::Executor::flush_update);
			//サウンドノベル風描画時の処理
			SAVE_SOUNDNOVEL();
			//ウインドウ風描画時の処理
			SAVE_WINDOWNOVEL();
		}

		return 0;
	}
	//セーブデータ１にセーブ
	int SAVEDATA_1_SAVE(KeyState& key) {
		return CreateSaveData(
			&SAVESNAP_HANDLE1,
			"セーブデータ1にセーブしますか？",
			"DATA/SAVE/SAVESNAP1.png",
			"DATA/SAVE/SAVEDATA1.dat",
			key
		);
	}

	//セーブデータ2にセーブ
	int SAVEDATA_2_SAVE(KeyState& key) {
		return CreateSaveData(
			&SAVESNAP_HANDLE2,
			"セーブデータ2にセーブしますか？",
			"DATA/SAVE/SAVESNAP2.png",
			"DATA/SAVE/SAVEDATA2.dat",
			key
		);
	}

	//セーブデータ3にセーブ
	int SAVEDATA_3_SAVE(KeyState& key) {
		return CreateSaveData(
			&SAVESNAP_HANDLE3,
			"セーブデータ3にセーブしますか？",
			"DATA/SAVE/SAVESNAP3.png",
			"DATA/SAVE/SAVEDATA3.dat",
			key
		);
	}

	//セーブ画面(キー操作)
	void SAVEDATA_KEY_MOVE(const KeyState& key) {

		if (key.down()) {
			SAVE_y = (SAVE_y == (save_buttom_y)) ? save_base_pos_y : SAVE_y + save_move_unit;
		}

		if (key.up()) {
			SAVE_y = (save_base_pos_y == SAVE_y) ? save_buttom_y : SAVE_y - save_move_unit;
		}
	}

	//セーブロードメニューカーソル関数
	void SAVE_LOAD_MENU(unsigned int color, int y) {
		//カーソルの描画
		DrawString(save_base_pos_x, y, "■", color);
	}

	//セーブデータ・セーブ画面ループ
	void SAVEDATA_SAVE_LOOP(KeyState& key) {

		//セーブデータ・セーブ画面ループ
		using clock = std::chrono::high_resolution_clock;
		using namespace std::chrono_literals;
		auto normal_con_f = []() -> bool {
			return -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen();
		};
		scoped_screen screen(DX_SCREEN_BACK);
		for (auto t = clock::now(); normal_con_f() && key.wait_key_change(t + 300ms) && false == GAMEMENU_COUNT; t = clock::now()) {
			//背景描画
			DrawGraph(0, 0, SAVETITLE, TRUE);

			//カーソル描画
			SAVE_LOAD_MENU(Cr, SAVE_y);

			//セーブ画面描画
			SAVEDATA_DRAW();

			//マウス操作
			Mouse_Move();

			//キー操作関連
			SAVEDATA_KEY_MOVE(key);

			//画面クリア処理
			//SCREEN_CLEAR();

			//セーブデータ１にセーブ
			if (SAVE_y == save_base_pos_y && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ１にセーブ
				SAVEDATA_1_SAVE(key);
			}

			//セーブデータ２にセーブ
			if (SAVE_y == (save_base_pos_y * 2) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ２にセーブ
				SAVEDATA_2_SAVE(key);
			}

			//セーブデータ３にセーブ
			if (SAVE_y == (save_base_pos_y * 3) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ３にセーブ
				SAVEDATA_3_SAVE(key);
			}

			//画面に戻る
			if (SAVE_y == (save_base_pos_y * 4) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				if (IDYES == MessageBoxYesNo("戻りますか？", key, KeyState::Executor::flush_update)) {
					ClearDrawScreen();
					//ショートカットキー時の事後処理
					SHORTCUT_KEY_DRAW();
					break;
				}
			}
		}
	}
}
//セーブデータセーブ関数
void SAVEDATA_SAVE(KeyState& key) {
	if (IDYES == MessageBoxYesNo("セーブ画面に移行しますか？", key, KeyState::Executor::flush, KeyState::Executor::flush_update)) {
		ClearDrawScreen();
		SAVE_y = save_base_pos_y;

		//セーブデータのスクリーンショットの読み込み
		SAVEDATA_SCREENSHOT_READ();

		//セーブデータ・セーブ画面ループ
		SAVEDATA_SAVE_LOOP(key);
	}
}

namespace {
	//ロード後のメッセージ	
	template<typename ExectorType>
	void LOAD_MESSAGE(KeyState& key, ExectorType t) {
		MessageBoxOk("ロードしました！", key, t);
	}

	//ロード後の処理(サウンドノベル風)
	void LOAD_SOUNDNOVEL() {

		//サウンドノベル風描画時の処理
		SOUNDNOVEL();

		GAMEMENU_COUNT = true;
	}

	//ロード後の処理(ウインドウ風)
	void LOAD_WINDOWNOVEL() {

		//ウインドウ風描画時の処理
		WINDOWNOVEL();

		GAMEMENU_COUNT = true;
	}

	static int LoadSaveData(const char* Message, const char* ErrorMessage, const char* SaveDataPath, KeyState& key) {
		//TODO: このif文は呼び出し元のループに持っていく
		if (IDYES == MessageBoxYesNo(Message, key, KeyState::Executor::none, KeyState::Executor::flush_update)) {
			SaveData_t Data;
			FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
			const errno_t er = fopen_s(&fp, SaveDataPath, "rb");
			if (0 != er || nullptr == fp) {
				MessageBoxOk(ErrorMessage, key, KeyState::Executor::flush_update);
				return 0;
			}
#else
			fp = fopen(SaveDataPath, "rb");
			if (fp == nullptr) {
				MessageBoxOk(ErrorMessage, key, KeyState::Executor::flush_update);
				return 0;
			}
#endif
			fread(&Data, sizeof(Data), 1, fp);
			fclose(fp);
			EndFlag = Data.ENDFLAG;
			SP = Data.SP;
			CP = Data.CP;
			charactor.activeResource(Data.CHAR);
			background.activeResource(Data.BG);
			backgroundMusic.activeResource(Data.BGM);
			SAVE_CHOICE = Data.SAVE_CHOICE;

			//ロード後のメッセージ
			LOAD_MESSAGE(key, KeyState::Executor::flush_update);
			//ロード後の処理(サウンドノベル風)
			LOAD_SOUNDNOVEL();
			//ロード後の処理(ウインドウ風)
			LOAD_WINDOWNOVEL();
		}
		return 0;
	}
	//セーブデータ1のロード
	int SAVEDATA_1_LOAD(KeyState& key) {
		return LoadSaveData("セーブデータ1をロードしますか？", "セーブデータ1がありません！", "DATA/SAVE/SAVEDATA1.dat", key);
	}

	//セーブデータ2のロード
	int SAVEDATA_2_LOAD(KeyState& key) {
		return LoadSaveData("セーブデータ2をロードしますか？", "セーブデータ2がありません！", "DATA/SAVE/SAVEDATA2.dat", key);
	}

	//セーブデータ3をロード
	int SAVEDATA_3_LOAD(KeyState& key) {
		return LoadSaveData("セーブデータ3をロードしますか？", "セーブデータ3がありません！", "DATA/SAVE/SAVEDATA3.dat", key);
	}

	//セーブデータ・ロード画面ループ
	void SAVEDATA_LOAD_LOOP(KeyState& key) {
		using clock = std::chrono::high_resolution_clock;
		using namespace std::chrono_literals;
		auto normal_con_f = []() -> bool {
			return -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen();
		};
		scoped_screen screen(DX_SCREEN_BACK);
		for (auto t = clock::now(); normal_con_f() && key.wait_key_change(t + 300ms) && false == GAMEMENU_COUNT; t = clock::now()) {
		//scoped_screen screen(DX_SCREEN_FRONT);
		//while (ProcessMessage() == 0 && key.update() && false == GAMEMENU_COUNT) {
			//背景描画
			DrawGraph(0, 0, SAVETITLE, TRUE);

			//カーソル描画
			SAVE_LOAD_MENU(Cr, SAVE_y);

			//セーブデータ一覧描画
			SAVEDATA_DRAW();

			//マウス操作
			Mouse_Move();

			//セーブ画面(キー操作)
			SAVEDATA_KEY_MOVE(key);

			//画面クリア処理
			SCREEN_CLEAR();

			//セーブデータ１のロード
			if (SAVE_y == save_base_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
				//セーブデータ１をロード
				SAVEDATA_1_LOAD(key);
			}
			//セーブデータ２のロード
			if (SAVE_y == (save_base_pos_y * 2) && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
				//セーブデータ2をロード
				SAVEDATA_2_LOAD(key);
			}
			//セーブデータ３のロード
			if (SAVE_y == (save_base_pos_y * 3) && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
				//セーブデータ2をロード
				SAVEDATA_3_LOAD(key);
			}
			//戻る
			if (SAVE_y == (save_base_pos_y * 4) && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
				if (IDYES == MessageBoxYesNo("戻りますか？", key, KeyState::Executor::flush)) {
					ClearDrawScreen();
					//ショートカットキー時の事後処理
					SHORTCUT_KEY_DRAW();
					break;
				}
			}
		}
	}
}

//セーブデータロード関数
int SAVEDATA_LOAD(KeyState& key) {
	if (IDYES == MessageBoxYesNo("ロード画面に移行しますか？", key, KeyState::Executor::flush, KeyState::Executor::flush_update)) {

		ClearDrawScreen();
		SAVE_y = save_base_pos_y;

		//セーブデータのスクリーンショット読込
		SAVEDATA_SCREENSHOT_READ();

		//セーブデータ・ロード画面ループ
		SAVEDATA_LOAD_LOOP(key);
	}
	return 0;
}

namespace {
	//削除後のメッセージ
	template<typename ExectorType>
	static void DELETE_MESSAGE(KeyState& key, ExectorType t) {
		MessageBoxOk("削除しました！", key, t);
	}

	//削除後の処理(サウンドノベル風)
	static void DELETE_SOUNDNOVEL() {

		//サウンドノベル風描画時の処理
		SOUNDNOVEL();

		GAMEMENU_COUNT = true;
	}

	//削除後の処理(ウインドウ風)
	static void DELETE_WINDOWNOVEL() {

		//削除後の処理(ウインドウ風)
		WINDOWNOVEL();

		GAMEMENU_COUNT = true;
	}

	static void DeleteSaveData(const char* Message, const char* ImagePath, const char* SaveDataPath, KeyState& key) {
		if (IDYES == MessageBoxYesNo(Message, key, KeyState::Executor::none, KeyState::Executor::flush_update)) {
			remove(SaveDataPath);
			remove(ImagePath);
			//削除後のメッセージ
			DELETE_MESSAGE(key, KeyState::Executor::flush_update);
			//削除後の処理(サウンドノベル風)
			DELETE_SOUNDNOVEL();
			//削除後の処理(ウインドウ風)
			DELETE_WINDOWNOVEL();
		}
	}

	//セーブデータ1削除
	void SAVEDATA_1_DELETE(KeyState& key) {
		DeleteSaveData("セーブデータ1を削除しますか？", "DATA/SAVE/SAVESNAP1.png", "DATA/SAVE/SAVEDATA1.dat", key);
	}

	//セーブデータ2削除
	void SAVEDATA_2_DELETE(KeyState& key) {
		DeleteSaveData("セーブデータ2を削除しますか？", "DATA/SAVE/SAVESNAP2.png", "DATA/SAVE/SAVEDATA2.dat", key);
	}

	//セーブデータ3削除
	void SAVEDATA_3_DELETE(KeyState& key) {
		DeleteSaveData("セーブデータ3を削除しますか？", "DATA/SAVE/SAVESNAP3.png", "DATA/SAVE/SAVEDATA3.dat", key);
	}

	//セーブデータ削除画面ループ
	void SAVEDATA_DELETE_LOOP(KeyState& key) {

		using clock = std::chrono::high_resolution_clock;
		using namespace std::chrono_literals;
		auto normal_con_f = []() -> bool {
			return -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen();
		};
		scoped_screen screen(DX_SCREEN_BACK);
		for (auto t = clock::now(); normal_con_f() && key.wait_key_change(t + 300ms) && false == GAMEMENU_COUNT; t = clock::now()) {
			//while (ProcessMessage() == 0 && key.update() && false == GAMEMENU_COUNT) {

			//背景描画
			DrawGraph(0, 0, SAVETITLE, TRUE);

			//カーソル描画
			SAVE_LOAD_MENU(Cr, SAVE_y);

			//セーブデータ一覧描画
			SAVEDATA_DRAW();

			//マウス操作
			Mouse_Move();

			//キー操作関連
			SAVEDATA_KEY_MOVE(key);

			//画面クリア処理
			SCREEN_CLEAR();

			if (SAVE_y == save_base_pos_y && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ1削除処理
				SAVEDATA_1_DELETE(key);
			}
			if (SAVE_y == (save_base_pos_y * 2) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ2削除処理
				SAVEDATA_2_DELETE(key);
			}
			if (SAVE_y == (save_base_pos_y * 3) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				//セーブデータ3削除処理
				SAVEDATA_3_DELETE(key);
			}
			if (SAVE_y == (save_base_pos_y * 4) && (key.enter() || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
				const auto choice = MessageBoxYesNo("戻りますか？", key, KeyState::Executor::flush);
				t = clock::now();
				if (IDYES == choice) {
					ClearDrawScreen();
					//ショートカットキー時の事後処理
					SHORTCUT_KEY_DRAW();
					//キー判定消去待ち目的ではない
					std::this_thread::sleep_until(t + 300ms);
					break;
				}
			}
		}
	}
}

//セーブデータ削除処理
void SAVEDATA_DELETE(KeyState& key) {
	if (IDYES == MessageBoxYesNo("セーブデータ削除画面に移行しますか？", key, KeyState::Executor::flush, KeyState::Executor::flush_update)) {

		ClearDrawScreen();
		SAVE_y = save_base_pos_y;

		//セーブ時のスクリーンショット読込
		SAVEDATA_SCREENSHOT_READ();

		//セーブデータ削除画面ループ
		SAVEDATA_DELETE_LOOP(key);
	}
}


//セーブデータ用スクリーンショット保存
void SAVESNAP() {

	//セーブデータ用スクリーンショット保存
	if (SAVESNAP_HANDLE1 == 1) {
		SaveDrawScreenToPNG(0, 0, 640, 480, "DATA/SAVE/SAVESNAP1.png", 0);
		SAVESNAP_HANDLE1 = 0;
	}

	if (SAVESNAP_HANDLE2 == 1) {
		SaveDrawScreenToPNG(0, 0, 640, 480, "DATA/SAVE/SAVESNAP2.png", 0);
		SAVESNAP_HANDLE2 = 0;
	}

	if (SAVESNAP_HANDLE3 == 1) {
		SaveDrawScreenToPNG(0, 0, 640, 480, "DATA/SAVE/SAVESNAP3.png", 0);
		SAVESNAP_HANDLE3 = 0;
	}
}

//セーブデータ用スクリーンショット取得(選択肢画面)
void SCRIPT_OUTPUT_CHOICE_LOOP_SAVESNAP() {

	if (SAVESNAP_CHOICE == 1) {

		SaveDrawScreenToPNG(0, 0, 640, 480, "DATA/SAVE/SAVESNAP_CHOICE.png", 0);

		SAVESNAP_CHOICE = LoadGraph("DATA/SAVE/SAVESNAP_CHOICE.png", 0);
	}
}

//マウス操作(セーブ画面関連)
void Mouse_Move_SAVE(int MouseY) {

	if (false == GAMEMENU_COUNT || EndFlag == 99) {
		SAVE_y = (MouseY <= 199) ? 100
			: (MouseY <= 299) ? 200
			: (MouseY <= 399) ? 300
			: 400;
	}
}
