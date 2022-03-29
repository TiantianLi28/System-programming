//
//  my_malloc.c
//  Lab1: Malloc
//
//THIS  CODE  IS  OUR  OWN  WORK,  IT  WAS  WRITTEN  WITHOUT  CONSULTING  A  TUTOR  OR  CODE  WRITTEN  BY 
//OTHER STUDENTS OUTSIDE OF OUR TEAM. 
//    - Tiantian Li

#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

MyErrorNo my_errno=MYNOERROR;
FreeListNode head = NULL;
void insert_node(FreeListNode node);
void remove_node(FreeListNode node);
FreeListNode find_node(uint32_t sz);

void *my_malloc(uint32_t size)
{   
    void * chunk_head;
    FreeListNode newNode;
    
    if (size==0){
        return NULL;
    }
    //requested size+ overhead
    size+=CHUNKHEADERSIZE;
    //min size = 16
    if (size<16){
        size=16;
    }
    if (size%8!=0){
        size+=8-size%8;
    }
    //store actual node size
    uint32_t node_size;
    //find if enough space availabe in freelist
    FreeListNode node=find_node(size);
    //if there is enough space
    if (node!=NULL){
        //record the actual size of node found
        node_size=node->size;
        //returning chunk's head at node position
        chunk_head=&(*node);
        //set first four byte to be the chunk size
        *((uint32_t*)chunk_head)=size;
        
        //set magic number
        chunk_head+=sizeof(uint32_t);
        *((uint32_t*) chunk_head)=1234;
        //return part of chunk after header
        chunk_head+=sizeof(uint32_t);
        //if more memory, add to free list
        if(node_size-size>16){
            newNode=(FreeListNode)(chunk_head+size);
            newNode->size=node_size-size;
            newNode->flink=NULL;
            insert_node(newNode);
        }
        return chunk_head;
    }
    //if there is no availabe space in freelist
    else{
        void * oldHeap=sbrk(0);
        if (size <= 8192){
            if(sbrk(8192)==-1){
                my_errno=MYENOMEM;
                return NULL;
            }
            *((uint32_t*)oldHeap)=size;
            //set magic number
            oldHeap+=sizeof(uint32_t);
            *((uint32_t*)oldHeap)=1234;
            //Now oldheap points to 8 byte after actual start
            oldHeap+=sizeof(uint32_t);
            //if chunk too big, add remaining space to free list
            if(8192-size>16){
                newNode=(FreeListNode) (oldHeap+size-CHUNKHEADERSIZE);
                newNode->size=8192-size;
                newNode->flink=NULL;
                insert_node(newNode);
            }
            return oldHeap;
        }
        //if requested size larger than 8192
        else{
            if(sbrk(size)==-1){
                my_errno=MYENOMEM;
                return NULL;
            }
            *((uint32_t*)oldHeap)=size;
            //set magic number
            oldHeap+=sizeof(uint32_t);
            *(uint32_t*)oldHeap=1234;
            oldHeap+=sizeof(uint32_t);
            return oldHeap;
        }
    }
}

void insert_node(FreeListNode node){
    int a=0;
    FreeListNode temp=NULL;
    if(head==NULL){
        head=node;
    }
    else if(head>node){
        node->flink=head;
        head=node;
    }
    else if(head->flink==NULL){
        head->flink=node;
        node->flink=NULL;
    }else{
        temp=head;
        while(temp!=NULL&& a==0){
            if(temp->flink==NULL){
                temp->flink=node;
                a=1;
            }
            else if(temp<node && temp->flink>=node){
                node->flink=temp->flink;
                temp->flink=node;
                a=1;
            }
            else if(temp->flink<node){
                temp=temp->flink;
            }
        }
    }
 
}

void remove_node(FreeListNode node){
    FreeListNode temp=head;
    if (node==head){
        head=head->flink;
    }
    while(temp->flink!=NULL){
        if(temp->flink==node){  
            temp->flink=node->flink;
            break;
        }
        temp=temp->flink;
    }
}

FreeListNode find_node(uint32_t sz){
    if (head==NULL){
        return NULL;
    }else{
    //hold current node
    FreeListNode temp=head;
    //hold output node
    FreeListNode out=NULL;
    //if head matches size
    if(head->size>sz){
        //if head size too big, cut in half and output first half
        if(head->size-sz>16){
            out=head;
            //set new head
            FreeListNode newHead=(void*)head+sz;
            newHead->size=head->size-sz;
            newHead->flink=head->flink;
            head=newHead;
            //update out size
            out->size=sz;
            return out;
        }
        else{
            //save old head as output
            out=head;
            //remove head node from list
            remove_node(head);
            return out;
        }
    }
    //if head does not match size
    while(temp->flink != NULL){
        if (temp->flink->size>sz){
            out=temp->flink;
            remove_node(temp->flink);
            return out;
        }
        else{
            temp=temp->flink;
        }
    }}
    return NULL;
}
      
void my_free(void *ptr)
{ 
    uint32_t * magic;
    if (ptr==NULL){
        my_errno=MYBADFREEPTR;
        return;
    }
    magic=ptr-sizeof(uint32_t);
    if (*magic!=1234){
        my_errno=MYBADFREEPTR;
        return;
    }
    FreeListNode newNode;
    newNode=ptr-CHUNKHEADERSIZE;
    newNode->size=*(uint32_t*)(ptr-CHUNKHEADERSIZE);
    newNode->flink=NULL;
    insert_node(newNode);
    return;
}

FreeListNode free_list_begin()
{
    return head;
}

void coalesce_free_list()
{
    FreeListNode curNode=head;
    void * temp;
    if (head==NULL){
        return;
    }
    if (head->flink==NULL){
        return;
    }
    while(curNode!=NULL){
        temp=(void*)curNode+curNode->size;
        if (temp==curNode->flink){
            curNode->size+=curNode->flink->size;
            curNode->flink=curNode->flink->flink;
            
        }
        else{
        curNode=curNode->flink;}
    }
    return;
}
