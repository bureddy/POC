#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>

#define BUF_SIZE (4*1024*1024)

        char *
alloc_workbuf(int size)
{
        char *ptr;

        /* allocate some memory */
        ptr = malloc(size);

        /* return NULL on failure */
        if (ptr == NULL) {
                printf("memmory allocation failed");
                return NULL;
        }

        /* lock this buffer into RAM */
        if (mlock(ptr, size)) {
                printf("Mlock failed :%m\n");
                free(ptr);
                return NULL;
        }
        return ptr;
}

        void
free_workbuf(char *ptr, int size)
{
        /* unlock the address range */
        munlock(ptr, size);

        /* free the memory */
        free(ptr);
}

int main()
{

        int i;

        for (i=0; i < 100000; i++) {
                if(alloc_workbuf(BUF_SIZE) == NULL)
                        return -1;
                printf("Done iter:%d size: %d\n MB", i, (i+1)*4);


        }
}

