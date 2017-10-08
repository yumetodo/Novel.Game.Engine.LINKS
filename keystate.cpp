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
#include <type_traits>

namespace {
	template<
		typename OnPushdFunc, typename OnNotPushdFunc,
		std::enable_if_t<
			std::is_same<KeyState::buf_elem_type, decltype(std::declval<OnPushdFunc>()(std::declval<KeyState::buf_elem_type>()))>::value
			&& std::is_same<KeyState::buf_elem_type, decltype(std::declval<OnNotPushdFunc>()(std::declval<KeyState::buf_elem_type>()))>::value,
			std::nullptr_t
		> = nullptr
	>
	bool update_keystatebuf(std::array<KeyState::buf_elem_type, KeyState::keybufsize>& keystatebuf, OnPushdFunc&& on_pushd, OnNotPushdFunc&& on_not_pushd) {
		char buf[KeyState::keybufsize];
		auto re = GetHitKeyStateAll(buf);
		if (0 != re) return false;
		for (size_t i = 0; i < KeyState::keybufsize; ++i) {
			if (buf[i]) keystatebuf[i] = on_pushd(keystatebuf[i]);
			else keystatebuf[i] = on_not_pushd(keystatebuf[i]);
		}
		return true;
	}
}

bool KeyState::update() noexcept {
	return (1 == *this->mouse_key_move_) ? true : update_keystatebuf(
		this->keystatebuf,
		[](KeyState::buf_elem_type n) { return (KeyState::buf_elem_limit::max() != n) ? n + 1 : n; },
		[](KeyState::buf_elem_type) -> KeyState::buf_elem_type { return 0; }
	);
}

bool KeyState::flush_update() noexcept
{
	return (1 == *this->mouse_key_move_) ? true : this->flush() && this->update();
}

bool KeyState::flush_update(default_time_point wait) noexcept {
	if (1 == *this->mouse_key_move_) return true;
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
	if (1 == *this->mouse_key_move_) return true;
	using namespace std::chrono_literals;
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->keystatebuf.size(); ++i) buf[0][i] = 0 != this->keystatebuf[i];
	this->clear_buf();
	char* first_p;
	char* last_p;
	for (first_p = buf[0], last_p = buf[1]; 0 == ProcessMessage() && 0 == DxLib::GetHitKeyStateAll(last_p); std::swap(first_p, last_p)) {
		if (diff_keystatebuf<keybufsize>(first_p, last_p)) return true;
		std::this_thread::sleep_for(2ms);
	}
	return false;
}

bool KeyState::flush(default_time_point wait) noexcept {
	if (1 == *this->mouse_key_move_) return true;
	if (this->flush_stresam(wait)) return false;
	std::this_thread::sleep_until(wait);
	return true;
}

void KeyState::clear_buf() noexcept
{
	this->keystatebuf.fill(0);
}

bool KeyState::flush_stresam(const default_time_point timeout) noexcept
{
	using namespace std::chrono_literals;
	using clock = default_clock;
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->keystatebuf.size(); ++i) buf[0][i] = 0 != this->keystatebuf[i];
	this->clear_buf();
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

bool KeyState::wait_key_change(const default_time_point wait) noexcept
{
	if (1 == *this->mouse_key_move_) return true;
	using clock = default_clock;
	this->clear_buf();
	bool return_not_by_time_check = true;
	while(true == (return_not_by_time_check = (clock::now() < wait)) && update_keystatebuf(
		this->keystatebuf,
		[](KeyState::buf_elem_type n) { return std::max<KeyState::buf_elem_type>(1, n); },
		[](KeyState::buf_elem_type n) { return n; }
	));
	return !return_not_by_time_check;
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
