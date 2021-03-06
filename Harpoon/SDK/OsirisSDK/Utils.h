#pragma once

#include <cmath>
#include <tuple>
#include <stdint.h>

#define M_PI 3.14

constexpr auto degreesToRadians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(M_PI) / 180.0f; };
constexpr auto radiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / static_cast<float>(M_PI); };

std::tuple<float, float, float> rainbowColor(float speed) noexcept;
uint64_t FindSignature(const char* szModule, const char* szSignature) noexcept;
