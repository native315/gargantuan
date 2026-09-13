#include "gargantuan/filesystem/PathResolver.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/filesystem/Paths.hpp"

#include <filesystem>
#include <optional>
#include <string_view>

namespace gargantuan {
	namespace {
		constexpr std::string_view ASSET_SCHEME = "project://";
		constexpr std::string_view SCHEME_SEPARATOR = "://";
	}

	std::filesystem::path PathResolver::GetContentDirectory() {
		return Paths::GetExecutableDirectory() / "content";
	}

	std::optional<std::filesystem::path> PathResolver::Resolve(std::string_view url) {
		if (url.empty()) return std::nullopt;

		if (url.starts_with(ASSET_SCHEME)) {
			std::string_view relative = url.substr(ASSET_SCHEME.size());

			if (relative.empty() || relative.find("..") != std::string_view::npos) {
				LOG_WARN(App, "Refusing to resolve asset URL outside of content: %.*s", (int)url.size(), url.data());
				return std::nullopt;
			}

			return GetContentDirectory() / std::filesystem::path(relative);
		}

		if (url.find(SCHEME_SEPARATOR) != std::string_view::npos) {
			LOG_WARN(App, "Cannot resolve URL with unsupported scheme: %.*s", (int)url.size(), url.data());
			return std::nullopt;
		}

		return std::filesystem::path(url);
	}
} // namespace gargantuan
