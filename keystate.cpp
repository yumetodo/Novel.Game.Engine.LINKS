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
#include <thread>

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

bool KeyState::flush_update() noexcept
{
	return this->flush() && this->update();
}

bool KeyState::flush_update(default_time_point wait) noexcept {
	if(!this->flush_stresam(wait) || !this->update()) return false;
	std::this_thread::sleep_until(wait);
	return true;
}

namespace {
	template<std::size_t size>
	bool diff_keystatebuf(const char* arr1, const char* arr2) {
		size_t i;
		for (i = 0; i < size && !arr1[i] && !arr2[i]; ++i);
		return (i == size);
	}
}
bool KeyState::flush() noexcept {
	using namespace std::chrono_literals;
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->keystatebuf.size(); ++i) buf[0][i] = 0 != this->keystatebuf[i];
	this->keystatebuf.fill(0);
	char* first_p;
	char* last_p;
	for (first_p = buf[0], last_p = buf[1]; 0 == ProcessMessage() && 0 == DxLib::GetHitKeyStateAll(last_p); std::swap(first_p, last_p)) {
		if (diff_keystatebuf<keybufsize>(first_p, last_p)) return true;
		std::this_thread::sleep_for(2ms);
	}
	return false;
}

bool KeyState::flush(default_time_point wait) noexcept {
	if (this->flush_stresam(wait)) return false;
	std::this_thread::sleep_until(wait);
}

bool KeyState::flush_stresam(const default_time_point timeout) noexcept
{
	using namespace std::chrono_literals;
	using clock = default_clock;
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->keystatebuf.size(); ++i) buf[0][i] = 0 != this->keystatebuf[i];
	this->keystatebuf.fill(0);
	char* first_p;
	char* last_p;
	for (
		first_p = buf[0], last_p = buf[1];
		0 == ProcessMessage() && 0 == DxLib::GetHitKeyStateAll(last_p);
		std::swap(first_p, last_p)
	) {
		if (diff_keystatebuf<keybufsize>(first_p, last_p)) return true;
		if (timeout + 2ms <= clock::now()) return true;
		std::this_thread::sleep_for(2ms);
		if (timeout <= clock::now()) return true;
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
bool KeyState::backspace() const noexcept {
	return 0 != this->keystatebuf[KEY_INPUT_BACK];;
}
bool operator!=(const KeyState& l, size_t r) {
	return 0 == l[r];
}
