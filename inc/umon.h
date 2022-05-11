#ifndef UMON_H
#define UMON_H

#include "cache.h"
#include "champsim.h"

class META{
	public:
		uint8_t valid;
		uint64_t tag;

		META(){
			valid = 0;
			tag = -1;
		};

		META(BLOCK block){
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
			for(int i = 0; i < WAYS; i++){
				counter = 0;
			}
			for(uint32_t i = 0; i < SETS; i++){
				table[i] = new META[WAYS];
			}
		}

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
		
		bool access_block(BLOCK b){
			int s = get_set(b.address);
			if(s < SETS){
				int w = get_way(b.address, s);
				if(w != WAYS){
					counter[w]++;
					META n = table[s][0];
					for(int i = 0; i < w; i++){
						META temp = table[s][i+1];
						table[s][i+1] = n;
						n = temp;
					}
					table[s][0] = n;
					return true;
				}
				addMeta(b);
			}
			return false;
		};

	private:
		uint32_t find_victim(uint32_t s){
			for(int i = 0; i < WAYS; i++){
				if(!table[s][i].valid)	return i;
			}
			return WAYS-1;
		}

		void addMeta(BLOCK b){
			META m(b);
			int s = get_set(b.address);
			if(s < SETS){
				int w = find_victim(s);
				META n = table[s][0];
				for(int i = 0; i < w; i++){
					META temp = table[s][i+1];
					table[s][i+1] = n;
					n = temp;
				}
				table[s][0] = m;
			}
		}
};

#endif