#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getcmd.c"

#define SSBLK 1024
#define NUM_BLOCK_BIT (sizeof(uint32_t) * 8)

// 複数のブロックのまとまりで構成されるストレージ
uint32_t storage[SSBLK / NUM_BLOCK_BIT];
// 空きブロック数
int num_free_block;

#define SET(num_block) \
  storage[num_block / NUM_BLOCK_BIT] |= (1 << (num_block % NUM_BLOCK_BIT))
#define CLEAR(num_block) \
  storage[num_block / NUM_BLOCK_BIT] &= ~(1 << (num_block % NUM_BLOCK_BIT))

// ビットが空かどうか判定する
bool is_free(int num_bit) {
  return (
      !(storage[num_bit / NUM_BLOCK_BIT] & (1 << (num_bit % NUM_BLOCK_BIT))));
}

// ビットマップのすべてのビットを0にする
void bitmap_clear() {
  memset(storage, 0, sizeof(storage));
  num_free_block = SSBLK;
}

// 必要なブロックを割り当てる
// 割り当てに成功した場合: 割り当てた領域の先頭のブロック番号を返す
// 割り当てに失敗した場合: -1を返す
int bitmap_allocate(int num_block_required) {
  int head;
  int count_free_block;

  for (head = 0; head < SSBLK; head++) {
    for (count_free_block = 0; head + count_free_block < SSBLK;
         count_free_block++) {
      // ビットが空ではないなら、これ以上count_free_blockを増やせないのでループから抜ける
      if (!is_free(head + count_free_block)) {
        break;
      }
    }
    // 必要なブロック数が集まったらループから抜ける
    if (count_free_block > num_block_required) {
      break;
    }
  }
  if (head < SSBLK) {
    // 必要なブロックを確保する
    for (int i = 0; i < num_block_required; i++) {
      SET(head + i);
      num_free_block--;
    }
    return head;
  } else {
    printf("ブロック割り当てに失敗しました\n");
    return -1;
  }
}

// ブロックを解放する
// 解放できた場合: 0を返す
// 解放できなかった場合: -1を返す
int bitmap_free(int block_index, int num_block) {
  if (block_index < 0 || block_index > SSBLK - 1) {
    printf("管理していないブロックは開放できません\n");
    return -1;
  } else {
    for (int i = 0; i < num_block; i++) {
      if (!is_free(block_index + i)) {
        CLEAR(block_index + i);
        num_free_block++;
      }
    }
    return 0;
  }
}

// データの状態を出力する
void bitmap_dump() {
  for (int i = 0; i < SSBLK; i++) {
    putchar(is_free(i) ? '0' : '1');

    if (i % 64 == 63) {
      putchar('\n');
    } else if (i % 8 == 7) {
      putchar(' ');
    }
  }
}

// ブロック割り当てが適切かどうかチェックする
void bitmap_verify() {
  int num_block = 0;
  for (int i = 0; i < SSBLK; i++) {
    if (!is_free(i)) {
      num_block++;
    }
  }
  if (num_block == (SSBLK - num_free_block)) {
    printf("ブロック割り当ては適切です\n");
  } else {
    printf("ブロック割り当てが不適切です\n");
  }
  printf("空きブロック数: %d, 割り当て済みのブロック数: %d\n", num_free_block,
         num_block);
}

void bitmap_interactive() {
  int bn;
  char cmd;
  int param1, param2;
  int ic;

  for (;;) {
    fputs("bitmap> ", stdout); fflush(stdout);
    switch((ic = getcmd(&cmd, &param1, &param2))) {
      case 0:
        goto out;
      case 1:
        if (cmd == 'd')
          bitmap_dump();
        else if (cmd == 'v')
          bitmap_verify();
        break;
      case 2:
        if (cmd == 'a') {
          bn = bitmap_allocate(param1);
          printf("A %d %d\n", bn, param1);
        }
        break;
      case 3:
        if (cmd == 'f') {
          bitmap_free(param1, param2);
          printf("F %d\n", param1);
        }
        break;
      case -1:
        break;
    }
  }
out:;
}

int main() {
  bitmap_clear();

  // 正常系テスト
  // bitmap_allocate(2);
  // bitmap_free(0, 1);
  // bitmap_allocate(10);


  // 異常系テスト
  // bitmap_allocate(1024);
  // bitmap_free(-1, 1);

#if 1
  bitmap_interactive();
#endif

  bitmap_dump();
  bitmap_verify();
  return 0;
}
