#include <stdlib.h>
#include <stdio.h>
#include <infiniband/verbs.h>
#include <string.h>

#define ITERS 10000
#define SIZE 2*1024*1024

int main (int argc, char **argv)
{
   int i = 0, j = 0;
   struct ibv_pd *pd = NULL;
   struct ibv_context *context = NULL;
   struct ibv_device **dev_list = NULL;
   void *buf_list[ITERS];
   struct ibv_mr *reg_list[ITERS];
   int num_devices;

   if (argc < 2) {
           printf("usage : %s <dev_name>\n", argv[0]);
           return -1;
   }

   dev_list = ibv_get_device_list(&num_devices);
   if (dev_list == NULL) {
        fprintf(stderr, "get_device_list failed \n");
        return -1;
   }

   if ( num_devices<= 0) {
        fprintf(stderr, "no devices fount!! \n");
        return -1;
   }

   for (j =0 ; j < num_devices; j++)
   {
        printf("%d %s %s \n", j, ibv_get_device_name(dev_list[j]), argv[1]);
           if (strcmp(ibv_get_device_name(dev_list[j]),argv[1]) == 0) {
                   break;
           }
   }

   if (j == num_devices)  {
           printf("device not found \n");
           return -1;
   }

   printf("testing on device:%s\n", dev_list[j]);

   context = ibv_open_device(dev_list[j]);
   if (context == NULL) {
           fprintf(stderr, "ibv_open_device failed \n");
           return -1;
   }

   pd = ibv_alloc_pd(context);
   if (pd == NULL) {
        fprintf(stderr, "ibv_alloc_pd failed \n");
        return -1;
   }

   for(j=0; j<ITERS; j++) {
       buf_list[j] = malloc(SIZE);
       if (buf_list[j] == NULL) {
            fprintf(stderr, " mem allocation failed after: %d MB\n", j * 2);
            return -1;
       }
       memset(buf_list[j], 0, SIZE);

       fprintf(stderr, "iteration: %d \n", j);
       reg_list[j] = ibv_reg_mr(pd, buf_list[j], SIZE,
                IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE |
                IBV_ACCESS_REMOTE_READ);
       if (reg_list[j] == NULL) {
           fprintf(stderr, "registration failed  after: %d MB\n", j *2);
           return -1;
       }
   }

   for(j=0; j<ITERS; j++) {
       ibv_dereg_mr(reg_list[j]);

       free(buf_list[j]);
   }

   ibv_dealloc_pd(pd);

   ibv_close_device(context);

   ibv_free_device_list(dev_list);

   return 0;
}
