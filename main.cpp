#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstddef>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "Utility.hpp"
#include "Ray.h"

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
  int numRays{ 500 };
  int vertexBounds[2] = { 3u, 10u };
  int averageSize{ 100u };
  float rayLength{ 500.f };
  imguiColor rayColor = constructImguiColor(sf::Color::Red);
  imguiColor lightColor = constructImguiColor(sf::Color::Red);
  bool drawRays{ false };
  bool drawLight{ true };


  //initalize vectors
  RayVector rays;

  std::vector<sf::VertexArray> polygons;
  polygons.push_back(constructRectangle(center, center));
  
  //main game loop
  while (window.isOpen())
  {
    rays.clear();
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
          break;

        case sf::Keyboard::Scancode::R:
          polygons.push_back(constructRandomPolygon(mousePos, vertexBounds[0] + 1, vertexBounds[1] + 1, averageSize));
          break;

        case sf::Keyboard::Scancode::Escape:
          window.close();
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

      if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
      {
        mousePos = (sf::Vector2f)mouseMoved->position;
        for (auto& ray : rays) ray.changeStart(mousePos);
      }
    }// end user input loop

    if (numRays > 0)
    {
      float i = 0.f, angleIncrement = 360.f / (float)numRays;
      for (int i = 0; i < numRays; i++) rays.push_back(Ray(mousePos, rayLength, sf::degrees(angleIncrement * i), rayColor.asSfColor()));
    }

    for (auto& poly : polygons)
    {
      for (std::size_t i = 0; i < poly.getVertexCount() - 1; i++)
      {
        if (distanceBetween(mousePos, poly[i].position) <= rayLength)
        {
          sf::Angle angle = sf::radians(std::atan2f(poly[i].position.y - mousePos.y, poly[i].position.x - mousePos.x));
          rays.push_back(Ray(mousePos, poly[i].position, rayColor.asSfColor()));
          rays.push_back(Ray(mousePos, rayLength, angle + sf::radians(.00001f), rayColor.asSfColor()));
          rays.push_back(Ray(mousePos, rayLength, angle - sf::radians(.00001f), rayColor.asSfColor()));
        }
      }
    }

    for (auto& ray : rays)
    {
      auto& rayPoints = ray.points();
      sf::Vector2f closestIntersection = rayPoints[end].position;
      std::optional<sf::Vector2f> currentIntersect{};
      for (auto& poly : polygons)
      {
        for (int i = 1; i <= poly.getVertexCount() - 1; i++) {
          currentIntersect = lineIntersect({ rayPoints[start].position, rayPoints[end].position }, { poly[i - 1].position , poly[i].position });
          if (currentIntersect)
          {
            if ((closestIntersection - rayPoints[start].position).length() > (currentIntersect.value() - rayPoints[start].position).length())
            {
              closestIntersection = currentIntersect.value();
            }
          }
        }
      }
      ray.changeEnd(closestIntersection);
    }

    std::sort(rays.begin(), rays.end(), [](const auto& lhs, const auto& rhs) { return lhs.angle().wrapUnsigned().asRadians() < rhs.angle().wrapUnsigned().asRadians(); });

    sf::VertexArray light(sf::PrimitiveType::TriangleFan, 1);
    if (not rays.empty())
    {
      light[0].position = mousePos;
      light[0].color = lightColor.asSfColor();
      for (auto& ray : rays)
      {
        light.append(ray.points()[end]);
      }
      light.append(rays.front().points()[end]);
    }

    for (int i = 0; i < light.getVertexCount(); i++)
      light[i].color = lightColor.asSfColor();

    
    ImGui::SFML::Update(window, clock.restart());

    ImGui::Begin("Config");
    ImGui::Text("Controls:");
    ImGui::Indent();
    ImGui::Text("Right Click: place rectangle at mouse position");
    ImGui::Text("R: place a randomly sized polygon at mouse position");
    ImGui::Text("C: clear all polygons");
    ImGui::Unindent();
    ImGui::Checkbox("Render rays", &drawRays);
    ImGui::SameLine();
    ImGui::Checkbox("Render light", &drawLight);
    if (ImGui::SliderInt("number of rays", &numRays, 5, 500))
    {
      rays.clear();

      {
        float i = 0.f, angleIncrement = 360.f / (float)numRays;
        for (int i = 0; i < numRays; i++) rays.push_back(Ray(mousePos, rayLength, sf::degrees(angleIncrement * i), rayColor.asSfColor()));
      }

    }
    ImGui::InputFloat("Ray Length", &rayLength);
    if (ImGui::ColorEdit3("Ray color", &rayColor.r))
    {
      for (auto& ray : rays) ray.changeColor(rayColor.asSfColor());
    }
    if (ImGui::ColorEdit3("Light color", &lightColor.r))
    {
      for (int i = 0; i < light.getVertexCount(); i++)
        light[i].color = lightColor.asSfColor();
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

    if (ImGui::InputInt("Average size", &averageSize))
    {
      if (averageSize < 20)
      {
        averageSize = 20;
      }
    }

    ImGui::End();

    window.clear(sf::Color::White);

    //render entities
    for (auto& poly : polygons) window.draw(poly);

    if (drawRays)
    {
      for (auto& ray : rays)
      {
        auto& rayPoints = ray.points();
        sf::CircleShape endPoint(5.f, 20u);
        endPoint.setOrigin({ 5, 5 });
        endPoint.setFillColor(rayColor.asSfColor());
        endPoint.setPosition(rayPoints[end].position);
        window.draw(rayPoints);
        window.draw(endPoint);
      }
    }

    if (drawLight) window.draw(light);

    ImGui::SFML::Render(window);

    window.display();
  }

  ImGui::SFML::Shutdown();
}
