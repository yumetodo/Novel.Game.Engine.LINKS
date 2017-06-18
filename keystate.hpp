#pragma once
#include <array>

class KeyState
{
public:
	KeyState() noexcept;
	KeyState(const KeyState&) = delete;
	KeyState(KeyState&&) = delete;
	KeyState& operator=(const KeyState&) = delete;
	KeyState& operator=(KeyState&&) = delete;
	bool update() noexcept;
	bool flush() noexcept;
	bool flush_update() noexcept;
	int operator[](std::size_t n) const noexcept;
	int at(std::size_t n) const;
	bool shift() const noexcept;
	bool lshift() const noexcept;
	bool rshift() const noexcept;
	bool ctrl() const noexcept;
	bool lctrl() const noexcept;
	bool rctrl() const noexcept;
	bool esc() const noexcept;
	bool right() const noexcept;
	bool up() const noexcept;
	bool left() const noexcept;
	bool down() const noexcept;
	bool enter() const noexcept;
	bool space() const noexcept;
	bool backspace() const noexcept;
	static constexpr std::size_t keybufsize = 256;
private:
	bool flush_stream() noexcept;
	std::array<int, 256> keystatebuf;
};
bool operator!=(const KeyState& l, std::size_t r);
inline bool operator!=(std::size_t l, const KeyState& r) {
	return r != l;
}
inline bool operator==(const KeyState& l, std::size_t r) {
	return !(l != r);
}
inline bool operator==(std::size_t l, const KeyState& r) {
	return !(l != r);
}