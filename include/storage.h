#ifndef STORAGE_H
#define STORAGE_H

#include "matrix_timer.h"

/* Session state save/load */
void save_state(int remaining, int total, const char *label);
int  load_state(int *remaining, int *total, char *label);

/* Tasks persistence */
void save_tasks(void);
void load_tasks(void);

/* Notes persistence */
void save_notes(void);
void load_notes(void);

/* Stats */
void update_stats(int seconds);
void check_daily_goal(void);
void add_history(int seconds, const char *label, int completed);

/* Pomodoro */
int  next_pomodoro(void);

#endif /* STORAGE_H */




/* ─── storage.h = File persistence, stats, pomodoro logic ──────────────────────────────────────────── */
