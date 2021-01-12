#include "context.hxx"
#include "transform.hxx"
#include <cstdio>

int main() {
  // Initialize the Vulkan.
  context_t context;

  vk_transform(context, 256, [](int gid) {
    printf("thread = %3d\n", gid);
  });

  vkQueueWaitIdle(context.queue);

  return true;
}