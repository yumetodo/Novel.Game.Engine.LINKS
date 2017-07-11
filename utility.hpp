/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#ifndef LINKS_UTILITY_HPP_
#define LINKS_UTILITY_HPP_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cassert>
#include "keystate.hpp"

#ifdef NDEBUG
#	define LINKS_EXPECT_TRUE(expression) expression
#else
#	define LINKS_EXPECT_TRUE(expression) assert(expression)
#endif

int MessageBoxYesNo(LPCTSTR lpText) noexcept;
int MessageBoxOk(LPCTSTR lpText) noexcept;
template<typename ExectorType>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, ExectorType) noexcept {
#pragma warning(push)
#pragma warning(disable: 4996)
	const auto re = MessageBoxYesNo(lpText);
#pragma warning(pop)
	KeyState::ExecutorObj<ExectorType>{key}();
	return re;
}
template<typename OnYesExectorType, typename OnNoExectorType>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, OnYesExectorType, OnNoExectorType) noexcept {
#pragma warning(push)
#pragma warning(disable: 4996)
	const auto re = MessageBoxYesNo(lpText);
#pragma warning(pop)
	if (IDYES == re) {
		KeyState::ExecutorObj<OnYesExectorType>{key}();
	}
	else {
		KeyState::ExecutorObj<OnNoExectorType>{key}();
	}
	return re;
}
template<typename ExectorType>
int MessageBoxOk(LPCTSTR lpText, KeyState& key, ExectorType) noexcept {
#pragma warning(push)
#pragma warning(disable: 4996)
	const auto re = MessageBoxOk(lpText);
#pragma warning(pop)
	KeyState::ExecutorObj<ExectorType>{key}();
	return re;
}
//画面クリア処理関数
void SCREEN_CLEAR() noexcept;

constexpr bool isdigit(char c) noexcept { return '0' <= c && c <= '9'; }
constexpr unsigned int ctoui(char c) noexcept { return c - '0'; }
#endif
