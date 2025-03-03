
#include "pool_queue.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define         TIME_OUT        2   //millisecond

#define configTICK_RATE_HZ  CONFIG_FREERTOS_HZ


static const char* TAG="queue pool";



typedef struct queue_handle{
    QueueHandle_t handle;
    StaticQueue_t queue_meta_data;
    uint8_t* buff;
    size_t object_size;
}queue_handle_t;


struct pool_queue{

    queue_handle_t queue;
    pool_alloc_interface_t interface;
};

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})




int poolQueueGiveBack(pool_alloc_interface_t * self,void** object){

    

    if(self==NULL ||  object==NULL || *object==NULL)
        return -1;

    struct pool_queue* pool_queue=container_of(self,struct pool_queue,interface);

    if(pool_queue->queue.handle==NULL)
        return -1;

    //Copy/backup the caller pointer 
    void * temp=*object;
    if(xQueueSend(pool_queue->queue.handle,&temp,pdMS_TO_TICKS(TIME_OUT))==pdTRUE){
        //So that the user
        *object=NULL;
        return 0;
    }

    
    return -1;
}







void* poolQueueAllocate(pool_alloc_interface_t* self){

    if(self==NULL)
        return NULL;

    struct pool_queue* pool_queue=container_of(self,struct pool_queue,interface);

    if(pool_queue->queue.handle==NULL)
        return NULL;

    void * temp;
    if(xQueueReceive(pool_queue->queue.handle,&temp,pdMS_TO_TICKS(TIME_OUT))==pdTRUE)
        return temp;


    return NULL;
}






size_t poolQueueGetSize(int total_elements){
    size_t size=sizeof(pool_queue_t);

    /*The Queue will store the void pointers to the objects instead of actual object
        So a memory equal to total void pointers must be allocated because queue creates
        copy and does not reuse the pointers passed
    */
    size+=sizeof(void*)*total_elements;

    return size;
}


pool_alloc_interface_t* poolQueueCreate(pool_queue_t* self,int total_elements,size_t object_size){

    
    if(self==NULL || total_elements<=0 || object_size <=0)
        return NULL;

        
    //self->queue.buff=object_array;
    self->queue.buff = (uint8_t*)self + sizeof(pool_queue_t);
        
    self->queue.object_size=object_size;
    self->queue.handle=xQueueCreateStatic(total_elements,sizeof(void*),self->queue.buff,&self->queue.queue_meta_data);
    

    if(self->queue.handle==NULL)
        return NULL;

    self->interface.poolDrain=poolQueueAllocate;
    self->interface.poolFill=poolQueueGiveBack;

    return &self->interface;
}

