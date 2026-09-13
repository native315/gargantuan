#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <string>

namespace gargantuan {
	static constexpr uint32_t CUBEMAP_FACE_COUNT = 6;

	struct GpuCubemap {
	  public:
		SDL_GPUTexture *Texture = nullptr;
		SDL_GPUSampler *Sampler = nullptr;

		uint32_t Width = 0;
		uint32_t Height = 0;

		uint32_t FaceBufferSize = 0;

		GpuCubemap(const char *ft, const char *bk, const char *dn, const char *up, const char *rt, const char *lf);

		SDL_GPUTransferBuffer *TransferBuffer = nullptr;

		SDL_GPUTexture *CreateTexture(SDL_GPUDevice *gpu);
		SDL_GPUSampler *CreateSampler(SDL_GPUDevice *gpu);
		SDL_GPUTransferBuffer *CreateTransferBuffer(SDL_GPUDevice *gpu);
		void DestroyTransferBuffer(SDL_GPUDevice *gpu);

		bool Upload(SDL_GPUDevice *gpu, SDL_GPUCopyPass *copyPass);
		void Destroy(SDL_GPUDevice *gpu);

		GpuCubemap(const GpuCubemap &) = delete;
		GpuCubemap &operator=(const GpuCubemap &) = delete;

	  private:
		std::array<std::string, CUBEMAP_FACE_COUNT> Urls;
	};
} // namespace gargantuan
