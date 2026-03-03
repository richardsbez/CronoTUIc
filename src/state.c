#include "matrix_timer.h"



/* ─── state.c = Owns all global variables and default values ────────────────────────────────────── */



/* ─── Name tables ────────────────────────────────────────────── */
const char *viz_names[] = {
    "Matrix Rain", "Vortex", "Pulse", "Static", "Wave",
    "Matrix 3D", "Fractal", "Firefly"
};

const char *theme_names[] = {
    "Green Matrix", "Ice Blue", "Fire Red",
    "Gold", "Neon Cyan", "Purple Haze", "Rainbow",
    "Hacker Dark", "Sunset"
};

const char *charset_names[] = {
    "Matrix", "Digits", "Binary", "Katakana", "Braille",
    "Hex", "DNA", "Runes", "Symbols"
};

/* ─── Motivational quotes ────────────────────────────────────── */
const char *quotes[QUOTE_COUNT] = {
    "Focus is the art of saying no to a thousand things.",
    "Work hard in silence. Let success make the noise.",
    "Discipline is the bridge between goals and achievement.",
    "Don't wait. The time will never be just right.",
    "Small steps every day.",
    "Concentration is the passport to excellence.",
    "Do now what others won't. Do later what others can't.",
    "Every session brings you closer to your goal.",
    "Genius is 1% inspiration and 99% perspiration.",
    "Start where you are. Use what you have.",
    "Persistence accomplishes what talent cannot.",
    "You are stronger than you think.",
    "One day at a time. One task at a time.",
    "Success belongs to those who persist.",
    "Turn your effort into achievement.",
    "Every minute counts. This minute counts.",
    "Today is a great day to be productive.",
    "Silence + Focus = Results.",
    "Less distraction, more accomplishment.",
    "You are building something great. Keep going.",
};

/* ─── Big digit font (5 rows, 4 cols each) ───────────────────── */
const char *big_digits[10][5] = {
    {" ██ ", "█  █", "█  █", "█  █", " ██ "},
    {"  █ ", " ██ ", "  █ ", "  █ ", " ███"},
    {" ██ ", "█  █", "  █ ", " █  ", "████"},
    {"████", "   █", " ███", "   █", "████"},
    {"█  █", "█  █", "████", "   █", "   █"},
    {"████", "█   ", "███ ", "   █", "███ "},
    {" ██ ", "█   ", "███ ", "█  █", " ██ "},
    {"████", "   █", "  █ ", " █  ", "█   "},
    {" ██ ", "█  █", " ██ ", "█  █", " ██ "},
    {" ██ ", "█  █", " ███", "   █", " ██ "},
};

/* ─── Default presets ────────────────────────────────────────── */
Preset presets[MAX_PRESETS] = {
    {0, 25,  0, "Pomodoro",    0, 1},
    {0,  5,  0, "Short Break", 0, 0},
    {0, 15,  0, "Long Break",  0, 0},
    {1,  0,  0, "1 Hour",      0, 0},
    {0, 10,  0, "10 Min",      0, 0},
    {0, 45,  0, "Study Block", 0, 0},
    {0,  2,  0, "Quick Test",  0, 0},
    {2,  0,  0, "2 Hours",     0, 0},
    {0, 52,  0, "52/17 Work",  0, 0},
    {0, 17,  0, "52/17 Break", 0, 0},
    {0, 90,  0, "Deep Work",   0, 0},
    {0, 30,  0, "Half Hour",   0, 0},
};
int num_presets = MAX_PRESETS;

/* ─── Runtime state ──────────────────────────────────────────── */
int max_y = 0, max_x = 0;
Theme         current_theme  = THEME_GREEN;
CharsetMode   charset        = CHARSET_MATRIX;
VizMode       viz_mode       = VIZ_RAIN;
int           show_percent   = 1;
int           show_clock     = 1;
int           show_stats_hud = 0;
int           density        = 2;
int           rainbow_frame  = 0;
AlertMode     alert_mode     = ALERT_BOTH;
int           fullscreen_clock = 0;
int           session_count  = 0;
int           pomodoro_mode  = 0;
int           pomodoro_count = 0;
PomodoroPhase pom_phase      = POMODORO_WORK;
int           show_tasks     = 1;
int           ambient_sound  = 0;
int           show_quotes    = 1;
int           current_quote  = 0;
int           quote_timer    = 0;

Task          tasks[MAX_TASKS];
int           task_count     = 0;
Note          notes[MAX_NOTES];
int           note_count     = 0;

HistoryEntry  history[HISTORY_MAX];
int           history_count  = 0;

Stats global_stats = {0, 0, 0, 0, 0, "", 7200, 0, ""};
