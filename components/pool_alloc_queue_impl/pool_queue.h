#ifndef POOL_QUEUE_H
#define POOL_QUEUE_H

#include "stddef.h"
#include "pool_alloc_interface.h"



//Queue stores pointers




typedef struct pool_queue pool_queue_t;


//size_t poolQueueGetObjectSize();

size_t poolQueueGetSize(int total_elements);

/// @brief Creates a pool queue and return the interface pointer which is a member. This way encapsulation
///         is ensured as client does not need to know the internals of implementation
/// @param self 
/// @param total_elements 
/// @param object_size 
/// @return 
pool_alloc_interface_t* poolQueueCreate(pool_queue_t* self,int total_elements,size_t object_size);



#endif