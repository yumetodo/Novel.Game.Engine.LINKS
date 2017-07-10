/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

#include "DxLib.h"
#include "stdio.h"
#include "GAME.h"
#include "DEF.h"
#include "back_log.hpp"
#include "resource.h"
#include "save.hpp"
#include "auto_skip.hpp"
#include "keystate.hpp"

//DXライブラリ初期化前処理
void DXLib_PREP() {

	//ログの抽出
	SetOutApplicationLogValidFlag(FALSE);

	//アイコン
	SetWindowIconID(101);

	//ウインドウに表示されるゲームソフト名
	SetWindowText("ノベルゲームエンジン「LINKS」");

	//ウインドウを自由に変更できる
	SetWindowSizeChangeEnableFlag(TRUE, TRUE);

	//ウインドウ描画の種類
	SetDrawMode(DX_DRAWMODE_NEAREST);

	//ウインドウの種類
	SetWindowStyleMode(7);

	//初期ウインドウサイズ
	SetWindowSize(640, 480);

	//ウインドウモードで起動
	ChangeWindowMode(TRUE);
}

//DXライブラリ初期化後処理
void DXLib_POST_PREP() {

	//マウス描画設定
	SetMouseDispFlag(TRUE);

	//DXアーカイブ機能の使用
	SetUseDXArchiveFlag(TRUE);

	//各素材データ読込
	MATERIAL_LOAD();

	// フォントのサイズセット
	SetFontSize(font_size);

	//フォントの設定
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_8X8);

	//ゲームリンクスロゴ表示
	PlayMovie("DATA/MOVIE/OP.wmv", 1, DX_MOVIEPLAYTYPE_BCANCEL);

	//OP画面の削除
	ClearDrawScreen();
	WaitTimer(100);

	//設定の読込
	CONFIG_LOAD();
}

//タイトルメニュー(キー操作)
void TITLE_MENU_KEY_MOVE(const KeyState& key) {

	if (key.down()) {
		TitleMenuPosY = (title_menu_game_quit_pos_y == TitleMenuPosY) ? title_menu_game_start_pos_y : TitleMenuPosY + cursor_move_unit;
	}

	if (key.up()) {
		TitleMenuPosY = (title_menu_game_start_pos_y == TitleMenuPosY) ? title_menu_game_quit_pos_y : TitleMenuPosY - cursor_move_unit;
	}
}

//タイトルメニュー終了処理
void TITLE_MENU_END() {
	if (IDYES == MessageBoxYesNo("終了しますか？")) {
		EndFlag = 99999;
	}
	WaitTimer(300);
}

//タイトルメニュー(選択処理)
void TITLE_MENU_CHOICE(KeyState& key) {
	if (TitleMenuPosY == title_menu_game_start_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		EndFlag = 1;
		key.flush_update();
	}
	if (TitleMenuPosY == title_menu_game_load_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		SAVEDATA_LOAD(key);
		key.flush_update();
	}
	if (TitleMenuPosY == title_menu_game_config_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		CONFIG(key);
		key.flush_update();
	}
	if (TitleMenuPosY == title_menu_quick_load_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		QUICKSAVE_LOAD();
		key.flush_update();
	}
	if (TitleMenuPosY == title_menu_continue_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		CONTINUE_LOAD();
		key.flush_update();
	}
	if (TitleMenuPosY == title_menu_game_quit_pos_y && (key.enter() || ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0))) {
		TITLE_MENU_END();
		key.flush_update();
	}
}

//タイトルメニュー関数
void TITLE_MENU(KeyState& key) {

	//タイトルメニュー
	if (EndFlag == 99) {

		while (ProcessMessage() == 0 && key.update() && EndFlag == 99) {

			//タイトル表示
			DrawGraph(0, 0, TITLE, TRUE);

			//タイトルメニュー表示
			title(Cr, TitleMenuPosY);

			//スクリーンショット機能
			SCREENSHOT(key);

			//マウス操作
			Mouse_Move();

			//キー操作関連
			TITLE_MENU_KEY_MOVE(key);

			//画面クリア処理
			SCREEN_CLEAR();

			//タイトルメニュー(選択処理)
			TITLE_MENU_CHOICE(key);

			//終了ウインドウ
			GAME_FINISH(key);
		}
	}
}

static void GameLoopType1(const int RouteNumber, int32_t& TextIgnoredFlag, KeyState& key){
	if (TextIgnoredFlag == 0) disableSkip();
	SCRIPT_READ();
	//Ａルートループ
	while (ProcessMessage() == 0 && key.update())
	{
		//タグ処理
		SCRIPT_OUTPUT(key);
		//ゲームメニュー
		GAMEMENU(key);
		//スクリーンショット取得
		SCREENSHOT(key);
		//ショートカットキー
		SHORTCUT_KEY(key);
		//終了
		GAME_FINISH(key);
		// 終了フラグが2でなかったら終了する
		if (EndFlag != RouteNumber && EndFlag != 99999) {
			if (SAVE_CHOICE == 0) FORMAT();
			TextIgnoredFlag = 1;
			SKIP_READ_SAVE();
			break;
		}
		if (EndFlag == 99999) break;
		//参照文字列処理
		WORD_FORMAT();
	}
}

static void GameLoopType2(const int RouteNumber, const int32_t TextIgnoredFlag, KeyState& key){
	if (TextIgnoredFlag == 0) disableSkip();
	SCRIPT_READ();
	while (ProcessMessage() == 0 && key.update())
	{
		//タグ処理
		SCRIPT_OUTPUT(key);
		//ゲームメニュー
		GAMEMENU(key);
		//スクリーンショット取得
		SCREENSHOT(key);
		//ショートカットキー
		SHORTCUT_KEY(key);
		//終了
		GAME_FINISH(key);
		if (EndFlag != RouteNumber) {
			if (SAVE_CHOICE == 0) FORMAT();
			break;
		}
		//参照文字列処理
		WORD_FORMAT();
	}
}

//ゲームのループ
void GAME_LOOP(KeyState& key) {
	if (EndFlag < 1 || 15 < EndFlag) return;
	SkipDataConv* conv = reinterpret_cast<SkipDataConv*>(&TextIgnoredFlags);
	if (/*1 <= EndFlag && */EndFlag <= 7) {
		//main, A-F
		GameLoopType1(EndFlag, conv->arr[EndFlag - 1], key);
	}
	else /*if(8 <= EndFlag && EndFlag <= 15)*/ {
		//G-N
		GameLoopType2(EndFlag, conv->arr[EndFlag - 1], key);
	}
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	//DXライブラリ初期化前処理
	DXLib_PREP();

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		// エラーが起きたら直ちに終了
		return -1;
	}

	//DXライブラリ初期化後処理
	DXLib_POST_PREP();
	KeyState keystate = {};

	while (ProcessMessage() == 0 && EndFlag != 99999) {

		//タイトルメニュー
		TITLE_MENU(keystate);

		//各種ゲームループ
		GAME_LOOP(keystate);
	}

	//設定の保存
	CONFIG_SAVE();

	//バックログ画像の削除
	remove("DATA/BACKLOG/BACKLOG1.png");

	//選択肢セーブスナップ削除
	remove("DATA/SAVE/SAVESNAP_CHOICE.png");

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;					// ソフトの終了
}
