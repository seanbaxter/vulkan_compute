#pragma once
#include "context.hxx"

namespace cvk {

template<typename... types_t>
struct tuple_t {
  types_t @(int...) ...;
};

template<typename type_t>
[[spirv::push]]
type_t shader_push;

template<typename push_t>
[[using spirv: comp, local_size(256)]]
void transform_shader() {
  int gid = glcomp_GlobalInvocationID.x;

  if(gid >= shader_push<push_t>._0)
    return;

  shader_push<push_t>._1(gid);
}

} // namespace cvk

// Chevron launch on a SPIR-V compute shader performs ADL lookup to find
// this symbol, and overload resolution to select this overload.
template<auto F, typename... params_t>
void spirv_compute_dispatch(int num_blocks, cmd_buffer_t& cmd_buffer, 
  params_t... params) {

  static_assert((... && std::is_trivially_copyable_v<params_t>));
  cvk::tuple_t<params_t...> storage { params... };

  cmd_buffer.context.dispatch_compute(
    cmd_buffer,
    @spirv(F),
    cmd_buffer.context.create_module(__spirv_data, __spirv_size),
    num_blocks,
    sizeof(storage),
    &storage
  );
}

template<typename func_t>
static void vk_transform(int count, cmd_buffer_t& cmd_buffer, 
  const func_t& func) {

  static_assert(std::is_trivially_copyable_v<func_t>);
  cvk::tuple_t<int, func_t> storage { count, func };

  static_assert(sizeof(storage) <= 128);

  cmd_buffer.context.dispatch_compute(
    cmd_buffer,
    @spirv(cvk::transform_shader<decltype(storage)>), 
    cmd_buffer.context.create_module(__spirv_data, __spirv_size),
    (count + 255) / 256,
    sizeof(storage),
    &storage
  );
}
