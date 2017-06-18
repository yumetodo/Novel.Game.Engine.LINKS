﻿#include "keystate.hpp"
#	if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
#		define CINTERFACE
#	endif
#include "DxLib.h"
#include <limits>
#ifdef max
#undef max
#endif
keystate::keystate() noexcept : keystatebuf() {
	this->fllush_stream();
}

bool keystate::update() noexcept {
	char buf[keybufsize];
	auto re = GetHitKeyStateAll(buf);
	if (0 != re) return false;
	for (size_t i = 0; i < keybufsize; ++i) {
		if (buf[i] && std::numeric_limits<int>::max() != this->keystatebuf[i]) ++this->keystatebuf[i];
		else this->keystatebuf[i] = 0;
	}
	return true;
}

bool keystate::fllush() noexcept {
	if(this->fllush_stream()) return false;
	this->keystatebuf.fill(0);
	return true;
}
bool keystate::fllush_stream() noexcept {
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->keystatebuf.size(); ++i) buf[0][i] = 0 != this->keystatebuf[i];
	char* first_p;
	char* last_p;
	size_t i;
	for (first_p = buf[0], last_p = buf[1]; 0 == DxLib::GetHitKeyStateAll(last_p); std::swap(first_p, last_p)) {
		for (i = 0; i < keybufsize && !first_p[i] && !last_p[i]; ++i);
		if (i == keybufsize) return true;
	}
	return false;
}

int keystate::operator[](size_t n) const noexcept {
	return this->keystatebuf[n];
}

int keystate::at(size_t n) const {
	return this->keystatebuf.at(n);
}
bool keystate::shift() const noexcept {
	return this->lshift() || this->rshift();
}
bool keystate::lshift() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LSHIFT];
}
bool keystate::rshift() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RSHIFT];
}
bool keystate::ctrl() const noexcept {
	return this->lctrl() || this->rctrl();
}
bool keystate::lctrl() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LCONTROL];
}
bool keystate::rctrl() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RCONTROL];
}
bool keystate::esc() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_ESCAPE];
}
bool keystate::right() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RIGHT];
}
bool keystate::up() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_UP];
}
bool keystate::left() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LEFT];
}
bool keystate::down() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_DOWN];
}
bool keystate::enter() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RETURN];
}
bool keystate::space() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_SPACE];
}
bool operator!=(const keystate& l, size_t r) {
	return 0 == l[r];
}
