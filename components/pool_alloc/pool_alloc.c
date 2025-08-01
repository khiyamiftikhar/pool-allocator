

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "pool_alloc.h"


static const char* TAG="Pool Alloc";

typedef struct node{
    void* data;             //Single object of the pool
    struct node* next;
}node_t;


typedef struct free_list{
    node_t* head;
    node_t* tail;
    node_t* node_list;       //List of all nodes
    
}free_list_t;


struct pool_manager{
    free_list_t list;
    void* object_array_base;             //Pointer to data object pool
    size_t object_size;
};


/// @brief Check if list is empty or not
/// @param self 
/// @return 0 if not empty, 1 if empty
static int freeListEmtpy(free_list_t* self){

    if(self->head==NULL/* && self->tail==NULL*/)
        return ERR_POOL_MANAGER_FREE_LIST_EMPTY;
    
    return 0;

}




/// @brief Assign the data member to the node. The nodes are alread created in the create manager method
/// @param node 
/// @param data     It is one object from the object pool
/// @return 
static int nodeInit(node_t* node,void* data){

    if(node==NULL || data == NULL)
        return ERR_POOL_MANAGER_INVALID_ARGS;
    
    node->data=data;
    return 0;
}



/// @brief Add node to the free list
/// @param self
/// @return Returns the node whose objeect is to be allocated 
static node_t* freeListGiveNodeFromHead(free_list_t* self){

    if(self==NULL)
        return NULL;

    node_t* temp=self->head;

    //head->next is tail for the last element, because tail is the last element
    if(freeListEmtpy(self)){
        ESP_LOGI(TAG, "cant provide, list empty");
        return NULL;
    }
    
    if(self->head==NULL)
        ESP_LOGI(TAG,"head is NULL");

    
    self->head=self->head->next;

    /*
    //If head has reached NULL, then list is empty, set tail to NULL also
    if(self->head==NULL)
        self->tail==NULL;
    */

    temp->next=NULL;

    return temp;
}





/// @brief Add The node whose data member i.e object is freed from use by the user code
/// @param self
/// @param node 
/// @return 
int freeListAddToHead(free_list_t* self, node_t* node){

    if(self==NULL || node==NULL)
        return ERR_POOL_MANAGER_INVALID_ARGS;

    //node_t* temp=self->head;

    //if list is completely empty
    if(freeListEmtpy(self)){
        self->head=node;
        self->head->next=NULL;
        self->tail=node;
        self->tail->next=NULL;
    }
    
    else{
        node->next=self->head;
        self->head=node;
    }
   
    return 0;
}




static int freeListNodeSearch(free_list_t* self,node_t* node){

    int index=0;
    if(self==NULL || node==NULL)
        return ERR_POOL_MANAGER_INVALID_ARGS;

    if(freeListEmtpy(self))
    //This error code must be <0, because >=0 means object found
        return ERR_POOL_MANAGER_FREE_LIST_EMPTY;

    node_t* temp=self->head;

    while(temp!=NULL){
        //If they both  point to the same location
        if(temp->data==node->data)
            return index;

        temp=temp->next;
        index++;
    }

    //Node not found
    return ERR_POOL_MANAGER_NODE_NOT_FOUND;
}


/// @brief Gets each node array and object array from the self pointer, and assigns node data member
////       (including head and tail) to one object from the pool. The connects all the nodes (next ptr)
/// @param self 
/// @param object_pool   Data is the complete object pool pointer, i.e the 0 index void* pointer 
/// @param pool_size     Size of complete pool
/// @param total_elements Total object elements in the pool array
/// @param  
/// @return 
static int freeListInitalize(free_list_t* self,void* object_pool,size_t pool_size, int total_elements){

    

    if(self==NULL || object_pool==NULL || pool_size<=0 || total_elements<=0)
        return -3;
    
    int ret=0;
    int object_size=pool_size/total_elements;
    int total_nodes=total_elements-2;           //Excluding head and tail, mem for remaining was allocated

    //ret=nodeInit(self->head,(void*)object_pool); //zeroth element

    if(ret!=0)
        return ret;



    char* next_object_address;
    uint8_t node_counter=0;
    //The loop starts from one by bassing 1st (head) and ends before tail
    for(uint8_t i=0;i<(total_elements);i++){

        next_object_address=(char*)object_pool + (object_size*i);

        ret=nodeInit(&self->node_list[i],(void*) next_object_address);
        if(ret!=0)
            return ret;
        
        if(i==(total_elements-1))
            self->node_list[i].next=NULL;
        else
            self->node_list[i].next=&self->node_list[i+1];
   
    }


    return 0;

}






