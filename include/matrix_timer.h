
#ifndef MATRIX_TIMER_H
#define MATRIX_TIMER_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <locale.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>


/* ─── matriz_time.h = Master header — all types, enums, constants, extern declarations───────────────────────────────────────────── */


/* ─── Version ────────────────────────────────────────────────── */
#define VERSION          "3.0"

/* ─── Settings ───────────────────────────────────────────────── */
#define FRAME_DELAY      40000   /* µs (~25 fps) */
#define MIN_TAIL         4
#define MAX_TAIL         30
#define MAX_PRESETS      12
#define HISTORY_MAX      30
#define ALERT_BLINK_RATE 4
#define ALERT_THRESHOLD  60
#define SAVE_FILE        "/tmp/.matrix_timer_save"
#define NOTES_FILE       "/tmp/.matrix_timer_notes"
#define TASKS_FILE       "/tmp/.matrix_timer_tasks"
#define MAX_SESSIONS     50
#define MAX_TASKS        20
#define MAX_NOTES        10
#define QUOTE_COUNT      20
#define BREAK_THRESHOLD  5   /* suggest break after N sessions */

/* ─── Visualization modes ────────────────────────────────────── */
typedef enum {
    VIZ_RAIN = 0,
    VIZ_VORTEX,
    VIZ_PULSE,
    VIZ_STATIC,
    VIZ_WAVE,
    VIZ_MATRIX3D,
    VIZ_FRACTAL,
    VIZ_FIREFLY,
    VIZ_COUNT
} VizMode;

extern const char *viz_names[];

/* ─── Color themes ───────────────────────────────────────────── */
typedef enum {
    THEME_GREEN = 0,
    THEME_BLUE,
    THEME_RED,
    THEME_GOLD,
    THEME_CYAN,
    THEME_PURPLE,
    THEME_RAINBOW,
    THEME_HACKER,
    THEME_SUNSET,
    THEME_COUNT
} Theme;

extern const char *theme_names[];

/* ─── Charset modes ──────────────────────────────────────────── */
typedef enum {
    CHARSET_MATRIX = 0,
    CHARSET_DIGITS,
    CHARSET_BINARY,
    CHARSET_KATAKANA,
    CHARSET_BRAILLE,
    CHARSET_HEX,
    CHARSET_DNA,
    CHARSET_RUNES,
    CHARSET_EMOJI,
    CHARSET_COUNT
} CharsetMode;

extern const char *charset_names[];

/* ─── Alert modes ────────────────────────────────────────────── */
typedef enum {
    ALERT_BEEP = 0,
    ALERT_VISUAL,
    ALERT_BOTH,
    ALERT_NONE
} AlertMode;

/* ─── Pomodoro phases ────────────────────────────────────────── */
typedef enum {
    POMODORO_WORK = 0,
    POMODORO_SHORT_BREAK,
    POMODORO_LONG_BREAK
} PomodoroPhase;

/* ─── Task ───────────────────────────────────────────────────── */
typedef struct {
    char text[64];
    int done;
    int session_id;
} Task;

/* ─── Quick note ─────────────────────────────────────────────── */
typedef struct {
    char text[128];
    time_t when;
} Note;

/* ─── Rain column (shared by all visualizations) ────────────── */
typedef struct {
    int x, y;
    int speed;
    int tail_len;
    int counter;
    int glitch;
    float angle;
    float radius;
    int color_offset;
    float z;
    float vx, vy;
    int brightness;
} RainColumn;

/* ─── Preset ─────────────────────────────────────────────────── */
typedef struct {
    int h, m, s;
    char label[32];
    int count_used;
    int is_pomodoro;
} Preset;

/* ─── History entry ──────────────────────────────────────────── */
typedef struct {
    int seconds;
    char label[32];
    time_t when;
    int completed;
} HistoryEntry;

/* ─── Global stats ───────────────────────────────────────────── */
typedef struct {
    int total_sessions;
    int total_seconds;
    int longest_session;
    int total_pauses;
    int pomodoros_completed;
    char most_used_preset[32];
    int daily_goal_seconds;
    int today_seconds;
    char today_date[16];
} Stats;

/* ─── Color pair IDs ─────────────────────────────────────────── */
#define CP_TAIL      1
#define CP_HEAD      2
#define CP_UI        3
#define CP_PAUSE     4
#define CP_ALERT     5
#define CP_DIM       6
#define CP_PROGRESS  7
#define CP_TITLE     8
#define CP_GLITCH    9
#define CP_CLOCK    10
#define CP_R1       11
#define CP_R2       12
#define CP_R3       13
#define CP_R4       14
#define CP_R5       15
#define CP_STATS    16
#define CP_TASK     17
#define CP_NOTE     18
#define CP_POMODORO 19
#define CP_GOAL     20
#define CP_FIREFLY  21

/* ─── Global state (defined in state.c) ─────────────────────── */
extern int max_y, max_x;
extern Theme current_theme;
extern CharsetMode charset;
extern VizMode viz_mode;
extern int show_percent;
extern int show_clock;
extern int show_stats_hud;
extern int density;
extern int rainbow_frame;
extern AlertMode alert_mode;
extern int fullscreen_clock;
extern int session_count;
extern int pomodoro_mode;
extern int pomodoro_count;
extern PomodoroPhase pom_phase;
extern int show_tasks;
extern int ambient_sound;
extern int show_quotes;
extern int current_quote;
extern int quote_timer;

extern const char *quotes[QUOTE_COUNT];
extern Task tasks[MAX_TASKS];
extern int task_count;
extern Note notes[MAX_NOTES];
extern int note_count;
extern const char *big_digits[10][5];
extern Preset presets[MAX_PRESETS];
extern int num_presets;
extern HistoryEntry history[HISTORY_MAX];
extern int history_count;
extern Stats global_stats;

#endif /* MATRIX_TIMER_H */
