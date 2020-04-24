//
// Created by polizi on 20-4-23.
//

#include "object_pool.h"
#include "utils.hpp"

namespace polizi {

    int object_pool::offset_table[NUM_NODES_PER_CHUNK]{0};
    const uint8_t object_pool::chunk_data_size;
    const uint8_t object_pool::num_nodes_per_chunk;
    const uint8_t object_pool::free_node_offset;

    void *object_pool::allocate(){

        if(!free_node_head_){
            auto new_chunk_data = reinterpret_cast<chunk_data*>(operator new(chunk_size_));
            new_chunk_data->next_ = chunk_node_head_;
            chunk_node_head_ = new_chunk_data;
            free_node_head_ = byte_offset<free_node>(new_chunk_data, chunk_data_size);

            free_node_head_->index_ = 0;
            free_node_head_->next_ = nullptr;

            for(int index = 1; index  < num_nodes_per_chunk; ++index){
                auto new_free_node = byte_offset<free_node>(free_node_head_, static_cast<int>(free_node_size_));
                new_free_node->index_ = index;
                new_free_node->next_ = free_node_head_;
                free_node_head_ = new_free_node;
            }

        }

        auto return_node = free_node_head_;
        free_node_head_ = free_node_head_->next_;

        return reinterpret_cast<void*>(&return_node->next_);
    }

    void *object_pool::deallocate(void *ptr) {
        auto new_free_node = byte_offset<free_node>(ptr, -1 * free_node_offset);
        new_free_node->next_ = free_node_head_;
        free_node_head_ = new_free_node;
        return nullptr;
    }

    size_t object_pool::recycle() {
        size_t num_recycled{0};
        for(auto chunk_node = chunk_node_head_; chunk_node; chunk_node = chunk_node->next_){
            chunk_node->free_node_size_ = 0;
        }

        for(auto free_node = free_node_head_; free_node; free_node = free_node->next_){
            auto chunk_node = byte_offset<chunk_data>(free_node, offset_table[free_node->index_]);
            ++chunk_node->free_node_size_;
        }

        for(free_node** p_node = &free_node_head_; *p_node;){
            auto chunk_node = byte_offset<chunk_data>(*p_node, offset_table[(*p_node)->index_]);
            if(chunk_node->free_node_size_ == num_nodes_per_chunk){
                *p_node = (*p_node)->next_;
            }
            else{
                p_node = &((*p_node)->next_);
            }
        }

        for(chunk_data** p_chunk = &chunk_node_head_; *p_chunk;){
            if((*p_chunk)->free_node_size_ == num_nodes_per_chunk){
                auto delete_chunk = *p_chunk;
                *p_chunk = (*p_chunk)->next_;
                operator delete(delete_chunk);
                ++num_recycled;
            }
            else{
                p_chunk = &((*p_chunk)->next_);
            }
        }

        return num_recycled * chunk_size_;
    }

    void object_pool::init() {

        //set offset.
        offset_table[0] = -1 * chunk_data_size;
        for(int i = 1; i < num_nodes_per_chunk; ++i){
            offset_table[i] = offset_table[i-1] - static_cast<int>(free_node_size_);
        }

    }
}
