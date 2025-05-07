#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init(void);
void keyboard_handler(void);

int keyboard_is_shift_pressed(void);
int keyboard_is_capslock_enabled(void);

#endif