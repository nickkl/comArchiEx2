//
// Created by nick on 5/3/19.
//

#include <tgmath.h>
#include "memUnit.h"

memUnit::memUnit(unsigned int size, unsigned int ways, unsigned int cyc,
                 unsigned int wr, unsigned int block) {
    this->size =size;
    this->ways =ways;
    this->cycle =cyc;
    this->writeAllocate = wr;
    this->blockSize = block;

    unsigned int realLSize = (int)pow(2,size);
    unsigned int realWays = (int)pow(2,ways);
    unsigned int realBlock = (int)pow(2, block);
    unsigned int numberOfRowsPerWay = realLSize/(realWays*realBlock);

    this->realBlock = realBlock;
    this->realRows = numberOfRowsPerWay;
    this->realSize = realLSize;
    this->realWays = realWays;


    //tags part
    std::vector<unsigned int > tag(32-block,0);
    std::vector<std::vector<unsigned int>> tags(numberOfRowsPerWay, tag);
    std::vector<std::vector<std::vector<unsigned int > >> temp_tag(realWays,
                                                                    tags);
    this->tags = temp_tag;

    //dirty
    std::vector<bool > dirty(numberOfRowsPerWay, false);
    std::vector<std::vector<bool >> tempDirty(realWays, dirty);
    this->dirty = tempDirty;

    std::vector<bool > valid(numberOfRowsPerWay, false);
    std::vector<std::vector<bool >> tempValid(realWays, valid);
    this->valid = tempValid;

    //    data part
//    std::vector<unsigned int > row(realBlock, 0);
//    std::vector<std::vector<unsigned int>> blocks(numberOfRowsPerWay, row);
//    std::vector<std::vector<std::vector<unsigned int > >> temp_data(realWays,
//            blocks);
//    this->data = temp_data;

}



///This function takes a binary vector (a vector with the values 0 or 1)
/// and convert it to a decimal number (select[0] is the lsb).
int convertBinaryVectorToDecimalNumber(std::vector<unsigned> &vec) {
    int dec = 0;
    int base = 1;
    for (unsigned i = 0; i < vec.size(); ++i) {
        dec += vec[i] * base;
        base *= 2;
    }
    return dec;
}


///This function takes in a pc and an empty vector, will fill the vector the
/// first tagSize bits of the pc (starting from the second bit).
void pcToTag(unsigned long int pc, std::vector<unsigned> &tag, unsigned int
offset) {
    for (unsigned int i = offset; i < tag.size(); ++i) {
        int mask = 1 << (i);
        int masked_n = pc & mask;
        int thebit = masked_n >> (i);
        tag[i-offset] = thebit;
    }
}


bool isTagEqual(std::vector<unsigned> &tag, std::vector<unsigned> &pc){
    for (int i = 0; i < tag.size(); ++i) {
        if(tag[i]!=pc[i]){
            return false;
        }
    }
    return true;
}


//return true if the tag is exist and is valid
//updates the LRU
bool memUnit::isTagExist(unsigned long int pc, class::LRU& lru) {
    this->access++;
    std::vector<unsigned > pcTag(32-this->blockSize, 0);
    pcToTag(pc,pcTag, this->blockSize);
    for (int i = 0; i < this->realWays; ++i) {
        for (int j = 0; j < realRows; ++j) {
            if(isTagEqual(this->tags[i][j],pcTag))
                if(this->valid[i][j]) {
                    class::LRU recent(i,j);
                    lru = recent;
                    this->LRU.remove(recent);
                    this->LRU.push_back(recent);//if tag exist and valid
                            // return true

                    return true;
                }
                else                  //if tag exist and invalid return false
                    return false;
        }
    }
    return false; //tag does not exist
}

bool memUnit::isFull() {
    for (int i = 0; i < this->realWays; ++i) {
        for (int j = 0; j < this->realRows; ++j) {
            if(!this->valid[i][j]){
                return false;
            }
        }
    }
    return true;
}

class::LRU memUnit::findFirstEmpty() {
    class::LRU lru;
    for (int i = 0; i < this->realWays; ++i) {
        for (int j = 0; j < this->realRows; ++j) {
            if(!this->valid[i][j]){
                lru.way = i;
                lru.row = j;
            }
        }
    }
    return lru;
}

void memUnit::updateDirty(class ::LRU lru, bool dirty) {
    this->dirty[lru.way][lru.row] = dirty;
}