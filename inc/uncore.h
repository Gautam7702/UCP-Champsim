#ifndef UNCORE_H
#define UNCORE_H

#include "champsim.h"
#include "cache.h"
#include "dram_controller.h"
//#include "drc_controller.h"

//#define DRC_MSHR_SIZE 48

// uncore
class UNCORE {
  public:

    // LLC
    CACHE LLC{"LLC", LLC_SET, LLC_WAY, LLC_SET*LLC_WAY, LLC_WQ_SIZE, LLC_RQ_SIZE, LLC_PQ_SIZE, LLC_MSHR_SIZE};
    // int COUNTER[16] = {};
    // for(int i=0;i<32;i++){
    //   for(int j=0;j<16;j++){
    //     if()
    //   }
    // }




    // DRAM
    MEMORY_CONTROLLER DRAM{"DRAM"}; 

    UNCORE();
};

extern UNCORE uncore;

#endif
