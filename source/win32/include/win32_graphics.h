#ifndef H_WIN32_GRAPHICS_H

#include "os_graphics_types.h"

OSGraphicsBuffer win32_graphics_create_buffer(OSGraphicsBufferDesc* buffer_desc);
OSGraphicsSampler win32_graphics_create_sampler(OSGraphicsSamplerDesc* sampler_desc);
OSGraphicsTexture win32_graphics_create_texture(OSGraphicsTextureDesc* texture_desc);
OSGraphicsShader win32_graphics_create_shader(OSGraphicsShaderDesc* shader_desc);
OSGraphicsPipeline win32_graphics_create_pipeline(OSGraphicsPipelineDesc* pipeline_desc);
void win32_graphics_apply_pipeline(OSGraphicsPipeline pipeline);
void win32_graphics_apply_bindings(OSGraphicsBindings* bindings);
void win32_graphics_apply_uniforms(u32 shader_stage_index, u32 uniform_index, const void* data, memory_size size);
void win32_graphics_draw(uptr handle_pointer, u32 index, u32 count);
uptr win32_graphics_init(uptr handle_pointer);

#define H_WIN32_GRAPHICS_H
#endif
