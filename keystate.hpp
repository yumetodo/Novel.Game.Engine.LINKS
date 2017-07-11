#pragma once
#include <array>
#include <functional>

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
	};
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