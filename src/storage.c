#include "storage.h"



/* ─── storage.c = File persistence for state/tasks/notes, stats tracking, daily goal check, pomodoro sequencing ────────────────────────────────────── */



/* ═══════════════════════════════════════════════════════════════
   SESSION STATE
   ═══════════════════════════════════════════════════════════════ */
void save_state(int remaining, int total, const char *label) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (f) { fprintf(f, "%d %d %s\n", remaining, total, label); fclose(f); }
}

int load_state(int *remaining, int *total, char *label) {
    FILE *f = fopen(SAVE_FILE, "r");
    if (!f) return 0;
    int r = 0, t = 0;
    char lb[32] = {0};
    if (fscanf(f, "%d %d %31s", &r, &t, lb) == 3 && r > 0 && t > 0) {
        *remaining = r;
        *total     = t;
        strncpy(label, lb, 31);
        label[31] = '\0';
        fclose(f);
        remove(SAVE_FILE);
        return 1;
    }
    fclose(f);
    return 0;
}

/* ═══════════════════════════════════════════════════════════════
   TASKS
   ═══════════════════════════════════════════════════════════════ */
void save_tasks(void) {
    FILE *f = fopen(TASKS_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", task_count);
    for (int i = 0; i < task_count; i++)
        fprintf(f, "%d|%d|%s\n", tasks[i].done, tasks[i].session_id, tasks[i].text);
    fclose(f);
}

void load_tasks(void) {
    FILE *f = fopen(TASKS_FILE, "r");
    if (!f) return;
    fscanf(f, "%d\n", &task_count);
    if (task_count > MAX_TASKS) task_count = MAX_TASKS;
    for (int i = 0; i < task_count; i++)
        fscanf(f, "%d|%d|%63[^\n]\n", &tasks[i].done, &tasks[i].session_id, tasks[i].text);
    fclose(f);
}

/* ═══════════════════════════════════════════════════════════════
   NOTES
   ═══════════════════════════════════════════════════════════════ */
void save_notes(void) {
    FILE *f = fopen(NOTES_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", note_count);
    for (int i = 0; i < note_count; i++)
        fprintf(f, "%ld|%s\n", (long)notes[i].when, notes[i].text);
    fclose(f);
}

void load_notes(void) {
    FILE *f = fopen(NOTES_FILE, "r");
    if (!f) return;
    fscanf(f, "%d\n", &note_count);
    if (note_count > MAX_NOTES) note_count = MAX_NOTES;
    for (int i = 0; i < note_count; i++) {
        long ts;
        fscanf(f, "%ld|%127[^\n]\n", &ts, notes[i].text);
        notes[i].when = (time_t)ts;
    }
    fclose(f);
}

/* ═══════════════════════════════════════════════════════════════
   STATS
   ═══════════════════════════════════════════════════════════════ */
void check_daily_goal(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char today[16];
    strftime(today, sizeof(today), "%Y-%m-%d", tm_info);
    if (strcmp(today, global_stats.today_date) != 0) {
        global_stats.today_seconds = 0;
        strncpy(global_stats.today_date, today, 15);
    }
}

void update_stats(int seconds) {
    global_stats.total_sessions++;
    global_stats.total_seconds += seconds;
    if (seconds > global_stats.longest_session)
        global_stats.longest_session = seconds;
    check_daily_goal();
    global_stats.today_seconds += seconds;
}

void add_history(int seconds, const char *label, int completed) {
    HistoryEntry *e;
    if (history_count < HISTORY_MAX) {
        e = &history[history_count++];
    } else {
        memmove(history, history + 1, sizeof(HistoryEntry) * (HISTORY_MAX - 1));
        e = &history[HISTORY_MAX - 1];
    }
    e->seconds   = seconds;
    e->when      = time(NULL);
    e->completed = completed;
    strncpy(e->label, label, 31);
    e->label[31] = '\0';
}

/* ═══════════════════════════════════════════════════════════════
   POMODORO SEQUENCING
   ═══════════════════════════════════════════════════════════════ */
int next_pomodoro(void) {
    if (pom_phase == POMODORO_WORK) {
        pomodoro_count++;
        global_stats.pomodoros_completed++;
        pom_phase = (pomodoro_count % 4 == 0) ? POMODORO_LONG_BREAK : POMODORO_SHORT_BREAK;
        return (pom_phase == POMODORO_LONG_BREAK) ? 15 * 60 : 5 * 60;
    } else {
        pom_phase = POMODORO_WORK;
        return 25 * 60;
    }
}
