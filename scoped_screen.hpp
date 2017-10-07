#include <DxLib.h>
class scoped_screen {
private:
	int pre_screen_;
public:
	scoped_screen() = delete;
	scoped_screen(const scoped_screen&) = delete;
	scoped_screen(scoped_screen&&) = delete;
	scoped_screen& operator=(const scoped_screen&) = delete;
	scoped_screen& operator=(scoped_screen&&) = delete;
	explicit scoped_screen(int draw_screen) noexcept : pre_screen_(DxLib::GetDrawScreen()) {
		DxLib::SetDrawScreen(draw_screen);
	}
	~scoped_screen() noexcept {
		DxLib::SetDrawScreen(this->pre_screen_);
	}
};