#pragma once

// a collection of useful funtions that i use across projects

#include <algorithm>
#include <tuple>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "Random.h"

template<typename T, typename U, typename V>
T getRandomNumber(U min, V max)
{
	return static_cast<T>(Random::get(min, max));
}

template<typename T, typename U, typename V, typename W>
inline sf::Color createColor(T r, U g, V b, W a)
{
	return sf::Color(static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(a));
}

template<typename T, typename U, typename V>
inline sf::Color createColor(T r, U g, V b, std::uint8_t a = 255u)
{
	return sf::Color(static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), a);
}

sf::Color getRandomColor()
{
	return sf::Color(getRandomNumber<std::uint8_t>(0u, 255u), getRandomNumber<std::uint8_t>(0u, 255u), getRandomNumber<std::uint8_t>(0u, 255u));
}

template<typename T>
T signage(T in)
{
	T zero{};
	if (in > zero) return ++zero;
	if (in < zero) return --zero;
	return zero;
}