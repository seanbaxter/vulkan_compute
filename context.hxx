#pragma once
#include <map>
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

struct context_t {
  context_t();
  ~context_t();

  // Make it non-copyable.
  context_t(const context_t&) = delete;
  context_t& operator=(const context_t&) = delete;

  operator VkInstance() const noexcept { return instance; }
  operator VkDevice() const noexcept { return device; }
  operator VkPhysicalDevice() const noexcept { return physical_device; }

  VkInstance instance;
  
  VkPhysicalDevice physical_device;
  VkDevice device;
  
  uint32_t queue_index;
  VkQueue queue;

  VkCommandPool command_pool;

  VkPipelineCache pipeline_cache;

  VmaAllocator allocator;

  struct item_t {
    uint32_t size;
    uint32_t usage;
    VkBuffer buffer;
    VmaAllocation allocation;
  };
  std::map<void*, item_t> alloc_map;

  void* alloc(uint32_t size, uint32_t usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  void free(void* p);

  std::map<const char*, VkShaderModule> modules;
  VkShaderModule create_module(const char* data, size_t size);

  struct transform_t {
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    VkCommandBuffer cmd_buffer;
  };
  std::map<const char*, transform_t> transforms;

  void submit_transform(const char* name, VkShaderModule module, 
    int num_blocks, uint32_t push_size, const void* push_data, 
    bool barrier);
};
