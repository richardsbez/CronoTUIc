#ifndef TIMER_H
#define TIMER_H

#include "matrix_timer.h"

void timer_loop(int total_seconds, const char *label);
void stopwatch_loop(void);
void draw_timer_ui(int remaining, int total, int paused, int frame, const char *label);

#endif /* TIMER_H */


/* ─── timer.h = Timer and stopwatch loops ──────────────────────────────────────────── */

