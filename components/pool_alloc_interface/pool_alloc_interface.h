#ifndef POOL_ALLOC_INTERFACE_H
#define POOL_ALLOC_INTERFACE_H




struct pool_alloc_interface{


    void* (*poolDrain)(struct pool_alloc_interface* self);
    /*The single pointer is not used  because the parameter then just becomes local copy of the pointer
    whereas we need to modify the actual pointer (set it to NULL after copying)*/

    int (*poolFill)(struct pool_alloc_interface* self,void** object);
};


typedef struct pool_alloc_interface pool_alloc_interface_t;















#endif