#pragma once
#include "context.hxx"

namespace cvk {

template<typename func_t>
[[using spirv: comp, local_size(256), push]]
void transform_shader(int count, func_t func) {
  int gid = glcomp_GlobalInvocationID.x;

  if(gid >= count)
    return;

  func(gid);
}

} // namespace cvk

template<typename func_t>
static void vk_transform(int count, cmd_buffer_t& cmd_buffer, 
  func_t func) {

  int num_blocks = (count + 255) / 256;
  cvk::transform_shader<<<num_blocks, cmd_buffer>>>(count, func);
}
