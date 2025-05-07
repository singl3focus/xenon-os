#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init(void);
void keyboard_handler(void);

extern int shift_pressed;
extern int caps_lock_enabled;

#endif