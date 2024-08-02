#ifndef GUI_GADTOOLS_H
#define GUI_GADTOOLS_H

int gui_gadtools_init();
void gui_gadtools_close();
extern "C" {
 struct Menu;
}
extern struct Menu *g_gtMenu;
#endif
