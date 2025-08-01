#ifndef POOL_ALLOC_h
#define POOL_ALLOC_h



#define     ERR_POOL_MANAGER_BASE                       0
#define     ERR_POOL_MANAGER_INVALID_ARGS               (ERR_POOL_MANAGER_BASE-1);
#define     ERR_POOL_MANAGER_OBJECT_ALREADY_IN_LIST     (ERR_POOL_MANAGER_BASE-2);
#define     ERR_POOL_MANAGER_NODE_NOT_FOUND             (ERR_POOL_MANAGER_BASE-3);
#define     ERR_POOL_MANAGER_FREE_LIST_EMPTY            (ERR_POOL_MANAGER_BASE-4);

typedef struct pool_manager pool_manager_t;

void* poolAllocate(pool_manager_t* self);

//Problem, now node object need to be created, which is malloc
int poolDeAllocate(pool_manager_t* self,void* object);
size_t poolManagerGetObjectSize(int total_elements);

int poolManagerCreate(pool_manager_t* self,void* object_pool, size_t pool_size, int total_elements);



#endif