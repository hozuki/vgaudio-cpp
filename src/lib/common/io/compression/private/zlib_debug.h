#pragma once

void ThrowDefaultZLibError(const char *stage, const char *description, int ret) noexcept(false);

void ThrowCommonZLibError(const char *stage, int ret, bool ignoreBufError = false) noexcept(false);
