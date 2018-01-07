#pragma once
#include <array>
#include <functional>
#include <chrono>
#include <limits>
#include <bitset>

class KeyState
{
public:
	using buf_elem_type = std::uint32_t;
	using buf_elem_limit = std::numeric_limits<buf_elem_type>;
	using default_clock = std::chrono::steady_clock;
	using default_time_point = std::chrono::time_point<default_clock>;
public:
	KeyState() = delete;
	KeyState(const std::int32_t* mouse_key_move) : mouse_key_move_(mouse_key_move), keystatebuf(), is_prev_key_left() {}
	KeyState(const KeyState&) = delete;
	KeyState(KeyState&&) = delete;
	KeyState& operator=(const KeyState&) = delete;
	KeyState& operator=(KeyState&&) = delete;
	bool update() noexcept;
private:
	void clear_buf() noexcept;
	bool flush_stresam(const default_time_point timeout) noexcept;
public:
	bool flush() noexcept;
	bool flush(default_time_point wait) noexcept;
	template<
		typename Rep, typename Period,
		std::enable_if_t<!std::is_same<std::chrono::time_point<Rep, Period>, default_time_point>{}, std::nullptr_t> = nullptr
	>
	bool flush(std::chrono::time_point<Rep, Period> wait) noexcept {
		return this->flush(std::chrono::time_point_cast<default_time_point>(wait));
	}
	bool flush_update() noexcept;
	bool flush_update(default_time_point wait) noexcept;
	template<
		typename Rep, typename Period,
		std::enable_if_t<!std::is_same<std::chrono::time_point<Rep, Period>, default_time_point>{}, std::nullptr_t> = nullptr
	>
	bool flush_update(std::chrono::time_point<Rep, Period> wait) noexcept {
		return this->flush_update(std::chrono::time_point_cast<default_time_point>(wait));
	}
	bool wait_key_change(const default_time_point wait) noexcept;
	template<
		typename Rep, typename Period,
		std::enable_if_t<!std::is_same<std::chrono::time_point<Rep, Period>, default_time_point>{}, std::nullptr_t> = nullptr
	>
	bool wait_key_change(std::chrono::time_point<Rep, Period> wait) noexcept {
		return this->wait_key_change(std::chrono::time_point_cast<default_time_point>(wait));
	}
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
	struct Executor{
		struct flush_tag {};
		struct flush_update_tag {};
		struct none_tag {};
		static constexpr flush_tag flush = {};
		static constexpr flush_update_tag flush_update = {};
		static constexpr none_tag none = {};
	};
private:
	//マウス操作とキー操作の情報 true/false
	const std::int32_t* mouse_key_move_;
	std::array<buf_elem_type, keybufsize> keystatebuf;
	std::bitset<keybufsize> is_prev_key_left;
};
template<typename ExecutorType> class KeyStateExecutorObj;
template<> class KeyStateExecutorObj<KeyState::Executor::flush_tag> {
private:
	std::reference_wrapper<::KeyState> key_;
public:
	KeyStateExecutorObj() = delete;
	KeyStateExecutorObj(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj(KeyStateExecutorObj&&) = default;
	KeyStateExecutorObj& operator=(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj& operator=(KeyStateExecutorObj&&) = default;
	KeyStateExecutorObj(::KeyState& key) : key_(key) {}
	bool operator()() {
		return this->key_.get().flush();
	}
	bool operator()(KeyState::default_time_point wait) {
		return this->key_.get().flush(wait);
	}
};
template<> class KeyStateExecutorObj<KeyState::Executor::flush_update_tag> {
private:
	std::reference_wrapper<::KeyState> key_;
public:
	KeyStateExecutorObj() = delete;
	KeyStateExecutorObj(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj(KeyStateExecutorObj&&) = default;
	KeyStateExecutorObj& operator=(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj& operator=(KeyStateExecutorObj&&) = default;
	KeyStateExecutorObj(::KeyState& key) : key_(key) {}
	bool operator()() {
		return this->key_.get().flush_update();
	}
	bool operator()(KeyState::default_time_point wait) {
		return this->key_.get().flush_update(wait);
	}
};
template<> class KeyStateExecutorObj<KeyState::Executor::none_tag> {
public:
	KeyStateExecutorObj() = delete;
	KeyStateExecutorObj(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj(KeyStateExecutorObj&&) = default;
	KeyStateExecutorObj& operator=(const KeyStateExecutorObj&) = delete;
	KeyStateExecutorObj& operator=(KeyStateExecutorObj&&) = default;
	constexpr KeyStateExecutorObj(::KeyState&) {}
	constexpr bool operator()() const { return true; }
	constexpr bool operator()(KeyState::default_time_point) { return true; }
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