//
// Created by nick on 4/30/19.
//

#include "cache.h"
#include "memUnit.h"

#define NOWRITEALLOCATE 0
#define WRITEALLOCATE 1
#define NOVICTIM 0
#define VICTIM 1

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
        memUnit vic(4, 0, 1, wrAlloc, Bsize); // maybe wrAlloc should be
        // different
        this->victim = vic;
    }
};


void cache::execute(unsigned long int pc, char operation) {
    this->numberOfcommands++;
    int setL1 = L1.getSetFromPc(pc);
    int setL2 = L2.getSetFromPc(pc);
    victim.setfully();
    if (operation == 'w') { // writing to memory
        class ::LRU lru;
        if (L1.isTagExist(pc, lru, setL1)) {
            this->hitL1++;
            L1.updateDirty(lru, true);
            return;
        } else {// tag is not in L1
            if (L2.isTagExist(pc, lru, setL2)) {
                this->hitL2++;
                if (this->WrAlloc == WRITEALLOCATE) {
                    if (L1.isFull(setL1)) {// no free space
                        class ::LRU toFree = L1.popLRU(setL1);
                        if (L1.isDirty(toFree)) { //L1 is dirty
                            std::vector<unsigned int> tagToClear = L1.getTag
                                    (toFree);
                            class ::LRU tag = L2.findTag(tagToClear,setL2);
                            L2.updateDirty(tag, true);
                            L2.updateLRU(tag);
                            tagToClear = L2.getTag(lru);
                            L1.updateTag(tagToClear, toFree);
                            L1.updateLRU(toFree);
                            L1.updateDirty(toFree, true);
                            return;
                        } else { // L1 is not dirty
                            std::vector<unsigned int> tag = L2.getTag(lru);
                            L1.updateTag(tag, toFree);
                            L1.updateLRU(toFree);
                            L1.updateDirty(toFree, true);
                            return;
                        }
                    } else {
                        class ::LRU freeTag = L1.findFirstEmpty(setL1);
                        L1.updateMemory(pc, freeTag);
                        L1.updateDirty(freeTag, true);
                        return;
                    }
                } else {//not write allocate
                    L2.updateLRU(lru);
                    L2.updateDirty(lru, true);
                    return;
                }
            } else { //not in L2
                if (this->VicCache == VICTIM) { //Victim
                    bool foundInVic;
                    if (victim.isTagExistVictim(pc, lru)) {
                        victim.updateLRUVictim(lru);
                        foundInVic=true;
                    } else {// tag does not exist
                        foundInVic=false;
                        this->accessMem++;
                    }
                    if (this->WrAlloc == WRITEALLOCATE) {
                        if (L2.isFull(setL2)) {
                            if (L1.isFull(setL1)) {//L1+L2 is full
                                class ::LRU toFreeL2 = L2.popLRU(setL2);
                                class ::LRU toFreeL1 = L1.popLRU(setL1);
                                if (L1.isDirty(toFreeL1)) {
                                    //update l1 to l2
                                    std::vector<unsigned int> tagToClear = L1.getTag
                                            (toFreeL1);
                                    class ::LRU tag = L2.findTag(tagToClear,
                                            setL2);
                                    L2.updateDirty(tag, true);
                                    L2.updateLRU(tag);
                                }
                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, true);

                                if(!foundInVic) {
                                    std::vector<unsigned int> tagToClear = L2.getTag(
                                            toFreeL2);
                                    class ::LRU vic;
                                    if (victim.isFull(0)) {
                                        vic = victim.popLRU(0);
                                    } else {
                                        vic = victim.findFirstEmpty(0);
                                    }
                                    victim.updateRow(tagToClear, vic);
                                }

                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, true);
                            } else { //only L2 is full
                                class ::LRU toFreeL2 = L2.popLRU(setL2);
                                class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, true);

                                std::vector<unsigned int> tagToClear = L2
                                        .getTag(toFreeL2);
                                //update pc to l2
                                class::LRU vic;
                                if(victim.isFull(0)){
                                    vic = victim.popLRU(0);
                                } else{
                                    vic = victim.findFirstEmpty(0);
                                }

                                victim.updateRow(tagToClear,vic);

                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, true);

                            }
                        } else {
                            if (L1.isFull(setL1)) {//L1 is full, L2 is not full
                                class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                                class ::LRU toFreeL1 = L1.popLRU(setL1);
                                if (L1.isDirty(toFreeL1)) {
                                    //update l1 to l2
                                    std::vector<unsigned int> tagToClear = L1.getTag
                                            (toFreeL1);
                                    class ::LRU tag = L2.findTag(tagToClear,
                                            setL2);
                                    L2.updateDirty(tag, true);
                                    L2.updateLRU(tag);

                                    //update pc to l1
                                    L1.updateMemory(pc, toFreeL1);
                                    L1.updateDirty(toFreeL1, true);

                                    //update pc to l2
                                    L2.updateMemory(pc, toFreeL2);
                                    L2.updateDirty(toFreeL2, true);
                                }
                            } else { //both empty
                                class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                                class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, true);

                                //update pc to l2
                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, true);

                            }

                        }
                    } else {
                        return;
                    }
                } else { // no Victim = memory access
                    this->accessMem++;
                    if (this->WrAlloc == WRITEALLOCATE) {
                        //bring from memory to L2
                        if (L2.isFull(setL2)) {
                            if (L1.isFull(setL1)) {//L1+L2 is full
                                class ::LRU toFreeL2 = L2.popLRU(setL2);
                                class ::LRU toFreeL1 = L1.popLRU(setL1);
                                if (L1.isDirty(toFreeL1)) {
                                    //update l1 to l2
                                    std::vector<unsigned int> tagToClear = L1.getTag
                                            (toFreeL1);
                                    class ::LRU tag = L2.findTag(tagToClear,
                                            setL2);
                                    L2.updateDirty(tag, true);
                                    L2.updateLRU(tag);

                                    //update pc to l1
                                    L1.updateMemory(pc, toFreeL1);
                                    L1.updateDirty(toFreeL1, true);

                                    //update pc to l2
                                    L2.updateMemory(pc, toFreeL2);
                                    L2.updateDirty(toFreeL2, true);
                                }
                            } else { //only L2 is full
                                class ::LRU toFreeL2 = L2.popLRU(setL2);
                                class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, true);

                                //update pc to l2
                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, true);

                            }
                        } else {
                            if (L1.isFull(setL1)) {//L1 is full, L2 is not full
                                class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                                class ::LRU toFreeL1 = L1.popLRU(setL1);
                                if (L1.isDirty(toFreeL1)) {
                                    //update l1 to l2
                                    std::vector<unsigned int> tagToClear = L1.getTag
                                            (toFreeL1);
                                    class ::LRU tag = L2.findTag(tagToClear,
                                            setL2);
                                    L2.updateDirty(tag, true);
                                    L2.updateLRU(tag);

                                    //update pc to l1
                                    L1.updateMemory(pc, toFreeL1);
                                    L1.updateDirty(toFreeL1, true);

                                    //update pc to l2
                                    L2.updateMemory(pc, toFreeL2);
                                    L2.updateDirty(toFreeL2, true);
                                }
                            } else { //both empty
                                class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                                class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, true);

                                //update pc to l2
                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, true);

                            }

                        }
                    } else { // no Write allocate
                        return;
                    }
                }
            }
        }
    }else { //read
        class ::LRU lru;
        if (L1.isTagExist(pc, lru, setL1)) {
            hitL1++;
            return;
        } else {// tag is not in L1
            if (L2.isTagExist(pc, lru,setL2)) {
                this->hitL2++;
                if (L1.isFull(setL1)) {// no free space
                    class ::LRU toFree = L1.popLRU(setL1);
                    if (L1.isDirty(toFree)) { //L1 is dirty
                        std::vector<unsigned int> tagToClear = L1.getTag
                                (toFree);
                        class ::LRU tag = L2.findTag(tagToClear, setL2);
                        L2.updateDirty(tag, true);
                        L2.updateLRU(tag);
                        tagToClear = L2.getTag(lru);
                        L1.updateTag(tagToClear, toFree);
                        L1.updateLRU(toFree);
                        L1.updateDirty(toFree, false);
                        return;
                    } else { // L1 is not dirty
                        std::vector<unsigned int> tag = L2.getTag(lru);
                        L1.updateTag(tag, toFree);
                        L1.updateLRU(toFree);
                        L1.updateDirty(toFree, false);
                        return;
                    }
                } else { // L1 is not full
                    class ::LRU freeTag = L1.findFirstEmpty(setL1);
                    L1.updateMemory(pc, freeTag);
                    L1.updateDirty(freeTag, false);
                    return;
                }
            } else { //not in L2
                if (this->VicCache == VICTIM) { //Victim
                    bool foundInVic;
                    if (victim.isTagExistVictim(pc, lru)) {
                        victim.updateLRUVictim(lru);
                        foundInVic=true;
                    } else {// tag does not exist
                        foundInVic=false;
                        this->accessMem++;
                    }
                    if (L2.isFull(setL2)) {
                        if (L1.isFull(setL1)) {//L1+L2 is full
                            class ::LRU toFreeL2 = L2.popLRU(setL2);
                            class ::LRU toFreeL1 = L1.popLRU(setL1);
                            if (L1.isDirty(toFreeL1)) {
                                //update l1 to l2
                                std::vector<unsigned int> tagToClear = L1.getTag
                                        (toFreeL1);
                                class ::LRU tag = L2.findTag(tagToClear,setL2);
                                L2.updateDirty(tag, true);
                                L2.updateLRU(tag);
                            }

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2

                            if(!foundInVic) {
                                std::vector<unsigned int> tagToClear = L2.getTag(
                                        toFreeL2);
                                class ::LRU vic;
                                if (victim.isFull(0)) {
                                    vic = victim.popLRU(0);
                                } else {
                                    vic = victim.findFirstEmpty(0);
                                }
                                victim.updateRow(tagToClear, vic);
                            }

                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);

                        } else { //only L2 is full
                            class ::LRU toFreeL2 = L2.popLRU(setL2);
                            class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            std::vector<unsigned int> tagToClear = L2
                                    .getTag(toFreeL2);
                            //update pc to l2
                            class::LRU vic;
                            if(victim.isFull(0)){
                                vic = victim.popLRU(0);
                            } else{
                                vic = victim.findFirstEmpty(0);
                            }

                            victim.updateRow(tagToClear,vic);

                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);

                        }
                    } else {
                        if (L1.isFull(setL1)) {//L1 is full, L2 is not full
                            class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                            class ::LRU toFreeL1 = L1.popLRU(setL1);
                            if (L1.isDirty(toFreeL1)) {
                                //update l1 to l2
                                std::vector<unsigned int> tagToClear = L1.getTag
                                        (toFreeL1);
                                class ::LRU tag = L2.findTag(tagToClear,setL2);
                                L2.updateDirty(tag, true);
                                L2.updateLRU(tag);
                            }
                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2
                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);
                        } else { //both empty
                            class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                            class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2
                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);

                        }

                    }
                } else { // no Victim = memory access
                    this->accessMem++;
                    //bring from memory to L2
                    if (L2.isFull(setL2)) {
                        if (L1.isFull(setL1)) {//L1+L2 is full
                            class ::LRU toFreeL2 = L2.popLRU(setL2);
                            class ::LRU toFreeL1 = L1.popLRU(setL1);
                            if (L1.isDirty(toFreeL1)) {
                                //update l1 to l2
                                std::vector<unsigned int> tagToClear = L1.getTag
                                        (toFreeL1);
                                class ::LRU tag = L2.findTag(tagToClear,setL2);
                                L2.updateDirty(tag, true);
                                L2.updateLRU(tag);

                                //update pc to l1
                                L1.updateMemory(pc, toFreeL1);
                                L1.updateDirty(toFreeL1, false);

                                //update pc to l2
                                L2.updateMemory(pc, toFreeL2);
                                L2.updateDirty(toFreeL2, false);
                            }
                        } else { //only L2 is full
                            class ::LRU toFreeL2 = L2.popLRU(setL2);
                            class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2
                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);

                        }
                    } else {
                        if (L1.isFull(setL1)) {//L1 is full, L2 is not full
                            class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                            class ::LRU toFreeL1 = L1.popLRU(setL1);
                            if (L1.isDirty(toFreeL1)) {
                                //update l1 to l2
                                std::vector<unsigned int> tagToClear = L1.getTag
                                        (toFreeL1);
                                class ::LRU tag = L2.findTag(tagToClear,setL2);
                                L2.updateDirty(tag, true);
                                L2.updateLRU(tag);
                            }

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2
                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);
                        } else { //both empty
                            class ::LRU toFreeL2 = L2.findFirstEmpty(setL2);
                            class ::LRU toFreeL1 = L1.findFirstEmpty(setL1);

                            //update pc to l1
                            L1.updateMemory(pc, toFreeL1);
                            L1.updateDirty(toFreeL1, false);

                            //update pc to l2
                            L2.updateMemory(pc, toFreeL2);
                            L2.updateDirty(toFreeL2, false);

                        }
                    }
                }
            }
        }
    }
}