/// @brief User will call this to  get one object from the pool, which is removed from free list
///         and provided
/// @param self 
/// @return 
void* poolAllocate(pool_manager_t* self){
    if(self==NULL)
        return NULL;

    node_t* node=freeListGiveNodeFromHead(&self->list);

    if(node==NULL)
        return NULL;
    
    int index=((uint8_t*) node - (uint8_t*)self->list.node_list)/sizeof(node_t);
    ESP_LOGI(TAG,"item removed %d",index);

    

    return (void*)node->data;
}



/// @brief The object index inside the pool determine to know the node index. Then that node is
///         inserted to the freelist
/// @param self 
/// @param object 
/// @return 
int poolDeAllocate(pool_manager_t* self,void* object){
    
    if(self==NULL || object==NULL)
        return ERR_POOL_MANAGER_INVALID_ARGS;

    
    ESP_LOGI(TAG," current object address %p",object);
    ESP_LOGI(TAG," base object address %p",self->object_array_base);

    int index=((uint8_t*) object - (uint8_t*)self->object_array_base)/self->object_size;

    //Check if node is already in the freelist
    int result=freeListNodeSearch(&self->list,&self->list.node_list[index]);
    ESP_LOGI(TAG,"index %d",index);
    if(result>=0)
        return ERR_POOL_MANAGER_OBJECT_ALREADY_IN_LIST;

    

    node_t* node =  &self->list.node_list[index];

    if(node==NULL)
        return ERR_POOL_MANAGER_INVALID_ARGS;
    
    int ret=freeListAddToHead(&self->list,node);
    return ret;
}


/// @brief Calculate and supply the pool manager object size. it comprises of pool manager struct size
///         plus the size for node list. But not complete node list equal to total elements, but
///         subtracting the 2 for head and tail nodes as they are not included and are declared
///         separately.
/// @param total_elements 
/// @return size in bytes
size_t poolManagerGetObjectSize(int total_elements){

    if(total_elements<=0)
        return -1;
    
    //2 is subtracted because head and tail are also nodes
    size_t size=sizeof(pool_manager_t)+sizeof(node_t)*(total_elements);

    ESP_LOGI(TAG,"pool manager size %d",sizeof(pool_manager_t));
    ESP_LOGI(TAG,"node size %d",sizeof(node_t));
    ESP_LOGI(TAG,"total elements %d",total_elements);


    return size;
}



/// @brief The self pointer is provied by user
/// @param self 
/// @param object_pool 
/// @param pool_size 
/// @param total_elements 
/// @return 
int poolManagerCreate(pool_manager_t* self,void* object_pool, size_t pool_size, int total_elements){

    int ret=0;
    if(self==NULL || object_pool==NULL || pool_size <=0 || total_elements<=0)
        return -2;


    self->object_array_base=object_pool;
    self->object_size=pool_size/total_elements;

    ESP_LOGI(TAG,"object size %d",self->object_size);
    
    //Here self+1 means one object bytes later i.e sizeof(self)
    ESP_LOGI(TAG,"buff start address %p",self);
    ESP_LOGI(TAG,"End of struc manager and node_list array start %p",self+1);
    ESP_LOGI(TAG,"Node list end %p",&self->list.node_list[total_elements-3]);
    ESP_LOGI(TAG,"head address %p",&self->list.node_list[total_elements-3]+1);
    ESP_LOGI(TAG,"tail address %p",&self->list.node_list[total_elements-3]+2);

    self->list.node_list=(node_t*)(self+1);
    

    //Head is assigned to the first element of the node list
    self->list.head=self->list.node_list;
    //Tail is assigned to last element of node array
    self->list.tail=self->list.head+(total_elements-1);


    //set all the elements zero
    memset(self->list.node_list,0,sizeof(node_t)*total_elements);
    

    ret=freeListInitalize(&self->list,object_pool,pool_size,total_elements);

    return ret;

}

