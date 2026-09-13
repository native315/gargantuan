#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace gargantuan {
	class PathResolver {
	  public:
		static std::filesystem::path GetContentDirectory();

		static std::optional<std::filesystem::path> Resolve(std::string_view url);
	};
} // namespace gargantuan
