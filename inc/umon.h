#ifndef UMON_H
#define UMON_H
#include "champsim.h"
#include "cache.h"
#include "dram_controller.h"



class ATD:public CACHE{
    public:
    SPECIAL_BLOCK **block;
    ATD(string v1, uint32_t v2, int v3, uint32_t v4, uint32_t v5, uint32_t v6, uint32_t v7, uint32_t v8) 
         {
            NAME = v1;
            NUM_SET = v2;
            NUM_WAY = v3;
            NUM_LINE = v4;
            WQ_SIZE = v5;
            RQ_SIZE = v6;
            PQ_SIZE = v7;
            MSHR_SIZE = v8;
        LATENCY = 0;

        // cache block
        block = new SPECIAL_BLOCK* [NUM_SET];
        for (uint32_t i=0; i<NUM_SET; i++) {
            block[i] = new SPECIAL_BLOCK[NUM_WAY]; 

            for (uint32_t j=0; j<NUM_WAY; j++) {
                block[i][j].lru = j;
            }
        }

        for (uint32_t i=0; i<NUM_CPUS; i++) {
            upper_level_icache[i] = NULL;
            upper_level_dcache[i] = NULL;

            for (uint32_t j=0; j<NUM_TYPES; j++) {
                sim_access[i][j] = 0;
                sim_hit[i][j] = 0;
                sim_miss[i][j] = 0;
                roi_access[i][j] = 0;
                roi_hit[i][j] = 0;
                roi_miss[i][j] = 0;
            }
        }

	total_miss_latency = 0;

        lower_level = NULL;
        extra_interface = NULL;
        fill_level = -1;
        MAX_READ = 1;
        MAX_FILL = 1;

        pf_requested = 0;
        pf_issued = 0;
        pf_useful = 0;
        pf_useless = 0;
        pf_fill = 0;
    };
};
class UMON{
    public:
    ATD atd{"ATD",32,16,32*16,LLC_WQ_SIZE, LLC_RQ_SIZE, LLC_PQ_SIZE, LLC_MSHR_SIZE};
    UMON(); 
};

extern UMON umon[NUM_CPUS];

#endif