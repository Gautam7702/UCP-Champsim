#ifndef UMON_H
#define UMON_H

#include "cache.h"
#include "champsim.h"

class META{
	public:
		uint8_t lru;
		bool valid;
		uint64_t addr;

		META(){
			lru = 0;
			valid = false;
			addr = 0;
		};

		META(uint64_t _addr){
			lru = 0;
			valid = true;
			addr = _addr;
		};
};

class UMON{
	public:
		META** table;
		uint32_t SETS, WAYS;
		uint64_t* counter;

		UMON(){
			SETS = 32;
			WAYS = LLC_WAY;
			counter = new uint64_t[WAYS];
			table = new META*[SETS];
			for(int i = 0; i < WAYS; i++){
				counter[i] = 0;
			}
			for(uint32_t i = 0; i < SETS; i++){
				table[i] = new META[WAYS];
			}
		}
		
		bool access_block(uint64_t addr){
			int s = get_set(addr);
			if(s < SETS){
				int w = get_way(addr, s);
				if(w != WAYS){
					counter[table[s][w].lru]++;
					uint8_t prev = table[s][w].lru;
					for(int i = 0; i < WAYS; i++){
						if(table[s][w].valid && table[s][i].lru < prev){
							table[s][i].lru++;
						}
					}
					table[s][w].lru = 0;
					return true;
				}
				addMeta(addr);
			}
			return false;
		};

		~UMON(){
			for(int i = 0; i < SETS; i++){
				delete[] table[i];
			}
			delete[] table;
		}

	private:

		uint32_t get_set(uint64_t address){
		    return (uint32_t) (address & ((1 << lg2(LLC_SET)) - 1)); 
		};

		uint32_t get_way(uint64_t address, uint32_t set){
			uint32_t way;
		    for (way = 0; way < WAYS; way++) {
		        if (table[set][way].valid && (table[set][way].addr == address))	break;
		    }

		    return way;
		};

		uint32_t find_victim(uint32_t s){
			for(int i = 0; i < WAYS; i++){
				if(!table[s][i].valid)	return i;
				if(table[s][i].lru == WAYS-1)	return i;
			}
		}

		void addMeta(uint64_t addr){
			META m(addr);
			int s = get_set(addr);
			if(s < SETS){
				int w = find_victim(s);
				for(int i = 0; i < WAYS; i++){
					table[s][i].lru++;
				}
				table[s][w] = m;
			}
		}
};

#endif