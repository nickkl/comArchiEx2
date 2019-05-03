//
// Created by nick on 5/3/19.
//

#ifndef CACHESIM_LRU_H
#define CACHESIM_LRU_H


class LRU {
    int way = 0;
    int row =0;
public:
    LRU(int way, int row){
        this->way=way;
        this->row=row;
    };
};


#endif //CACHESIM_LRU_H
