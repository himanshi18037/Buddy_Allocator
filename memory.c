#define _GNU_SOURCE

#include<stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include "memory.h"

#define PAGE_SIZE 4096
// array[9]


struct node {
	struct node* next;
	struct node* prev;	
};

struct node* bucketlist[9];


struct metadata {
int allocation_size;
long long free_bytes_available;
}; 

static void *alloc_from_ram(size_t size)
{
	assert((size % PAGE_SIZE) == 0 && "size must be multiples of 4096");
	void* base = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (base == MAP_FAILED)
	{
		printf("Unable to allocate RAM space\n");
		exit(0);
	}
	return base;
}


static void free_ram(void *addr, size_t size)
{
	munmap(addr, size);
}


int togetsize(int size){
	if(size==16)
		return 0;
	if(size==32)
		return 1;
	if(size==64)
		return 2;
	if(size==128)
		return 3;
	if(size==256)
		return 4;
	if(size==512)
		return 5;
	if(size==1024)
		return 6;
	if(size==2048)
		return 7;
	if(size==4080)
		return 8;
	else
		return -1;
	
}

void myfree(void *ptr)
{ 
uintptr_t ddr = (uintptr_t) ptr;
void *temp = (void *) (ddr - ddr%PAGE_SIZE);
struct metadata* page=(struct metadata*) temp; 
int size=page->allocation_size;
page->free_bytes_available+=size;
struct node* ptr1=(struct node*) ptr;
ptr1->next=NULL;
ptr1->prev=NULL;
int index=togetsize(size);
   

if(size>4080){
	free_ram(temp,size);
}
if(size<=4080)
{
   
    if(bucketlist[index]==NULL)
    {
    	bucketlist[index]=ptr1;	
    }
    else
	{
	ptr1->next=bucketlist[index];
	bucketlist[index]->prev=ptr1;
	bucketlist[index]= ptr1;
}
	if(page->free_bytes_available==4080)
	{

		struct node *pre=bucketlist[index]->prev;
		struct node *curr=bucketlist[index];
		while(curr!=NULL){
			uintptr_t ddrr = (uintptr_t) curr;
			void *nodep = (void *) (ddrr - ddrr%PAGE_SIZE);
			struct metadata* nodem=(struct metadata*) nodep; 
			if(nodem==page)
			{
				if(pre==NULL)
				{
					bucketlist[index]=bucketlist[index]->next;
					curr->next=NULL;
					if(bucketlist[index]!=NULL)
					{
						bucketlist[index]->prev=NULL;
					}
					curr=bucketlist[index];
					pre=NULL;

					continue;
				}

				else {
					pre->next=curr->next;
					if(curr->next!=NULL){
						curr->next->prev=pre;
					}
					curr->prev=NULL;
					curr=curr->next;
					continue;

				}	
				

			}
			
				pre=curr;
				curr=curr->next;
			
				
			}
			free_ram(temp,4096);
		}
		
		
		
	}
}

  


int conversion(int size){
	if(size<=16)
		return 16;
	if(size<=32)
		return 32;
	if(size<=64)
		return 64;
	if(size<=128)
		return 128;
	if(size<=256)
		return 256;
	if(size<=512)
		return 512;
	if(size<=1024)
		return 1024;
	if(size<=2048)
		return 2048;
	if(size<=4080)
		return 4080;
	if(size>4080)
	{
		int multiple=size/4096;
		if(size<4096)
		{
			multiple+=1;
		}
		multiple+=1;
		return (multiple*4096);
	}
}



void *mymalloc(size_t sie)
{ 
int size =conversion(sie);
int index=togetsize(size);

void* ptr;
if(size>4080)
{
	ptr=alloc_from_ram(size);
	struct metadata* page=ptr;
	page->free_bytes_available=size-16;
	page->allocation_size=size;
	page->free_bytes_available-=size;
	return (ptr+16);
}
if(bucketlist[index]==NULL)
{
	void* ptr=alloc_from_ram(PAGE_SIZE);
	struct metadata* page=ptr;
	page->allocation_size=size;
	page->free_bytes_available=PAGE_SIZE-16;
	struct node* temp=(struct node*)(ptr+16);
	bucketlist[index]=temp;
	int xx=(PAGE_SIZE-16-size)/(size);
	for(int i=1;i<=xx;i++)
	{
	   struct node* inter=(struct node*)(ptr+(size*(i))+16);
	   inter->next=bucketlist[index];
	   bucketlist[index]->prev=inter;
	   bucketlist[index]=inter;
	   
	 } 

	}
		
void* ptr1=bucketlist[index];

bucketlist[index]=bucketlist[index]->next;
if(bucketlist[index]!=NULL){
	bucketlist[index]->prev=NULL;
}
uintptr_t ddr = (uintptr_t) ptr1;
void *temp = (void *) (ddr - ddr%PAGE_SIZE);
struct metadata* page=(struct metadata*) temp;
page->free_bytes_available=page->free_bytes_available-size;
return (ptr1);

 }
	


	
	
	




int main(){

	// printf("%d\n",conversion(16) );
	void * ptr1=mymalloc(1024);

	
	return 0;
}