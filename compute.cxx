#include "context.hxx"
#include "transform.hxx"
#include <cstdio>

constexpr int div_up(int x, int y) {
  return (x + y - 1) / y;
}

[[using spirv: comp, local_size(128), push]]
void saxpy(int count, float a, const float* x, float* y) {
  int gid = glcomp_GlobalInvocationID.x;
 // if(gid < count) { 
 //   x[gid] = gid;
 //   y[gid] = sin(.01f * gid);
 // }
}

int main() {
  context_t context;

  // Create a command buffer.
  cmd_buffer_t cmd_buffer(context);

  int count = 1000;
  float a = 3;
  float* x = context.alloc_gpu<float>(count);
  float* y = context.alloc_gpu<float>(count);
  
  // Initialize the data on the GPU. Use launch chevron syntax.
  int num_blocks = div_up(count, 128);
  saxpy<<<num_blocks, cmd_buffer>>>(count, a, x, y);

  // Compute the SAXPY with transform syntax. Use a closure to capture the 
  // arguments.
  vk_transform(cmd_buffer, count, [=](int index) {
    y[index] += a * x[index];
  });

  // Copy the data to host memory.
  float* host = context.alloc_cpu<float>(count);
  context.memcpy(cmd_buffer, host, y, sizeof(float) * count);

  // Submite the command buffer.

  // And wait for it to be done.
  vkQueueWaitIdle(context.queue);

  printf("%3d: %f\n", @range(count), host[:])...;

  return true;
}