//
// Created by zxy on 4/25/22.
//
#include <iostream>
using  namespace  std;
#include "stinger_test.h"

#define printeb(eb) cout<<eb->vertexID<<' '<<eb->etype<<' '<<eb->high<<' '<<eb->smallStamp<<' '<<eb->largeStamp<<' '<<ebpool->ebpool + eb->next<<endl;







int main(){
    cout<<"12312312"<<endl;
    struct stinger_config_t * stinger_config;
    struct stinger * S;
    stinger_config = (struct stinger_config_t *)xcalloc(1,sizeof(struct stinger_config_t));
    stinger_config->nv = 1<<13;
    stinger_config->nebs = 1<<16;
    stinger_config->netypes = 2;
    stinger_config->nvtypes = 2;
    stinger_config->memory_size = 1<<30;
    S = stinger_new_full(stinger_config);
    int64_t consistency = stinger_consistency_check(S,S->max_nv);
    cout<<consistency<<endl;
    xfree(stinger_config);

    const vtype_t DEFAULT_TYPE = 0;
    const vtype_t INVALID_VERTEX = -1;


    vtype_t vtype;
    vtype = stinger_vtype_get(S,S->max_nv-1);
    cout<<vtype<<endl;

    vtype = stinger_vtype_get(S,S->max_nv+1);
    cout<<vtype<<endl;

    vtype = stinger_vtype_set(S,S->max_nv-1,1);
    cout<<vtype<<endl;


    vweight_t vweight;
    for (int i=0; i < 1000; i++) {
        stinger_vweight_increment(S, 2, 1);
    }
    vweight = stinger_vweight_get(S,2);
    cout<<vweight<<endl;



    struct stinger_eb * eb; // edge block
    MAP_STING(S);
    eb = ebpool->ebpool + stinger_adjacency_get(S, 1);
    printeb(eb)

    stinger_insert_edge(S, 0, 1, 2, 1, 1);//G, type, from, to, weight, timestamp
    eb = ebpool->ebpool + stinger_adjacency_get(S, 1);
    printeb(eb)
    //vertexID, etype, high, smallStamp, largeStamp, ebpool->ebpool + eb->next

    stinger_remove_edge (S, 0 ,1 ,2 );
    stinger_insert_edge(S, 0, 2, 1, 1, 1);
    stinger_insert_edge(S, 0, 2, 1, 1, 1);
    stinger_remove_edge (S, 0 ,2 ,1 );
    stinger_remove_edge (S, 0 ,2 ,1 );

    stinger_free_all(S);
}

