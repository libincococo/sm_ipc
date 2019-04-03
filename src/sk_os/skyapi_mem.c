#include <stdarg.h>
#include <stdio.h> 
#include <stdlib.h>
#include <memory.h>

#include "sk_os.h"
#include "sk_tbx.h"

void * sk_mem_malloc(U32 size)
{
	if(size==0)
	{
		SK_ERROR(("Param Err"));
		return NULL;
	}

	return (void*)malloc(size);
}

void * sk_mem_realloc(void* p_mem, U32 size)
{
	if(size==0)
	{
		SK_ERROR(("Param Err"));
		return NULL;
	}

	return (void*)realloc(p_mem,size);
}

void * sk_mem_calloc(const U32 num, const  U32 size)
{
	void * ptr;

	if((num==0)||(size==0))
	{
		SK_ERROR(("Param Err"));
		return NULL;
	}
	
	//ptr = malloc(num*size);
	ptr = calloc(num,size);   //use c fun 
	memset(ptr, 0, num * size);
	return ptr;
}

void sk_mem_free(void * p_mem)
{
	if(p_mem == NULL)
	{
		return;
	}
	
	free(p_mem);
	p_mem = NULL;
}



