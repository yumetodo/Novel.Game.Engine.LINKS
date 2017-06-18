#pragma once
#include <array>

class keystate
{
public:
	keystate() noexcept;
	keystate(const keystate&) = delete;
	keystate(keystate&&) = delete;
	keystate& operator=(const keystate&) = delete;
	keystate& operator=(keystate&&) = delete;
	bool update() noexcept;
	bool fllush() noexcept;
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
	static constexpr std::size_t keybufsize = 256;
private:
	bool fllush_stream() noexcept;
	std::array<int, 256> keystatebuf;
};
bool operator!=(const keystate& l, std::size_t r);
inline bool operator!=(std::size_t l, const keystate& r) {
	return r != l;
}
inline bool operator==(const keystate& l, std::size_t r) {
	return !(l != r);
}
inline bool operator==(std::size_t l, const keystate& r) {
	return !(l != r);
}