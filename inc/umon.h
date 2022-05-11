#ifndef UMON_H
#define UMON_H

#include "cache.h"
#include "champsim.h"

class META{
	public:
		uint8_t valid, lru;
		uint64_t tag;

		META(){
			lru = 0;
			valid = 0;
			tag = -1;
		};

		META(BLOCK block){
			lru = 0;
			valid = block.valid;
			tag = block.tag;
		};
};

class UMON{
	public:
		META** table;
		uint64_t SETS, WAYS;
		uint64_t* counter;

		UMON(){
			SETS = 32;
			WAYS = LLC_WAY;
			counter = new uint64_t[WAYS];
			table = new META*[SETS];
			for(int i = 0; i < WAYS; i++){
				counter = 0;
			}
			for(uint32_t i = 0; i < SETS; i++){
				table[i] = new META[WAYS];
			}
		}
		
		bool access_block(BLOCK b){
			int s = get_set(b.address);
			if(s < SETS){
				int w = get_way(b.address, s);
				if(w != WAYS){
					counter[w]++;
					uint8_t prev = table[s][w].lru;
					for(int i = 0; i < WAYS; i++){
						if(table[s][i].lru < prev){
							table[s][i].lru++;
						}
					}
					table[s][w].lru = 0;
					return true;
				}
				addMeta(b);
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
		        if (table[set][way].valid && (table[set][way].tag == address))	break;
		    }

		    return way;
		};

		uint32_t find_victim(uint32_t s){
			for(int i = 0; i < WAYS; i++){
				if(!table[s][i].valid)	return i;
				if(table[s][i].lru == WAYS-1)	return i;
			}
		}

		void addMeta(BLOCK b){
			META m(b);
			int s = get_set(b.address);
			if(s < SETS){
				int w = find_victim(s);
				table[s][w] = m;
			}
		}
};

#endif