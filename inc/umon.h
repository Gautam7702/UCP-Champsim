#ifndef UMON_H
#define UMON_H

#include "cache.h"
#include "champsim.h"

/*
	G15:
	META class defines the smallest unit of the ATD.
	It is like the BLOCK class defined in block.h
*/
class META{
	public:
		uint8_t lru;		//	G15:	lru position of the block
		bool valid;			//	G15:	whether it is a valid block
		uint64_t addr;		//	G15:	Address of the block

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

/*
	G15:
	Defines the class of a Utility monitor.
	It has an ATD, counters and info reagarding SETS, WAYS, accesses etc.
*/
class UMON{
	public:
		META** table;
		uint32_t SETS, WAYS;
		uint64_t* counter;
		uint64_t accesses;

		UMON(){
			SETS = LLC_SET/32;		//	G15:	Every 33rd set of LLC will be used in DSS
			WAYS = LLC_WAY;			//	G15:	Every set will have same number of ways as LLC
			accesses = 0;
			counter = new uint64_t[WAYS];
			table = new META*[SETS];
			for(int i = 0; i < WAYS; i++){
				counter[i] = 0;
			}
			for(uint32_t i = 0; i < SETS; i++){
				table[i] = new META[WAYS];
			}
		}
		
		//	G15:	Used to access a block. If hit then update the counter else add the new addr to the ATD
		bool access_block(uint64_t addr){
			int s = get_set(addr);
			accesses++;
			if(s%32 == 0){
				s /= 32;
				int w = get_way(addr, s);
				if(w != WAYS){
					counter[table[s][w].lru]++;
					uint8_t prev = table[s][w].lru;
					for(int i = 0; i < WAYS; i++){
						if(table[s][i].valid && table[s][i].lru < prev){
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
			delete[] counter;
		}

	private:

		//	G15:	Returns the set the block maps to in an LLC
		uint32_t get_set(uint64_t address){
		    return (uint32_t) (address & ((1 << lg2(LLC_SET)) - 1)); 
		};

		//	G15:	Returns the way in which the block with given address is present. Returns WAYS if block not present
		uint32_t get_way(uint64_t address, uint32_t set){
			uint32_t way;
		    for (way = 0; way < WAYS; way++) {
		        if (table[set][way].valid && (table[set][way].addr == address))	break;
		    }

		    return way;
		};

		//	G15:	Returns the way which will be evicted according to LRU policy
		uint32_t find_victim(uint32_t s){
			for(int i = 0; i < WAYS; i++){
				if(table[s][i].valid == false)	return i;
			}
			for(int i = 0; i < WAYS; i++){
				if(table[s][i].lru == WAYS-1)	return i;
			}
		}

		//	G15:	Adds the block with given address to the ATD
		void addMeta(uint64_t addr){
			int s = get_set(addr);
			if(s%32 == 0){
				s /= 32;
				int w = find_victim(s);
				for(int i = 0; i < WAYS; i++){
					if(table[s][i].valid)	table[s][i].lru++;
				}
				table[s][w].lru = 0;
				table[s][w].addr = addr;
				table[s][w].valid = true;
			}
		}
};

#endif