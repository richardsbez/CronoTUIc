#ifndef DISPLAY_H
#define DISPLAY_H

#include "matrix_timer.h"

/* Ncurses lifecycle */
void ncurses_init(void);
void apply_theme(Theme t);

/* Shared drawing helpers */
char random_char(void);
void reset_column(RainColumn *col, int x_pos);
void draw_decorative_border(void);
void draw_big_clock(int start_y, int start_x, int remaining);
void draw_current_clock(void);
void draw_hud_stats(int remaining, int total);
void draw_goal_bar(void);
void draw_tasks_hud(void);
void draw_quote(void);
void draw_pomodoro_hud(void);

#endif /* DISPLAY_H */

/* ─── display.h = ncurses init, theme, HUD elements ──────────────────────────────────────────── */

