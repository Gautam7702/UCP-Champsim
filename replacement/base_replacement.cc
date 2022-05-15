#include "cache.h"
#include "ooo_cpu.h"

uint32_t CACHE::find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    // baseline LRU replacement policy for other caches 
    return lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type); 
}

void CACHE::update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    if (type == WRITEBACK) {
        if (hit) // wrietback hit does not update LRU state
            return;
    }

    return lru_update(set, way);
}

uint32_t CACHE::lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    uint32_t way = 0;
     
    for (way=0; way<NUM_WAY; way++) {
        if (block[set][way].valid == false) {
            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });

            break;
        }
    }

    // LRU victim
    if (way == NUM_WAY) {
        for (way=0; way<NUM_WAY; way++) {
            if (block[set][way].lru == NUM_WAY-1) {

                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });

                break;
            }
        }
    }

    if (way == NUM_WAY) {
        cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
        assert(0);
    }
    return way;
}

uint32_t CACHE::llc_lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    uint32_t way1 = 0;
    for (way1=0; way1<NUM_WAY; way1++) {
        if (!current_set[way1].valid) {
            return way1;
        }
    }
    int lru_victim[NUM_CPUS],lru_victim_value[NUM_CPUS];
    int cpu_way_count[NUM_CPUS];
    for(int i=0;i<NUM_CPUS;i++){
            lru_victim_value[i]=0;
            cpu_way_count[i]=0;
            lru_victim[i] = -1;
    }

    // LRU victim
    int way;
    for (way=0; way<NUM_WAY; way++) {
            int cur_cpu = current_set[way].cpu;
            cpu_way_count[cur_cpu]++;
            if(lru_victim_value[cur_cpu]<=current_set[way].lru)
                {
                    lru_victim_value[cur_cpu] = current_set[way].lru;
                    lru_victim[cur_cpu] = way; 
                }
        }

    if(cpu_way_count[cpu] < max_way[cpu])
    {
        for(int j=0;j<NUM_CPUS;j++)
            {
                
                if(cpu_way_count[j] >max_way[j])
                    {
                        way1  = lru_victim[j];
                        break;
                    }
            }
    }
    else    
    {
        way1 = lru_victim[cpu];
    }
    return way1;
}



void CACHE::lru_update(uint32_t set, uint32_t way)
{
    // update lru replacement state
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    block[set][way].lru = 0; // promote to the MRU position
}

void CACHE::llc_lru_update(uint32_t set, uint32_t way,int cpu)
{
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block[set][i].cpu == cpu && block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    block[set][way].lru = 0;
    
}

void CACHE::replacement_final_stats()
{

}

#ifdef NO_CRC2_COMPILE
void InitReplacementState()
{
    
}

uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    return 0;
}

void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    
}

void PrintStats_Heartbeat()
{
    
}

void PrintStats()
{

}
#endif
