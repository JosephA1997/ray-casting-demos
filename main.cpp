#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <cmath>

#include <iostream>
#include <vector>
#include <algorithm>

#include "Helper.hpp"

struct LineSegment 
{
  sf::Vector2f a{};

  sf::Vector2f b{};
};

struct Intersect { bool result{}; sf::Vector2f pos{};  };

struct imguiColor
{
  float r{};
  float g{};
  float b{};
  float a{1.f};

  sf::Color asSfColor()
  {
    return sf::Color(std::uint8_t(r * 255.f), std::uint8_t(g * 255.f), std::uint8_t(b * 255.f), std::uint8_t(a * 255.f));
  }
};

imguiColor constructImguiColor(sf::Color color)
{
  return imguiColor((float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f);
}

Intersect lineIntersect(LineSegment first, LineSegment second)
{
  sf::Vector2f firstVector = (first.b - first.a);
  sf::Vector2f secondVector = (second.b - second.a);

  float lengthCrossProduct = firstVector.cross(secondVector);

  sf::Vector2f fms = second.a - first.a; //first line start point minus second

  float firstScalar = fms.cross(secondVector) / lengthCrossProduct;
  float secondScalar = fms.cross(firstVector) / lengthCrossProduct;
  
  if ((firstScalar >= 0 and firstScalar <= 1) and (secondScalar >= 0 and secondScalar <= 1))
    return{ true, sf::Vector2f(first.a.x + (firstScalar * firstVector.x), first.a.y + (firstScalar * firstVector.y)) };
  else
    return{ false, sf::Vector2f(0,0) };
}

sf::VertexArray constructRectangle(sf::Vector2f center, sf::Vector2f size, sf::Color color = sf::Color::Black)
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
  rectangle[4].position = { center.x - size.x, center.y - size.y };

  return rectangle;

}

sf::Vector2f constructVector(sf::Vector2f startPoint, sf::Angle angle, float length)
{
  return { startPoint.x + (length * std::cos(angle.asRadians())), startPoint.y + (length * std::sin(angle.asRadians())) };
}

