#include <stddef.h>
#include "pool_alloc.h"
#include "unity.h"
#include "esp_log.h"

static const char* tag="test pool manager";


typedef struct some_struct{
    int a;
    char b;
    float c;

}some_struct_t;

#define TOTAL_ELEMENTS  5

some_struct_t array[TOTAL_ELEMENTS]={
                            {.a=1,
                            .b='h',
                            .c=4.5},
                            {.a=2,
                            .b='i',
                            .c=5.5},
                            {.a=3,
                            .b='j',
                            .c=6.5},
                            {.a=4,
                            .b='k',
                            .c=7.5},
                            {.a=5,
                            .b='l',
                            .c=8.5},
};

                        


static size_t size;
static uint8_t buff[44];

static pool_manager_t* manager;



//Setup will run before any test. The state is returned to inital after each test
//Thus state is not ,maintained, so new object must be created before each test
void setUp() {
    ESP_LOGI(tag, "Setting up test environment...");
    size=poolManagerGetObjectSize(3);
    ESP_LOGI(tag,"size %d",size);
    
    manager = (pool_manager_t*) buff;
    int ret=poolManagerCreate(manager,(void*)&array,sizeof(some_struct_t)*3,3);
    ESP_LOGI(tag,"create %d",ret);
}

TEST_CASE("PoolManager Create Object","[Unit Test: PoolManager]"){

    size=poolManagerGetObjectSize(3);
    ESP_LOGI(tag,"size %d",size);
    //manager=(pool_manager_t*) malloc(size);
    manager=(pool_manager_t*) buff;
    int ret=poolManagerCreate(manager,(void*)&array,sizeof(some_struct_t)*3,3);
    ESP_LOGI(tag,"create %d",ret);
    
}


static some_struct_t* object1;
static some_struct_t* object2;
static some_struct_t* object3;
static some_struct_t* object4;

//Allocate the object
static some_struct_t* allocateObject(){
    manager=(pool_manager_t*) buff;
    some_struct_t* object=(some_struct_t*)poolAllocate(manager);
    return object;
}


static int deallocateObject(some_struct_t* object){
    manager=(pool_manager_t*) buff;
    int ret=poolDeAllocate(manager,(void*)object);
    return ret;
}


//All allocation

TEST_CASE("PoolManager Allocate 4 , Deallocate 1","[Unit Test: PoolManager]"){
    
    object1=allocateObject();
    if (object1==NULL)
        ESP_LOGI(tag,"failed 1");
    else
        ESP_LOGI(tag,"true 1");
    object2=allocateObject();
    if (object2==NULL)
        ESP_LOGI(tag,"failed 2");
    else
        ESP_LOGI(tag,"true 2");
    object3=allocateObject();
    if (object3==NULL)
        ESP_LOGI(tag,"failed 3");
    else
        ESP_LOGI(tag,"true 3");
    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed 4");
    else
        ESP_LOGI(tag,"true 4");

    int ret= deallocateObject(object2);
    ESP_LOGI(tag,"free ret %d",ret);

    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed after  free");
    else
        ESP_LOGI(tag,"true after free");
  

}



TEST_CASE("PoolManager Allocate 4 , Deallocate 2, Alloc2","[Unit Test: PoolManager]"){
    
    object1=allocateObject();
    if (object1==NULL)
        ESP_LOGI(tag,"failed 1");
    else
        ESP_LOGI(tag,"true 1");
    object2=allocateObject();
    if (object2==NULL)
        ESP_LOGI(tag,"failed 2");
    else
        ESP_LOGI(tag,"true 2");
    object3=allocateObject();
    if (object3==NULL)
        ESP_LOGI(tag,"failed 3");
    else
        ESP_LOGI(tag,"true 3");
    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed 4");
    else
        ESP_LOGI(tag,"true 4");

    int ret= deallocateObject(object2);
    ESP_LOGI(tag,"free ret %d",ret);

    ret= deallocateObject(object3);
    ESP_LOGI(tag,"free ret %d",ret);

    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed after  free");
    else
        ESP_LOGI(tag,"true after free");
  
    object3=allocateObject();
    if (object1==NULL)
        ESP_LOGI(tag,"failed after  free");
    else
        ESP_LOGI(tag,"true after free");

}


TEST_CASE("PoolManager Deallocate same object twice","[Unit Test: PoolManager]"){
    
    object1=allocateObject();
    if (object1==NULL)
        ESP_LOGI(tag,"failed 1");
    else
        ESP_LOGI(tag,"true 1");
    object2=allocateObject();
    if (object2==NULL)
        ESP_LOGI(tag,"failed 2");
    else
        ESP_LOGI(tag,"true 2");
    object3=allocateObject();
    if (object3==NULL)
        ESP_LOGI(tag,"failed 3");
    else
        ESP_LOGI(tag,"true 3");
    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed 4");
    else
        ESP_LOGI(tag,"true 4");

    int ret= deallocateObject(object2);
    ESP_LOGI(tag,"free ret %d",ret);

  
    ret= deallocateObject(object2);
    ESP_LOGI(tag,"free ret %d",ret);

    object4=allocateObject();
    if (object4==NULL)
        ESP_LOGI(tag,"failed after  free 1");
    else
        ESP_LOGI(tag,"true after free 1");
  
    object3=allocateObject();
    if (object3==NULL)
        ESP_LOGI(tag,"failed after  free 2");
    else
        ESP_LOGI(tag,"true after free 2");

}

