#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "keystate.hpp"
#	if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
struct IUnknown;
#	endif
#include "DxLib.h"
#include <limits>
KeyState::KeyState() noexcept : keystatebuf() {}

bool KeyState::update() noexcept {
	char buf[keybufsize];
	auto re = GetHitKeyStateAll(buf);
	if (0 != re) return false;
	for (size_t i = 0; i < keybufsize; ++i) {
		if (buf[i] && std::numeric_limits<int>::max() != this->keystatebuf[i]) ++this->keystatebuf[i];
		else this->keystatebuf[i] = 0;
	}
	return true;
}

bool KeyState::flush() noexcept {
	if(this->flush_stream()) return false;
	this->keystatebuf.fill(0);
	return true;
}
bool KeyState::flush_stream() noexcept {
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

int KeyState::operator[](size_t n) const noexcept {
	return this->keystatebuf[n];
}

int KeyState::at(size_t n) const {
	return this->keystatebuf.at(n);
}
bool KeyState::shift() const noexcept {
	return this->lshift() || this->rshift();
}
bool KeyState::lshift() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LSHIFT];
}
bool KeyState::rshift() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RSHIFT];
}
bool KeyState::ctrl() const noexcept {
	return this->lctrl() || this->rctrl();
}
bool KeyState::lctrl() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LCONTROL];
}
bool KeyState::rctrl() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RCONTROL];
}
bool KeyState::esc() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_ESCAPE];
}
bool KeyState::right() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RIGHT];
}
bool KeyState::up() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_UP];
}
bool KeyState::left() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_LEFT];
}
bool KeyState::down() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_DOWN];
}
bool KeyState::enter() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_RETURN];
}
bool KeyState::space() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_SPACE];
}
bool operator!=(const KeyState& l, size_t r) {
	return 0 == l[r];
}
