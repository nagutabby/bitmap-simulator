#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_STORAGE_SYSTEM_BLOCK 4096
#define NUM_BLOCK_BIT (sizeof(uint32_t) * 8)

// 複数のブロックのまとまりで構成されるストレージ
uint32_t storage[NUM_STORAGE_SYSTEM_BLOCK / NUM_BLOCK_BIT];
// 空きビット数
int num_empty_bit;

#define	SET_DATA(num_bit) storage[num_bit / NUM_BLOCK_BIT] |= (1 << (num_bit % NUM_BLOCK_BIT))
#define	CLEAR(num_bit) storage[num_bit / NUM_BLOCK_BIT] &= ~(1 << (num_bit % NUM_BLOCK_BIT))

// ビットが空かどうか判定する
bool is_empty(int num_bit) {
    return (!(storage[num_bit / NUM_BLOCK_BIT] & (1 << (num_bit % NUM_BLOCK_BIT))));
}

// ビットマップのすべてのビットを0にする
void clear() {
    memset(storage, 0, sizeof(storage));
    num_empty_bit = NUM_STORAGE_SYSTEM_BLOCK;
}

// nblksブロックだけ割り当てる
// 割り当てた領域の先頭のブロック番号を返す
// 割り当てに失敗した場合は-1を返す
int allocate(int num_block_required) {
    int head;
    int num_free_block;

    for (head = 0; head < NUM_STORAGE_SYSTEM_BLOCK; head++) {
        for (num_free_block = 0; head + num_free_block < NUM_STORAGE_SYSTEM_BLOCK; num_free_block++) {
            // ビットが空ではないなら、これ以上num_free_blockを増やせないのでループから抜ける
            if (!is_empty(head + num_free_block)) {
                break;
            }
        }
        // 必要なブロック数が集まったらループから抜ける
        if (num_free_block > num_block_required) {
            break;
        }
    }
    if (head < NUM_STORAGE_SYSTEM_BLOCK) {
        // 必要なブロックを確保する
        for (int i = 0; i < num_block_required; i++) {
            SET_DATA(head + i);
        }
        return head;
    } else {
        return -1;
    }
}
