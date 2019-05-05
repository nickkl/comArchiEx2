//
// Created by nick on 4/30/19.
//

#include "cache.h"
#include "memUnit.h"

#define NOWRITEALLOCATE 0
#define WRITEALLOCATE 1
#define NOVICTIM 0
#define VICTIM 1


double L1MissRate = 0;
double L2MissRate = 0;
double avgAccTime = 0;


cache::cache(unsigned int memCyc, unsigned int Bsize, unsigned int L1size,
             unsigned int L2size, unsigned int L1Assoc, unsigned int L2Assoc,
             unsigned int L1Cyc, unsigned int L2Cyc, unsigned int wrAlloc,
             unsigned int VicCache) {
    this->MemCyc = memCyc;
    this->BSize = Bsize;
    this->L1Size = L1size;
    this->L2Size = L2size;
    this->L1Assoc = L1Assoc;
    this->L2Assoc = L2Assoc;
    this->L1Cyc = L1Cyc;
    this->L2Cyc = L2Cyc;
    this->WrAlloc = wrAlloc;
    this->VicCache = VicCache;

    memUnit l1(L1size, L1Assoc, L1Cyc, wrAlloc, Bsize);
    this->L1 = l1;
    memUnit l2(L2size, L2Assoc, L2Cyc, wrAlloc, Bsize);
    this->L2 = l2;
    if (VicCache == 1) {
        memUnit vic(4 * Bsize, 1, 1, wrAlloc, Bsize); // maybe wrAlloc should be
        // different
        this->victim = vic;
    }
};


void cache::execute(unsigned long int pc, char operation) {
    this->numberOfcommands++;
//    if (operation == 'r') {
//        if (L1.isTagExist(pc)) {
//            this->hitL1++;
//            return;
//        } else{
//            if(L2.isTagExist(pc)){
//
//            }
//        }
//    }
    if(operation=='w'){ // writing to memory
        class::LRU lru;
        if(L1.isTagExist(pc, lru)){
            L1.updateDirty(lru, true);
            return;
        } else{
            if(this->WrAlloc == NOWRITEALLOCATE){
                //try to write in L2
                return;
            } else{

            }
        } // tag is not in L1
    }else{ // reading from memory
        class::LRU lru;
        if(L1.isTagExist(pc, lru)){
            return;
        } else{
            if(L2.isTagExist(pc,lru)){
                //write to L1
                //update L1 LRU
                //L2 LRU should be OK
                return;
            } else { //trying to read from victim
                if(this->VicCache == VICTIM){

                } else{
                    this->accessMem++;
                    return;
                }
            }

        }
    }
}