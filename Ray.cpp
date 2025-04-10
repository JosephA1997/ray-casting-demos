#include "Ray.h"

 Ray::Ray(sf::Vector2f startPoint, sf::Vector2f endPoint, sf::Color color)
{
  m_points[start].position = startPoint;
  m_points[start].color = color;
  m_points[end].position = endPoint;
  m_points[end].color = color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 Ray::Ray(sf::Vector2f startPoint, float length, sf::Angle angle, sf::Color color)
{
  m_points[start].position = startPoint;
  m_points[start].color = color;
  m_points[end].position = constructVector(startPoint, angle, length);
  m_points[end].color = color;
  m_angle = angle;
}

 Ray::Ray(const Ray& ray)
{
  auto& newPoints = ray.points();
  m_points[start].position = newPoints[start].position;
  m_points[start].color = newPoints[start].color;
  m_points[end].position = newPoints[end].position;
  m_points[end].color = newPoints[end].color;
  m_angle = ray.angle();
}

 void Ray::change(const Ray& ray, bool copyColor)
{
  auto& newPoints = ray.points();
  m_points[start].position = newPoints[start].position;
  m_points[end].position = newPoints[end].position;
  if (copyColor)
  {
    m_points[start].color = newPoints[start].color;
    m_points[end].color = newPoints[0].color;
  }
  m_angle = ray.angle();
}

 void Ray::change(sf::Vector2f startPoint, sf::Vector2f endPoint)
{
  m_points[start].position = startPoint;
  m_points[end].position = endPoint;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::change(sf::Vector2f startPoint, sf::Vector2f endPoint, sf::Color color)
{
  m_points[start].position = startPoint;
  m_points[start].color = color;
  m_points[end].position = endPoint;
  m_points[end].color = color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeStart(sf::Vector2f startPoint, sf::Color color)
{
  m_points[start].position = startPoint;
  m_points[start].color = color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeStart(sf::Vector2f startPoint)
{
  m_points[start].position = startPoint;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeStart(const Ray& ray, bool copyColor)
{
  auto& newStart = ray.points()[0];
  m_points[start].position = newStart.position;
  if (copyColor) m_points[start].color = newStart.color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeEnd(sf::Vector2f endPoint, sf::Color color)
{
  m_points[end].position = endPoint;
  m_points[end].color = color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeEnd(sf::Vector2f endPoint)
{
  m_points[end].position = endPoint;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeEnd(const Ray& ray, bool copyColor)
{
  auto& newEnd = ray.points()[end];
  m_points[end].position = newEnd.position;
  if (copyColor) m_points[end].color = newEnd.color;
  if (m_points[start].position - m_points[end].position == sf::Vector2f{ 0.f, 0.f })
  {
    m_angle = sf::Angle::Zero;
  }
  else
  {
    m_angle = sf::radians(std::atan2f(m_points[end].position.y - m_points[start].position.y, m_points[end].position.x - m_points[start].position.x));
  }
}

 void Ray::changeColor(sf::Color color)
{
  m_points[start].color = color;
  m_points[end].color = color;
}
