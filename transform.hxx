#pragma once
#include "context.hxx"

template<typename type_t>
[[spirv::push]]
type_t shader_push;

template<typename closure_t>
struct transform_push_t {
  int count;
  closure_t closure;
};

template<typename push_t>
[[using spirv: comp, local_size(256)]]
void transform_shader() {
  int gid = glcomp_GlobalInvocationID.x;

  if(gid >= shader_push<push_t>.count)
    return;

  shader_push<push_t>.closure(gid);
}

template<typename func_t>
static void vk_transform(context_t& context, int count, const func_t& func,
  bool barrier = true) {

  typedef transform_push_t<func_t> push_t;

  static_assert(sizeof(push_t) <= 128);
  static_assert(std::is_trivially_copyable_v<push_t>);

  push_t push { count, func };
  context.submit_transform(
    @spirv(transform_shader<push_t>), 
    context.create_module(__spirv_data, __spirv_size),
    (count + 255) / 256,
    sizeof(push_t),
    &push,
    barrier
  );
}

