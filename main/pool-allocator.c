#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "pool_queue.h"
#include "seg_sysview.h"

//DElay set such that both tasks also collide as well as it is easy to get id out of them just by left shift
#define     TASK1_DELAY             16   //ms
#define     TASK2_DELAY             32  //ms


#define     TASK1_POOL_ALLOC_START          0
#define     TASK1_POOL_ALLOC_END            1
#define     TASK1_POOL_DEALLOC_START        2
#define     TASK1_POOL_DEALLOC_END          3
#define     TASK2_POOL_ALLOC_START          4
#define     TASK2_POOL_ALLOC_END            5
#define     TASK2_POOL_DEALLOC_START        6
#define     TASK2_POOL_DEALLOC_END          7



#define     ALLOC_EVENT_START           0
#define     ALLOC_EVENT_END             1
#define     DEALLOC_EVENT_START         2
#define     DEALLOC_EVENT_END           3

static int taskArgToID(int delay){

    //0 for task 1 delay and 1 for task 2 delay    
    return ((delay>>4)-1);

}

static int getEventID(int task_id,int event_id){

    

    return((task_id<<2)+event_id);
}


static const char* TAG="test alloc";

typedef struct some_struct{
    int a;
    char b;
    float c;
}some_struct_t;

static pool_alloc_interface_t* pool;
static sysview_interface_t* sysview_obj;

static void task_testAlloc(void* args){
    int delay=(int) args;
    int ret=0;
    some_struct_t* a[6];
    int task_id=taskArgToID(delay);
    //ESP_LOGI(TAG,"task id %d",task_id);
    while(1){
        uint8_t i=0;
        int ret=0;

        for(i=0;i<6;i++){
            sysview_obj->userSendData(sysview_obj,getEventID(task_id,ALLOC_EVENT_START),i);
            a[i]=(some_struct_t*)pool->poolDrain(pool);
            //Cannot send some value from the struct bcz it can be null
            if(a[i]==NULL){
                sysview_obj->userSendData(sysview_obj,getEventID(task_id,ALLOC_EVENT_END),9999);
                break;
            }
            else{
                sysview_obj->userSendData(sysview_obj,getEventID(task_id,ALLOC_EVENT_END),a[i]->a);
            }
            
            vTaskDelay(pdMS_TO_TICKS(delay));
            
            //ESP_LOGI(TAG,"task id %d, mem  %d, char val %d",task_id, i,a[i]->a);
        
        }

        //here <=
        for(uint8_t j=0;j<i;j++){

            sysview_obj->userSendData(sysview_obj,getEventID(task_id,DEALLOC_EVENT_START),j);
            ret=pool->poolFill(pool,a[j]);
            sysview_obj->userSendData(sysview_obj,getEventID(task_id,DEALLOC_EVENT_END),ret);
            a[j]=NULL;                          //This is now the responsibilty of the user to set to NULL after returning
            //ESP_LOGI(TAG,"task id %d count %d, return %d",task_id,j,ret);
            vTaskDelay(pdMS_TO_TICKS(delay));
        }


    

    }



}



void app_main(void)
{

    //creating a pool. Declared static so that it remain till life time of program bcz main exits after creating tasks, and it goes out of scope
    static some_struct_t s[]={{1,'a',1.1},{2,'b',2.2},{3,'c',3.3},{4,'d',4.4},{5,'e',5.5},{6,'f',6.6}};
    

    //The total elements in the pool are set using Kconfig
    pool=poolQueueCreate();

    //filling it with users
    ESP_ERROR_CHECK(pool==NULL);

    sysview_config_t config;

    config.module_name="test_pool";
    config.total_events=4;

    sysview_obj=sysviewCreate(&config);

    ESP_ERROR_CHECK(sysview_obj==NULL);

    uint8_t length=sizeof(s)/sizeof(some_struct_t);

    int ret=0;
    for(uint8_t i=0;i<length;i++){
        ret=pool->poolFill(pool,&s[i]);
       ESP_LOGI(TAG,"ret %d address %p",ret,&s[i]);
    }

    /*
    some_struct_t* test= (some_struct_t*)pool->poolDrain(pool);
    ESP_LOGI(TAG,"sucess %d address %p",test==NULL,test);
    ESP_LOGI(TAG,"test a %d, b %c, d %f",test->a,test->b,test->c);

    pool->poolFill(pool,(void*)test);
    test=NULL;          //It is responsibilty of user to set the pointer to NULL after returning
    */
    //create  two tasks, which will keep on allocating and deallocating after different delays
    xTaskCreate(task_testAlloc,"allocdealloc",configMINIMAL_STACK_SIZE,(void*)TASK1_DELAY,5,NULL);
    xTaskCreate(task_testAlloc,"allocdealloc",configMINIMAL_STACK_SIZE,(void*)TASK2_DELAY,5,NULL);

}
