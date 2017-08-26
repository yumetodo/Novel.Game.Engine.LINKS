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
namespace legacy {
	int MessageBoxYesNo(LPCTSTR lpText) noexcept;
	int MessageBoxOk(LPCTSTR lpText) noexcept;
}
template<typename ExectorType>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, ExectorType) noexcept {
	const auto re = legacy::MessageBoxYesNo(lpText);
	KeyState::ExecutorObj<ExectorType>{key}();
	return re;
}
template<typename ExectorType, typename Rep, typename Period>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, ExectorType, std::chrono::duration<Rep, Period> flush_timeout) noexcept {
	const auto t = std::chrono::steady_clock::now();
	const auto re = legacy::MessageBoxYesNo(lpText);
	KeyState::ExecutorObj<ExectorType>{key}(t + flush_timeout);
	return re;
}
template<typename OnYesExectorType, typename OnNoExectorType>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, OnYesExectorType, OnNoExectorType) noexcept {
	const auto re = legacy::MessageBoxYesNo(lpText);
	if (IDYES == re) {
		KeyState::ExecutorObj<OnYesExectorType>{key}();
	}
	else {
		KeyState::ExecutorObj<OnNoExectorType>{key}();
	}
	return re;
}
template<typename OnYesExectorType, typename OnNoExectorType, typename Rep, typename Period>
int MessageBoxYesNo(LPCTSTR lpText, KeyState& key, OnYesExectorType, OnNoExectorType, std::chrono::duration<Rep, Period> flush_timeout) noexcept {
	const auto t = std::chrono::steady_clock::now();
	const auto re = legacy::MessageBoxYesNo(lpText);
	if (IDYES == re) {
		KeyState::ExecutorObj<OnYesExectorType>{key}(t + flush_timeout);
	}
	else {
		KeyState::ExecutorObj<OnNoExectorType>{key}(t + flush_timeout);
	}
	return re;
}
template<typename ExectorType>
int MessageBoxOk(LPCTSTR lpText, KeyState& key, ExectorType) noexcept {
	const auto re = legacy::MessageBoxOk(lpText);
	KeyState::ExecutorObj<ExectorType>{key}();
	return re;
}
template<typename ExectorType, typename Rep, typename Period>
int MessageBoxOk(LPCTSTR lpText, KeyState& key, ExectorType, std::chrono::duration<Rep, Period> flush_timeout) noexcept {
	const auto t = std::chrono::steady_clock::now();
	const auto re = legacy::MessageBoxOk(lpText);
	KeyState::ExecutorObj<ExectorType>{key}(t + flush_timeout);
	return re;
}
//画面クリア処理関数
void SCREEN_CLEAR() noexcept;

constexpr bool isdigit(char c) noexcept { return '0' <= c && c <= '9'; }
constexpr unsigned int ctoui(char c) noexcept { return c - '0'; }
#endif
