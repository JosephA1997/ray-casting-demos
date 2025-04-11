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
#include "Light.h"


auto window = sf::RenderWindow(sf::VideoMode({ 1920u, 1080u }), "Ray Casting Test");

  Light light;
  RayVector rays;
  std::vector<sf::VertexArray> polygons;

void GUI();
void userInput(std::vector<sf::VertexArray>& polygons);
void processRays();
void render();

int main()
{
  sf::Vector2f center{ 1920.f / 2.f, 1080.f / 2.f };

  //sfml setup
  sf::Clock clock;
  window.setFramerateLimit(144);
  if (!ImGui::SFML::Init(window))
    return -1;

  mousePos = center;

  polygons.emplace_back(constructRectangle(center, center));
  //main game loop
  while (window.isOpen())
  {
    ImGui::SFML::Update(window, clock.restart());

    rays.clear();
    light.clearRays();

    userInput(polygons);

    processRays();

    light.setOrigin(mousePos);

    for (auto& ray : rays) light.addRay(ray);

    GUI();

    render();

  }

  ImGui::SFML::Shutdown();
}

void GUI()
{
  ImGui::Begin("Config");
  ImGui::Text("Controls:");
  ImGui::Indent();
  ImGui::Text("Right Click: place rectangle at mouse position");
  ImGui::Text("R: place a randomly sized polygon at mouse position");
  ImGui::Text("C: clear all polygons");
  ImGui::Text("Scroll wheel: increase/decrease light direction (if light is bounded)");
  ImGui::Unindent();
  ImGui::Checkbox("Render rays", &drawRays);
  ImGui::SameLine();
  if (ImGui::Checkbox("Render light", &drawLight))
  {
    light.setVisibility(drawLight);
  }
  if (ImGui::SliderInt("number of rays", &numRays, 5, 500))
  {
    rays.clear();

    {
      float i = 0.f, angleIncrement = 360.f / (float)numRays;
      for (int i = 0; i < numRays; i++) rays.emplace_back(mousePos, rayLength, sf::degrees(angleIncrement * i), rayColor.asSfColor());
    }

  }
  ImGui::InputFloat("Ray Length", &rayLength);
  if (ImGui::ColorEdit3("Ray color", &rayColor.r))
  {
    for (auto& ray : rays) ray.changeColor(rayColor.asSfColor());
  }
  ImGui::ColorEdit3("Light color", &lightColor.r);
  if(ImGui::Checkbox("Light is bounded", &boundedLight))
  {
    if (boundedLight)
    {
      light.addAngleBounds(sf::degrees((float)lightCenterAngle), sf::degrees((float)lightOffsetAngle));
    }
    else
    {
      light.removeAngleBounds();
    }
  }
  if (boundedLight)
  {
    ImGui::Checkbox("Draw end points", &drawEndPoints);
    ImGui::SliderInt("Light direction", &lightCenterAngle, 0, 359);
    light.setCenterAngle(sf::degrees((float)lightCenterAngle));
   
    if (ImGui::InputInt("Offset size", &lightOffsetAngle))
    {
      if (lightOffsetAngle > 89)
      {
        lightOffsetAngle = 89;
      }
      if (lightOffsetAngle < 0)
      {
        lightOffsetAngle = 0;
      }
    }
    light.setOffsetAngle(sf::degrees((float)lightOffsetAngle));
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
}

void userInput(std::vector<sf::VertexArray>& polygons)
{
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
        polygons.emplace_back(constructRandomPolygon(mousePos, vertexBounds[0] + 1, vertexBounds[1] + 1, averageSize));
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
        polygons.emplace_back(constructRectangle((sf::Vector2f)mousePressed->position, { polySize[0], polySize[1] }, sf::Color::Black));
        break;

      default:
        break;
      }
    }

    if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
    {
      switch (mouseWheelScrolled->wheel)
      {
      case sf::Mouse::Wheel::Vertical:
        if (boundedLight)
          lightCenterAngle = std::clamp(lightCenterAngle += (int)mouseWheelScrolled->delta, 0, 360);
        break;

      default:
        break;
      }
    }

    if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
    {
      mousePos = (sf::Vector2f)mouseMoved->position;
    }
  }
}

void processRays() 
{
  if (numRays > 0)
  {
    float i = 0.f, angleIncrement = 360.f / (float)numRays;
    for (int i = 0; i < numRays; i++) rays.emplace_back(Ray(mousePos, rayLength, sf::degrees(angleIncrement * i), rayColor.asSfColor()));
  }

  for (auto& poly : polygons)
  {
    for (std::size_t i = 0; i < poly.getVertexCount() - 1; i++)
    {
      if (distanceBetween(mousePos, poly[i].position) <= rayLength)
      {
        sf::Angle angle = sf::radians(std::atan2f(poly[i].position.y - mousePos.y, poly[i].position.x - mousePos.x));
        rays.emplace_back(Ray(mousePos, poly[i].position, rayColor.asSfColor()));
        rays.emplace_back(Ray(mousePos, rayLength, angle + sf::radians(.00001f), rayColor.asSfColor()));
        rays.emplace_back(Ray(mousePos, rayLength, angle - sf::radians(.00001f), rayColor.asSfColor()));
      }
    }
  }

  for (auto& ray : rays)
  {
    ray.calculateIntersect(polygons);
  }
  
}

void render()
{
  window.clear(sf::Color::White);

  light.processRays(polygons);
  light.draw(window);

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

  ImGui::SFML::Render(window);

  window.display();
}