#include "gargantuan/render/GpuCubemap.hpp"
#include "gargantuan/render/RenderPass.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/render/Shader.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <memory>

namespace gargantuan {
	class SkyboxPass final : public RenderPass {
	  public:
		static constexpr const char *LABEL = "Skybox";

		struct alignas(16) WorldUniforms {
			glm::mat4 InverseViewProjection;
		};

		FileShader Shader{
			.VertexFilepath = GetShaderPath("skybox.vert"),
			.VertexUniformBufferCount = 1,
			.FragmentFilepath = GetShaderPath("skybox.frag"),
			.FragmentUniformBufferCount = 0,
			.FragmentSamplerCount = 1,
		};

		std::unique_ptr<GpuCubemap> Cubemap;

		SkyboxPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
			Shader.Init(gpu);

			SDL_GPUGraphicsPipelineCreateInfo info{};
			info.vertex_shader = Shader.VertexShader;
			info.fragment_shader = Shader.FragmentShader;

			info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
			info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
			info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
			info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

			info.depth_stencil_state.enable_depth_test = true;
			info.depth_stencil_state.enable_depth_write = false;
			info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;

			SDL_GPUColorTargetDescription colorTarget{};
			{
				colorTarget.format = swapchainFormat;
				colorTarget.blend_state.enable_blend = true;
				colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
				colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
				colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
				colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
			}
			info.target_info.num_color_targets = 1;
			info.target_info.color_target_descriptions = &colorTarget;

			info.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
			info.target_info.has_depth_stencil_target = true;

			Pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &info);
		};

		SDL_GPURenderPass *Draw(SDL_GPUDevice *gpu, FrameContext &context) override {
			if (!context.Lighting->FindSky()) return nullptr;
			auto skybox = context.Lighting->FindSky();

			bool CubemapFailed = false;

			if (!Cubemap && !CubemapFailed) {
				Cubemap = std::make_unique<GpuCubemap>(
					skybox->GetSkyboxFt().c_str(),
					skybox->GetSkyboxBk().c_str(),
					skybox->GetSkyboxDn().c_str(),
					skybox->GetSkyboxUp().c_str(),
					skybox->GetSkyboxRt().c_str(),
					skybox->GetSkyboxLf().c_str()
				);
				auto copy = SDL_BeginGPUCopyPass(context.Commands);
				bool ok = Cubemap->Upload(gpu, copy);
				SDL_EndGPUCopyPass(copy);
				if (!ok) {
					Cubemap.reset();
					CubemapFailed = true;
					return nullptr;
				}
			}
			if (!Cubemap) return nullptr;

			SDL_GPUColorTargetInfo colorTarget = {
				.texture = context.SwapchainTexture,
				.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
				.load_op = SDL_GPU_LOADOP_LOAD,
				.store_op = SDL_GPU_STOREOP_STORE,
			};

			SDL_GPUDepthStencilTargetInfo depthTarget = {
				.texture = context.DepthTexture,
				.clear_depth = 1.0f,
				.load_op = SDL_GPU_LOADOP_LOAD,
				.store_op = SDL_GPU_STOREOP_DONT_CARE,
				.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
				.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
			};

			SDL_GPUTextureSamplerBinding skyBinding{
				.texture = Cubemap->Texture,
				.sampler = Cubemap->Sampler,
			};

			auto view = glm::mat4(glm::mat3(context.Camera->GetViewMatrix())); // strip translation
			WorldUniforms u{.InverseViewProjection = glm::inverse(context.Camera->GetProjectionMatrix() * view)};
			SDL_PushGPUVertexUniformData(context.Commands, 0, &u, sizeof(u));

			auto pass = SDL_BeginGPURenderPass(context.Commands, &colorTarget, 1, &depthTarget);
			SDL_BindGPUGraphicsPipeline(pass, Pipeline);
			SDL_BindGPUFragmentSamplers(pass, 0, &skyBinding, 1);

			SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);

			return pass;
		};
	};

	std::unique_ptr<RenderPass> CreateSkyboxPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
		return std::make_unique<SkyboxPass>(gpu, swapchainFormat);
	}
} // namespace gargantuan
