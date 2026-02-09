#ifndef MOUSE_H
#define MOUSE_H

void mouse_init(void);
void mouse_poll(void);

int mouse_get_x(void);
int mouse_get_y(void);

void show_mouse(int x, int y);
void hide_mouse(int x, int y);
int mouse_get_button();
#endif
