#include "tests.h"

#include <stdlib.h>

#include "shared/algorithm/map.h"
#include "shared/utils.h"
#include "utils/debug.h"

void test_map() {
    Map m;
    make_map(&m);
    uint8_t idx;
    // check that no element in map
    for (idx = 0; idx<255; ++idx) {
        assert(map_index(&m, idx) == -1);
    }
    // add value 66 at idx 33
    uint32_t* val = (uint32_t*)malloc(sizeof(uint32_t));
    *val = 66;
    map_add(&m, 33, (void*)val);
    // assert value is correct
    assert(*((uint32_t*)map_get(&m, 33)) == 66);
    // make sure only 33 is present
    for (idx = 0; idx<255; ++idx) {
        if (idx == 33) {
            assert(map_index(&m, idx) != -1);
        } else {
            assert(map_index(&m, idx) == -1);
        }
    }
    // remove 33
    map_remove(&m, 33);
    // assume was removed
    for (idx = 0; idx<255; ++idx) {
        assert(map_index(&m, idx) == -1);
    }
    // add 2*idx at index idx for all idx;
    for (idx = 0; idx<255; ++idx) {
        uint32_t* val2 = (uint32_t*)malloc(sizeof(uint32_t));
        *val2 = 2LL*idx;
        map_add(&m, idx, (void*)val2);
    }  
    // check it worked
    for (idx = 0; idx<255; ++idx) {
        assert(*((uint32_t*)map_get(&m, idx)) == 2LL*idx);
        assert(map_index(&m, idx) != -1);
    }
    // remove all element
    for (idx = 0; idx<255; ++idx) {
        map_remove(&m, idx);
    }  
    // check map empty
    for (idx = 0; idx<255; ++idx) {
        assert(map_index(&m, idx) == -1);
    }
    map_destroy(&m);
    debug(DEBUG_INFO, "Map test successful.");
}