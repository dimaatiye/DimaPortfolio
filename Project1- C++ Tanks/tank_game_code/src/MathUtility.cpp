#include "MathUtility.h"

namespace MathUtility
{
	////////////////////////////////////////////////////////////
	double distance(sf::Vector2f v1, sf::Vector2f v2)
	{
		return std::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}

	////////////////////////////////////////////////////////////
	bool lineIntersectsCircle(sf::Vector2f ahead, sf::Vector2f halfAhead, sf::CircleShape circle)
	{
		return distance(circle.getPosition(), ahead) <= circle.getRadius() ||
			distance(circle.getPosition(), halfAhead) <= circle.getRadius();
	}
	////////////////////////////////////////////////////////////
	float smoothRotate(float currentRotation, float targetRotation, float maxRotationSpeed) {
		float angleDiff = targetRotation - currentRotation;
		if (angleDiff > 180.0f) {
			angleDiff -= 360.0f;
		}
		else if (angleDiff < -180.0f) {
			angleDiff += 360.0f;
		}

		if (angleDiff > maxRotationSpeed) {
			angleDiff = maxRotationSpeed;
		}
		else if (angleDiff < -maxRotationSpeed) {
			angleDiff = -maxRotationSpeed;
		}

		return currentRotation + angleDiff;
	}
	////////////////////////////////////////////////////////////
	sf::Vector2f truncate(sf::Vector2f v, float const max)
	{		
		if (thor::length(v) > max)
		{
			v = thor::unitVector(v);			
			v *= max;
		}	

		return v;
	}

}
