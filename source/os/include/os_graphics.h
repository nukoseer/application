#ifndef H_OS_GRAPHICS_H

#include "os_graphics_types.h"

OSGraphicsBuffer os_graphics_create_buffer(OSGraphicsBufferDesc* buffer_desc);
OSGraphicsSampler os_graphics_create_sampler(OSGraphicsSamplerDesc* sampler_desc);
OSGraphicsTexture os_graphics_create_texture(OSGraphicsTextureDesc* texture_desc);
OSGraphicsShader os_graphics_create_shader(OSGraphicsShaderDesc* shader_desc);
OSGraphicsPipeline os_graphics_create_pipeline(OSGraphicsPipelineDesc* pipeline_desc);
void os_graphics_apply_pipeline(OSGraphicsPipeline pipeline);
void os_graphics_apply_bindings(OSGraphicsBindings* bindings);
void os_graphics_apply_uniforms(u32 shader_stage_index, u32 uniform_index, const void* data, memory_size size);
void os_graphics_draw(uptr handle_pointer, u32 index, u32 count);

#define H_OS_GRAPHICS_H
#endif
