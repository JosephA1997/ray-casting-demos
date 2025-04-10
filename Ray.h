#pragma once
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstddef>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "Utility.hpp"

//used for sf::VertexArray index
enum
{
  start,
  end
};

class Ray
{
public:
  Ray(sf::Vector2f startPoint, sf::Vector2f endPoint, sf::Color color = sf::Color::Red);
  Ray(sf::Vector2f startPoint, float length, sf::Angle angle, sf::Color color = sf::Color::Red);
  Ray(const Ray& ray);

  ~Ray() = default;

  const sf::Angle& angle() const { return m_angle; }
  const sf::VertexArray& points() const { return m_points; }

  void change(const Ray& ray, bool copyColor = false);
  void change(sf::Vector2f startPoint, sf::Vector2f endPoint);
  void change(sf::Vector2f startPoint, sf::Vector2f endPoint, sf::Color color);

  void changeStart(sf::Vector2f startPoint, sf::Color color);
  void changeStart(sf::Vector2f startPoint);
  void changeStart(const Ray& ray, bool copyColor = false);

  void changeEnd(sf::Vector2f endPoint, sf::Color color);
  void changeEnd(sf::Vector2f endPoint);
  void changeEnd(const Ray& ray, bool copyColor = false);

  void changeColor(sf::Color color);

  bool operator== (const Ray& rhs)
  {
    return m_points[start].position == rhs.points()[start].position and m_points[end].position == rhs.points()[end].position;
  }

  Ray operator+ (const Ray& rhs)
  {
    return Ray(this->m_points[start].position + rhs.m_points[start].position, this->m_points[end].position + rhs.m_points[end].position);
  }

  Ray operator- (const Ray& rhs)
  {
    return Ray(this->m_points[start].position - rhs.m_points[start].position, this->m_points[end].position - rhs.m_points[end].position);
  }

private:
  sf::VertexArray m_points{ sf::PrimitiveType::Lines, 2 };//points[0] = origin of ray, points[1] = end of ray
  sf::Angle m_angle{};
};

using RayVector = std::vector<Ray>;