sf::VertexArray constructRandomPolygon(sf::Vector2f center, int vertexLowerBound, int vertexUpperBound, int averageSize, sf::Color color = sf::Color::Black)
{
  size_t vertexCount = getRandomNumber<size_t>(vertexLowerBound, vertexUpperBound);
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

sf::VertexArray constructRay(sf::Vector2f start, float length, sf::Angle angle, sf::Color color = sf::Color::Red) 
{
  sf::VertexArray ray(sf::PrimitiveType::Lines, 2);

  ray[0].position = start;
  ray[0].color = color;
  ray[1].position = constructVector(start, angle, length);
  ray[1].color = color;
    
  return ray;
}

int main()
{ 
  sf::Vector2f center{ 1920.f / 2.f, 1080.f / 2.f };
  
  //sfml setup
  auto window = sf::RenderWindow(sf::VideoMode({ 1920u, 1080u }), "Ray Casting Test");

  window.setFramerateLimit(144);
  if (!ImGui::SFML::Init(window))
    return -1;

  sf::Clock clock;
  sf::Vector2f mousePos = center;

  //imGui Variables
  float polySize[2] = { 100.f, 100.f };
  int numRays{ 10 };
  int vertexBounds[2] = { 3u, 10u };
  int averageSize{ 100u };
  float rayLength{ 5000.f };
  imguiColor rayColor = constructImguiColor(sf::Color::Red);


  //initalize vectors
  std::vector<sf::VertexArray> rays;

  {
    float i = 0.f, angleIncrement = 360.f / (float)numRays;
    for (int i = 0; i < numRays; i++) rays.push_back(constructRay(mousePos, 5000.f, sf::degrees(angleIncrement * i), rayColor.asSfColor()));
  }

  std::vector<sf::VertexArray> polygons;
  polygons.push_back(constructRectangle(center, center));//outer bounds of the window

  
  //main game loop
  while (window.isOpen())
  {

    //handle input
    while (const std::optional event = window.pollEvent())
    {
      ImGui::SFML::ProcessEvent(window, *event);

      if (event->is<sf::Event::Closed>())
      {
        window.close();
      }

      if (const auto* buttonPressed = event->getIf<sf::Event::KeyPressed>())
      {
        switch (buttonPressed->scancode)
        {
        case sf::Keyboard::Scancode::C:
          polygons.clear();
          polygons.push_back(constructRectangle(center, center));
          break;

        case sf::Keyboard::Scancode::R:
          polygons.push_back(constructRandomPolygon(mousePos, vertexBounds[0] + 1, vertexBounds[1] + 1, averageSize));
          break;

        default:
          break;
        }
      }

      if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
      {
        switch (mousePressed->button)
        {
        case sf::Mouse::Button::Right:
          polygons.push_back(constructRectangle((sf::Vector2f)mousePressed->position, { polySize[0], polySize[1] }, sf::Color::Black));
        break; 

        default:
          break;
        }
      }

      if (rays.empty()) continue;
      if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
      {
        mousePos = (sf::Vector2f)mouseMoved->position;
        for (auto& ray : rays) ray[0].position = mousePos;
      }
    }// end user input loop

    {
      float i = 0, angleIncrement = 360.f / (float)rays.size();
      for (auto& ray : rays)
      {
        ray[1].position = constructVector(ray[0].position, sf::degrees(i * angleIncrement), rayLength);
        i++;
      }
    }

    for (auto& ray : rays)
    {

      sf::Vector2f closestIntersection = ray[1].position;
      Intersect currentIntersect{};
      for (auto& poly : polygons)
      {
        for (int i = 1; i <= poly.getVertexCount() - 1; i++) {
          currentIntersect = lineIntersect({ ray[0].position, ray[1].position }, { poly[i - 1].position , poly[i].position });
          if (currentIntersect.result)
          {
            if ((closestIntersection - ray[0].position).length() > (currentIntersect.pos - ray[0].position).length())
            {
              closestIntersection = currentIntersect.pos;
            }
          }
        }
      }

      ray[1].position = closestIntersection;
    }

    ImGui::SFML::Update(window, clock.restart());

    ImGui::Begin("Config");
    ImGui::Text("Controls:");
    ImGui::Indent();
    ImGui::Text("Right Click: place rectangle at mouse position");
    ImGui::Text("R: place a randomly sized polygon at mouse position");
    ImGui::Text("C: clear all polygons");
    ImGui::Unindent();
    if (ImGui::SliderInt("number of rays", &numRays, 0, 50))
    {
      rays.clear();

      {
        float i = 0.f, angleIncrement = 360.f / (float)numRays;
        for (int i = 0; i < numRays; i++) rays.push_back(constructRay(mousePos, rayLength, sf::degrees(angleIncrement * i), rayColor.asSfColor()));
      }

    }
    ImGui::InputFloat("Ray Length", &rayLength);
    if (ImGui::ColorEdit3("Ray color", &rayColor.r))
    {
      for (auto& ray : rays)
      {
        ray[0].color = rayColor.asSfColor();
        ray[1].color = rayColor.asSfColor();
      }
    }
    ImGui::InputFloat2("rectangle Size (X,Y)", polySize, "%.1f");
    ImGui::Text("Random polygon config: ");
    if (ImGui::SliderInt2("vertex count lower/upper bounds", vertexBounds, 3, 20))
    {
      if (vertexBounds[0] > vertexBounds[1])
        vertexBounds[0] = vertexBounds[1];

      if (vertexBounds[1] < vertexBounds[0])
        vertexBounds[0] = vertexBounds[1];
    }

    if(ImGui::InputInt("Average size", &averageSize))
    {
      if (averageSize < 20)
      {
        averageSize = 20;
      }
    }

    ImGui::End();

    window.clear(sf::Color::White);

    //render entities
    for (auto& poly : polygons)
    window.draw(poly);

    for (auto& ray : rays)
    {
      sf::CircleShape endPoint(5.f, 20u);
      endPoint.setOrigin({ 5, 5 });
      endPoint.setFillColor(rayColor.asSfColor());
      endPoint.setPosition(ray[1].position);
      window.draw(ray);
      window.draw(endPoint);
    }

    ImGui::SFML::Render(window);

    window.display();
  }

  ImGui::SFML::Shutdown();
}
