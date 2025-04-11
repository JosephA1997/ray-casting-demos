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
#include "Ray.h"

class Light
{
public:

	Light(bool visible = true)
		: m_visible(visible)
	{
		m_light.push_back(sf::Vertex());
	}

	Light(sf::Angle center, sf::Angle offset, bool visible = true)
		: m_center(center)
		, m_offset(offset)
		, m_visible(visible)
	{
		m_light.push_back(sf::Vertex{});
	}

	void addAngleBounds(sf::Angle center, sf::Angle offset)
	{
		m_center = center;
		m_offset = offset;
	}

	void removeAngleBounds()
	{
		m_center.reset();
		m_offset.reset();
	}

	void setVisibility(bool input) { m_visible = input; }
	bool visible() const { return m_visible; }

	void setCenterAngle(sf::Angle center) { m_center = center; }
	std::optional<sf::Angle> getCenterAngle() const { return m_center; }

	void setOffsetAngle(sf::Angle offset) { m_offset = offset; }
	std::optional<sf::Angle> getOffsetAngle() const { return m_offset; }

	sf::Vector2f getOrigin() const { return m_light[0].position; }
	void setOrigin(sf::Vector2f center) { m_light[0].position = center; }

	void draw(sf::RenderWindow& window);

	void addRay(Ray ray);

	void clearRays() { m_rays.clear(); m_light.clear(); m_light.push_back(sf::Vertex{});}

	void changeColor(sf::Color color);

	void processRays(const std::vector<sf::VertexArray>& polygons);
private:
	std::vector<sf::Vertex> m_light{};
	RayVector m_rays{};
	std::optional<sf::Angle> m_center{};
	std::optional<sf::Angle> m_offset{};

	bool m_visible{};

	void constructFullLight();
	void constructPartialLight(const std::vector<sf::VertexArray>& polygons);
};

