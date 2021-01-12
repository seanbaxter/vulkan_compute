#include "context.hxx"
#include "transform.hxx"
#include <cstdio>

int main() {
  context_t context;

  int count = 32;
  vec3* vectors = context.alloc<vec3>(count);

  vk_transform(context, count, [=](int gid) {
    // Initialize the device memory.
    vectors[gid] = 3 * gid + vec3(0, 1, 2);
  });

  // vk_transform(context, count, [=](int gid) {
  //   // Load the data and print.
  //   vec3 v = vectors[gid];
  //   printf("%3d: (%f %f %f)\n", gid, v.x, v.y, v.z);
  // });
  vkQueueWaitIdle(context.queue);

  context.free(vectors);

  return true;
}