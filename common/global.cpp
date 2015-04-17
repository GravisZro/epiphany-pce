#include "global.h"

volatile e_mutex_t        mutex;                           // groupe lock mutex
volatile e_barrier_t      core_sync_barriers    [_Ncores]; // barriers array
         e_barrier_t*     tgt_core_sync_barriers[_Ncores]; // barriers array
//         e_dma_desc_t     dma_descriptor[3];               // descriptor structure for DMA


void EndOfClockCycle(void)
{
    e_barrier(core_sync_barriers, tgt_core_sync_barriers);
}

/*
template<typename T> T* memzero(T* data)
{
  char* d = reinterpret_cast<char*>(data);
  for(size_t i = 0; i < sizeof(T); ++i)
    d[i] = 0;

  return data;
}
*/
