#pragma once

#include <algorithm>
#include <tuple>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "Random.h"
#include "Ray.h"

template<typename T, typename U, typename V>
inline T getRandomNumber(U min, V max)
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

inline sf::Color getRandomColor()
{
	return sf::Color(getRandomNumber<std::uint8_t>(0u, 255u), getRandomNumber<std::uint8_t>(0u, 255u), getRandomNumber<std::uint8_t>(0u, 255u));
}

template<typename T>
inline T signage(T in)
{
	T zero{};
	if (in > zero) return ++zero;
	if (in < zero) return --zero;
	return zero;
}

struct LineSegment
{
  sf::Vector2f startPoint{};

  sf::Vector2f endPoint{};
};

struct imguiColor
{
  float r{};
  float g{};
  float b{};
  float a{ 1.f };

  sf::Color asSfColor()
  {
    return sf::Color(std::uint8_t(r * 255.f), std::uint8_t(g * 255.f), std::uint8_t(b * 255.f), std::uint8_t(a * 255.f));
  }
};

inline imguiColor constructImguiColor(sf::Color color)
{
  return imguiColor((float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f);
}

inline std::optional<sf::Vector2f> lineIntersect(LineSegment first, LineSegment second)
{
  sf::Vector2f firstVector = (first.endPoint - first.startPoint);
  sf::Vector2f secondVector = (second.endPoint - second.startPoint);

  float lengthCrossProduct = firstVector.cross(secondVector);

  sf::Vector2f fms = second.startPoint - first.startPoint; //first line start point minus second

  float firstScalar = fms.cross(secondVector) / lengthCrossProduct;
  float secondScalar = fms.cross(firstVector) / lengthCrossProduct;

  if ((firstScalar >= 0 and firstScalar <= 1) and (secondScalar >= 0 and secondScalar <= 1))
    return sf::Vector2f(first.startPoint.x + (firstScalar * firstVector.x), first.startPoint.y + (firstScalar * firstVector.y));
  else
    return std::nullopt;
}

inline sf::VertexArray constructRectangle(sf::Vector2f center, sf::Vector2f size, sf::Color color = sf::Color::Black)
{
  sf::VertexArray rectangle(sf::PrimitiveType::LineStrip, 5);
  rectangle[0].color = color;
  rectangle[1].color = color;
  rectangle[2].color = color;
  rectangle[3].color = color;
  rectangle[4].color = color;

  rectangle[0].position = { center.x - size.x, center.y - size.y };
  rectangle[1].position = { center.x + size.x, center.y - size.y };
  rectangle[2].position = { center.x + size.x, center.y + size.y };
  rectangle[3].position = { center.x - size.x, center.y + size.y };
  rectangle[4].position = rectangle[0].position;

  return rectangle;

}

inline sf::Vector2f constructVector(sf::Vector2f startPoint, sf::Angle angle, float length)
{
  return { startPoint.x + (length * std::cos(angle.asRadians())), startPoint.y + (length * std::sin(angle.asRadians())) };
}

inline sf::VertexArray constructRandomPolygon(sf::Vector2f center, int vertexLowerBound, int vertexUpperBound, int averageSize, sf::Color color = sf::Color::Black)
{
  std::size_t vertexCount = getRandomNumber<std::size_t>(vertexLowerBound, vertexUpperBound);
  sf::VertexArray polygon(sf::PrimitiveType::LineStrip, vertexCount);

  float angleIncrement = 360.f / (float)(vertexCount - 1);
  float angleStart = getRandomNumber<float>(0, 360);

  for (int i = 0; i < vertexCount - 1; i++)
  {
    float angleOffset = getRandomNumber<float>(-(90 / (int)vertexCount), (90 / (int)vertexCount));
    polygon[i].color = color;
    polygon[i].position = constructVector(center, sf::degrees(angleStart + (angleIncrement * i) + angleOffset), getRandomNumber<float>((averageSize - 50), (averageSize + 50)));
  }
  polygon[vertexCount - 1].position = polygon[0].position;
  polygon[vertexCount - 1].color = color;
  return polygon;
}

inline float distanceBetween(sf::Vector2f startPoint, sf::Vector2f endPoint)
{
  return sqrtf(powf(endPoint.x - startPoint.x, 2) + powf(endPoint.y - startPoint.y, 2));
}
