#include "gargantuan/services/Lighting.hpp"
#include "gargantuan/classes/Sky.hpp"

#include <glm.hpp>
#include <gtc/constants.hpp>
#include <memory>

namespace gargantuan {
	std::shared_ptr<Sky> Lighting::FindSky() {
		return std::dynamic_pointer_cast<Sky>(FindFirstChildOfClass("Sky", false));
	}

	glm::vec3 Lighting::GetSunDirection() {
		float lat = ((GeographicLatitude - 23.5) / 360) * glm::two_pi<float>();
		float lon = ((ClockTime + 6) / 24) * glm::two_pi<float>();

		float z = sin(lat);
		float hradius = cos(lat);

		float lf = -1; // -1 = sun, 1 = moon
		float x = (hradius * cos(lon)) / lf;
		float y = (hradius * sin(lon)) / lf;

		return glm::normalize(glm::vec3(x, y, z));
	}

	glm::vec3 Lighting::GetMoonDirection() {
		float lat = ((GeographicLatitude - 23.5) / 360) * glm::two_pi<float>();
		float lon = ((ClockTime + 6) / 24) * glm::two_pi<float>();

		float z = sin(lat);
		float hradius = cos(lat);

		float lf = 1; // -1 = sun, 1 = moon
		float x = (hradius * cos(lon)) / lf;
		float y = (hradius * sin(lon)) / lf;

		return glm::normalize(glm::vec3(x, y, z));
	}
}
