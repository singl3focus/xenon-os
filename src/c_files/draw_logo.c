#include "fb.h"
#include "draw_logo.h"

void draw_logo(){
    for(int i = 0; i < 62; ++i){
        draw_line(0, i, 1024, 0x00BF00FF);
        delay(500000);
    }
    int x_coord = 352;
    draw_char(x_coord, 15, 'X', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'e', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'n', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'o', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'n', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, ' ', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'O', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
    x_coord += 40;
    draw_char(x_coord, 15, 'S', 0x00000000, 0x00BF00FF, 5);
    delay(5000000);
}