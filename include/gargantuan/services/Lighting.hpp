#pragma once

#include "gargantuan/classes/Sky.hpp"
#include "gargantuan/services/generated/Lighting.hpp"

#include <memory>

namespace gargantuan {
	class Lighting : public Instance {
		I_Lighting;

	  public:
		std::shared_ptr<Sky> FindSky();
	};
}
