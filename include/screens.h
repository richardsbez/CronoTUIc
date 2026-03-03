#ifndef SCREENS_H
#define SCREENS_H

#include "matrix_timer.h"

int screen_main_menu(void);
int screen_time_input(int *out_seconds);
int screen_presets(int *out_seconds);
int screen_settings(void);
int screen_history(int *out_seconds);
int screen_statistics(void);
int screen_tasks(void);
int screen_notes(void);
void screen_countdown_final(const char *label);

#endif /* SCREENS_H */


/* ─── screens.h = TUI menus and screens ──────────────────────────────────────────── */
