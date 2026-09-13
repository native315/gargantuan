#include "gargantuan/render/GpuCubemap.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/filesystem/PathResolver.hpp"
#include "gargantuan/filesystem/Paths.hpp"

#include <SDL3_image/SDL_image.h>
#include <array>
#include <cstring>

namespace gargantuan {
	namespace {
		SDL_Surface *LoadFace(const std::string &url) {
			auto path = PathResolver::Resolve(url);
			if (!path) {
				return nullptr;
			}

			auto loaded = IMG_Load(Paths::ToUtf8(*path).c_str());
			if (!loaded) {
				LOG_ERROR(App, "Failed to load cubemap face %s: %s", url.c_str(), SDL_GetError());
				return nullptr;
			}

			if (loaded->format == SDL_PIXELFORMAT_RGBA32) {
				return loaded;
			}

			auto converted = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
			SDL_DestroySurface(loaded);

			if (!converted) {
				LOG_ERROR(App, "Failed to convert cubemap face %s: %s", url.c_str(), SDL_GetError());
			}

			return converted;
		}
	} // namespace

	GpuCubemap::GpuCubemap(
		const char *ft, const char *bk, const char *dn, const char *up, const char *rt, const char *lf
	) {
		this->Urls = {lf, rt, up, dn, bk, ft};
	}

	SDL_GPUTexture *GpuCubemap::CreateTexture(SDL_GPUDevice *gpu) {
		if (Texture) {
			return Texture;
		}

		SDL_GPUTextureCreateInfo info = {
			.type = SDL_GPU_TEXTURETYPE_CUBE,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = Width,
			.height = Height,
			.layer_count_or_depth = CUBEMAP_FACE_COUNT,
			.num_levels = 1,
		};

		Texture = SDL_CreateGPUTexture(gpu, &info);
		if (!Texture) {
			LOG_ERROR(App, "Failed to create cubemap texture: %s", SDL_GetError());
		}

		return Texture;
	}

	SDL_GPUSampler *GpuCubemap::CreateSampler(SDL_GPUDevice *gpu) {
		if (Sampler) {
			return Sampler;
		}

		SDL_GPUSamplerCreateInfo info = {
			.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		};

		Sampler = SDL_CreateGPUSampler(gpu, &info);
		if (!Sampler) {
			LOG_ERROR(App, "Failed to create cubemap sampler: %s", SDL_GetError());
		}

		return Sampler;
	}

	SDL_GPUTransferBuffer *GpuCubemap::CreateTransferBuffer(SDL_GPUDevice *gpu) {
		if (TransferBuffer) {
			return TransferBuffer;
		}

		std::array<SDL_Surface *, CUBEMAP_FACE_COUNT> faces = {};
		bool valid = true;

		for (uint32_t face = 0; face < CUBEMAP_FACE_COUNT && valid; face++) {
			faces[face] = LoadFace(Urls[face]);
			if (!faces[face]) {
				valid = false;
				break;
			}

			if (face == 0) {
				Width = faces[0]->w;
				Height = faces[0]->h;

				if (Width == 0 || Width != Height) {
					LOG_ERROR(App, "Cubemap faces must be square, got %ux%u", Width, Height);
					valid = false;
				}
			} else if ((uint32_t)faces[face]->w != Width || (uint32_t)faces[face]->h != Height) {
				LOG_ERROR(
					App,
					"Cubemap face %s is %dx%d, expected %ux%u",
					Urls[face].c_str(),
					faces[face]->w,
					faces[face]->h,
					Width,
					Height
				);
				valid = false;
			}
		}

		const uint32_t rowSize = Width * 4;
		FaceBufferSize = rowSize * Height;

		if (valid) {
			SDL_GPUTransferBufferCreateInfo info = {
				.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = CUBEMAP_FACE_COUNT * FaceBufferSize,
			};

			TransferBuffer = SDL_CreateGPUTransferBuffer(gpu, &info);
			if (!TransferBuffer) {
				LOG_ERROR(App, "Failed to create cubemap transfer buffer: %s", SDL_GetError());
			}
		}

		if (TransferBuffer) {
			auto pointer = (uint8_t *)SDL_MapGPUTransferBuffer(gpu, TransferBuffer, false);

			for (uint32_t face = 0; face < CUBEMAP_FACE_COUNT; face++) {
				auto destination = pointer + (face * FaceBufferSize);
				auto source = (const uint8_t *)faces[face]->pixels;

				for (uint32_t row = 0; row < Height; row++) {
					std::memcpy(destination + (row * rowSize), source + (row * faces[face]->pitch), rowSize);
				}
			}

			SDL_UnmapGPUTransferBuffer(gpu, TransferBuffer);
		}

		for (auto face : faces) {
			if (face) {
				SDL_DestroySurface(face);
			}
		}

		return TransferBuffer;
	}

	void GpuCubemap::DestroyTransferBuffer(SDL_GPUDevice *gpu) {
		if (TransferBuffer) {
			SDL_ReleaseGPUTransferBuffer(gpu, TransferBuffer);
			TransferBuffer = nullptr;
		}
	}

	bool GpuCubemap::Upload(SDL_GPUDevice *gpu, SDL_GPUCopyPass *copyPass) {
		if (!CreateTransferBuffer(gpu)) {
			return false;
		}

		if (!CreateTexture(gpu) || !CreateSampler(gpu)) {
			DestroyTransferBuffer(gpu);
			return false;
		}

		for (uint32_t face = 0; face < CUBEMAP_FACE_COUNT; face++) {
			SDL_GPUTextureTransferInfo source{
				.transfer_buffer = TransferBuffer,
				.offset = face * FaceBufferSize,
				.pixels_per_row = Width,
				.rows_per_layer = Height,
			};

			SDL_GPUTextureRegion destination{
				.texture = Texture,
				.layer = face,
				.w = Width,
				.h = Height,
				.d = 1,
			};

			SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
		}

		DestroyTransferBuffer(gpu);

		LOG_INFO(App, "Uploaded %ux%u cubemap", Width, Height);

		return true;
	}

	void GpuCubemap::Destroy(SDL_GPUDevice *gpu) {
		DestroyTransferBuffer(gpu);

		if (Texture) {
			SDL_ReleaseGPUTexture(gpu, Texture);
			Texture = nullptr;
		}

		if (Sampler) {
			SDL_ReleaseGPUSampler(gpu, Sampler);
			Sampler = nullptr;
		}
	}
} // namespace gargantuan
