#include "fb.h"
#include "draw_logo.h"

void draw_logo(){
    for (int i = 10; i <= 1014; ++i){
        draw_pixel(i, 10, 0x00FFFFFF);
    }
    for (int i = 10; i <= 92; ++i){
        draw_pixel(1014, i, 0x00FFFFFF);
    }
    for (int i = 1014; i >= 10; --i){
        draw_pixel(i, 92, 0x00FFFFFF);
    }
    for (int i = 92; i >= 10; --i){
        draw_pixel(10, i, 0x00FFFFFF);
    }
    const uint8_t name_os[8] = {'X', 'e', 'n', 'o', 'n', ' ', 'O', 'S'};
    int x_coord = 357;
    int i = 0;
    while (i < 8){
        draw_char(x_coord, 34, name_os[i], 0x00FFFFFF, 0x001F2126, 5);
        if (name_os[i] != ' ') delay(5000000);
        x_coord += 40;
        i += 1;
    } 
    
    for (int i = 15; i <= 1009; ++i){
        draw_pixel(i, 15, 0x00FFFFFF);
    }
    for (int i = 15; i <= 87; ++i){
        draw_pixel(1009, i, 0x00FFFFFF);
    }
    for (int i = 1009; i >= 15; --i){
        draw_pixel(i, 87, 0x00FFFFFF);
    }
    for (int i = 87; i >= 15; --i){
        draw_pixel(15, i, 0x00FFFFFF);
    }
}