#include "Keypad4x4.h"
#include "delay.h"

signed char pushBtn(void){
    // P0 是按鈕
    signed char num = 0;
    for(char r=0; r<4; r++){
        P0 = ~(0x10<<r);         // 設定現在是在第幾個 row
        
        if((P0 & 0xF0) != 0xF0){ // 代表有按下按鈕
            for(char c=0; c<4; c++){
                if((P0 & 0x0F) == (~(1<<c) & 0x0F)){   //確認是第幾列
                    return  r * 4 + c;                // 根據 row & col 計算 number
                }
            }
        }
        
    }
    return -1;      // 如果沒有按下按鈕就回傳 -1
}
