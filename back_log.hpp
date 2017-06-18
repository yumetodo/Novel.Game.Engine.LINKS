﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Novel.Game.Engine.LINKS
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#ifndef LINKS_BACK_LOG_HPP_
#define LINKS_BACK_LOG_HPP_
#include "keystate.hpp"

void BACKLOG_DRAW(KeyState& key) noexcept;
void BACKLOG_GET() noexcept;
void incrementBackLogCount() noexcept;

#endif
