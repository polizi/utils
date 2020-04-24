//
// Created by polizi on 20-4-23.
//

#ifndef UTILS_OBJECT_POOL_H
#define UTILS_OBJECT_POOL_H

#ifndef NUM_NODES_PER_CHUNK
#define NUM_NODES_PER_CHUNK 128
#endif

#include <iostream>
#include <cstddef>


namespace polizi{

    class object_pool {
    public:
        explicit object_pool(size_t node_size)
            : node_size_(node_size),
              free_node_size_(free_node_offset + std::max(node_size, sizeof(free_node*))),
              chunk_size_(chunk_data_size + free_node_size_ * num_nodes_per_chunk),
              free_node_head_(nullptr),
              chunk_node_head_(nullptr){

            init();

        };
        ~object_pool(){
            while(chunk_node_head_){
                auto new_chunk_node_head = chunk_node_head_;
                chunk_node_head_ = chunk_node_head_->next_;
                operator delete(new_chunk_node_head);
            }
        }

        void* allocate();
        void* deallocate(void* ptr);
        size_t recycle();

    private:
        void init();

    private:
        //考虑到字节对齐，选择使用int32_t.
        struct chunk_data{
            int32_t free_node_size_;
            chunk_data* next_;
        };

        struct free_node{
            int32_t index_;
            free_node* next_;
        };


        static int offset_table[NUM_NODES_PER_CHUNK];
        static const uint8_t chunk_data_size = sizeof(chunk_data);
        static const uint8_t num_nodes_per_chunk = NUM_NODES_PER_CHUNK;
        static const uint8_t free_node_offset = static_cast<uint8_t >(offsetof(free_node, next_));

        const size_t node_size_;
        const size_t free_node_size_;
        const size_t chunk_size_;

        free_node* free_node_head_;
        chunk_data* chunk_node_head_;

    };

}




#endif //UTILS_OBJECT_POOL_H
