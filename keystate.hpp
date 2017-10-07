#pragma once
#include <array>
#include <functional>
#include <chrono>

class KeyState
{
public:
	using default_clock = std::chrono::steady_clock;
private:
	using default_time_point = std::chrono::time_point<default_clock>;
public:
	KeyState() = default;
	KeyState(const KeyState&) = delete;
	KeyState(KeyState&&) = delete;
	KeyState& operator=(const KeyState&) = delete;
	KeyState& operator=(KeyState&&) = delete;
	bool update() noexcept;
private:
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
	template<typename ExecutorType> struct ExecutorObj;
	template<> class ExecutorObj<Executor::flush_tag> {
	private:
		std::reference_wrapper<::KeyState> key_;
	public:
		ExecutorObj() = delete;
		ExecutorObj(const ExecutorObj&) = delete;
		ExecutorObj(ExecutorObj&&) = default;
		ExecutorObj& operator=(const ExecutorObj&) = delete;
		ExecutorObj& operator=(ExecutorObj&&) = default;
		ExecutorObj(::KeyState& key) : key_(key) {}
		bool operator()() {
			return this->key_.get().flush();
		}
		bool operator()(default_time_point wait) {
			return this->key_.get().flush(wait);
		}
	};
	template<> class ExecutorObj<Executor::flush_update_tag> {
	private:
		std::reference_wrapper<::KeyState> key_;
	public:
		ExecutorObj() = delete;
		ExecutorObj(const ExecutorObj&) = delete;
		ExecutorObj(ExecutorObj&&) = default;
		ExecutorObj& operator=(const ExecutorObj&) = delete;
		ExecutorObj& operator=(ExecutorObj&&) = default;
		ExecutorObj(::KeyState& key) : key_(key) {}
		bool operator()() {
			return this->key_.get().flush_update();
		}
		bool operator()(default_time_point wait) {
			return this->key_.get().flush_update(wait);
		}
	};
	template<> class ExecutorObj<Executor::none_tag> {
	public:
		ExecutorObj() = delete;
		ExecutorObj(const ExecutorObj&) = delete;
		ExecutorObj(ExecutorObj&&) = default;
		ExecutorObj& operator=(const ExecutorObj&) = delete;
		ExecutorObj& operator=(ExecutorObj&&) = default;
		constexpr ExecutorObj(::KeyState&) {}
		constexpr bool operator()() const { return true; }
		constexpr bool operator()(default_time_point) { return true; }
	};
private:
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