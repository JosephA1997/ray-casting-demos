#include "Light.h"

void Light::draw(sf::RenderWindow& window)
{
	if (not m_visible) return;

	changeColor(lightColor.asSfColor());
	window.draw(m_light.data(), m_light.size(), sf::PrimitiveType::TriangleFan);
	if (not drawEndPoints) return;
	for (auto& point : m_light)
	{
		point.color = sf::Color::Black;
		sf::CircleShape endPoint(5.f, 20u);
		endPoint.setOrigin({ 5, 5 });
		endPoint.setFillColor(rayColor.asSfColor());
		endPoint.setPosition(point.position);
		window.draw(endPoint);
	}
}

void Light::addRay(Ray ray)
{
	m_rays.emplace_back(ray);
}

void Light::changeColor(sf::Color color)
{
	for (auto& points : m_light)
		points.color = color;

	for (auto& ray : m_rays)
		ray.changeColor(color);
}

void Light::processRays(const std::vector<sf::VertexArray>& polygons)
{
	if (m_center and m_offset)
	{
		constructPartialLight(polygons);
	}
	else
	{
		constructFullLight();
	}

}

void Light::constructFullLight()
{
	std::sort(m_rays.begin(), m_rays.end(), [](const auto& lhs, const auto& rhs) { return lhs.angle().wrapUnsigned().asRadians() < rhs.angle().wrapUnsigned().asRadians(); });

	for (auto& ray : m_rays)
	{
		m_light.emplace_back(ray.points()[end]);
	}

	m_light.emplace_back(m_rays.front().points()[end]);//fully connect the triangle fan

}

void Light::constructPartialLight(const std::vector<sf::VertexArray>& polygons)
{
	auto upperAngleBound = m_center.value() + m_offset.value();
	auto lowerAngleBound = m_center.value() - m_offset.value();
	Ray upperBound(m_light[0].position, rayLength, upperAngleBound, lightColor.asSfColor());
	Ray lowerBound(m_light[0].position, rayLength, lowerAngleBound, lightColor.asSfColor());

	m_rays.emplace_back(upperBound);
	m_rays.emplace_back(lowerBound);
	for (auto& ray : m_rays)
	{
		ray.calculateIntersect(polygons);
	}

	if ((m_center.value().wrapUnsigned().asDegrees() - m_offset.value().wrapUnsigned().asDegrees() < 0.f) or (m_center.value().wrapUnsigned().asDegrees() + m_offset.value().wrapUnsigned().asDegrees() > 360.f))
	{
		std::sort(m_rays.begin(), m_rays.end(), [](const auto& lhs, const auto& rhs) { return lhs.angle().wrapSigned().asRadians() < rhs.angle().wrapSigned().asRadians(); });
		for (auto& ray : m_rays)
		if (ray.angle().wrapSigned() >= lowerAngleBound.wrapSigned() and ray.angle().wrapSigned() <= upperAngleBound.wrapSigned())
		{
			m_light.emplace_back(ray.points()[end]);
		}
	}
	else
	{
		std::sort(m_rays.begin(), m_rays.end(), [](const auto& lhs, const auto& rhs) { return lhs.angle().wrapUnsigned().asRadians() < rhs.angle().wrapUnsigned().asRadians(); });
		for (auto& ray : m_rays)
			if (ray.angle().wrapUnsigned() >= lowerAngleBound.wrapUnsigned() and ray.angle().wrapUnsigned() <= upperAngleBound.wrapUnsigned())
			{
				m_light.emplace_back(ray.points()[end]);
			}
	}
	
}