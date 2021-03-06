#include "context.hxx"
#include "transform.hxx"
#include "launch.hxx"
#include <cstdio>

template<int NT = 128, typename type_t>
[[using spirv: comp, local_size(NT), push]]
void saxpy(int count, type_t a, type_t* x, type_t* y) {
  int gid = glcomp_GlobalInvocationID.x;
  if(gid < count)
    y[gid] += a * x[gid];
}

int main() {
  context_t context;

  // Allocate test data storage.
  int count = 100;
  float* x = context.alloc_gpu<float>(count);
  float* y = context.alloc_gpu<float>(count);

  // Create a command buffer.
  cmd_buffer_t cmd_buffer(context);
  cmd_buffer.begin();
 
  // Initialize the data on the GPU using lambda closure syntax. This is 
  // better for embarrassingly parallel launches. The lambda is invoked once
  // for each index < count.
  vk_transform(count, cmd_buffer, [=](int index) {
    x[index] = index;
    y[index] = 2 * index + 1;
  });

  // Perform SAXPY with a chevron launch. This launches the compute shader on 
  // thread blocks. This is best for kernels requiring cooperative parallel
  // programming. You control the block size. The first chevron argument is 
  // the number of blocks, not the number of threads.
  float a = 3;
  int num_blocks = (count + 127) / 128;
  saxpy<<<num_blocks, cmd_buffer>>>(count, a, x, y);

  // Copy the data to host memory.
  float* host = context.alloc_cpu<float>(count);

  cmd_buffer.host_barrier();
  context.memcpy(cmd_buffer, host, y, sizeof(float) * count);

  // End and submit the command buffer.
  cmd_buffer.end();
  context.submit(cmd_buffer);

  // And wait for it to be done.
  vkQueueWaitIdle(context.queue);

  // Print our results.
  for(int i = 0; i < count; ++i)
    printf("%3d: %f\n", i, host[i]);

  context.free(x);
  context.free(y);
  context.free(host);

  return 0;
}

