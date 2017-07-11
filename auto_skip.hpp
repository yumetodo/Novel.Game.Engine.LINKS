/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#ifndef LINKS_AUTO_SKIP_HPP_
#define LINKS_AUTO_SKIP_HPP_
#include "keystate.hpp"

void disableSkip() noexcept;
void SKIP_READ_CHECK(KeyState& key) noexcept;
void SKIP_START(KeyState& key) noexcept;
void AUTO_START(KeyState& key) noexcept;
void AUTO_SKIP_STOP(KeyState& key) noexcept;
void SCRIPT_UTPUT_KEYWAIT(const KeyState& key) noexcept;
void SCRIPT_OUTPUT_WAIT() noexcept;
void SCRIPT_OUTPUT_STRING_DRAW_SPEED() noexcept;
#endif
