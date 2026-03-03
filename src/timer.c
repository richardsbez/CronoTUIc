#include "timer.h"
#include "display.h"
#include "visualizations.h"
#include "storage.h"
#include "screens.h"



/* ─── timer.c = The timer loop and stopwatch loop — uses all the other modules ────────────────────────────────────── */



/* Helper: allocate and initialise column array */
static RainColumn *alloc_columns(int count) {
    RainColumn *cols = malloc(sizeof(RainColumn) * count);
    for (int i = 0; i < count; i++) {
        reset_column(&cols[i], i * density);
        cols[i].y = rand() % max_y;
    }
    return cols;
}

/* ═══════════════════════════════════════════════════════════════
   TIMER UI OVERLAY
   ═══════════════════════════════════════════════════════════════ */
void draw_timer_ui(int remaining, int total, int paused, int frame, const char *label) {
    int alert = (remaining <= ALERT_THRESHOLD && remaining > 0);

    /* Big clock: fullscreen mode or last 5 minutes */
    if ((paused && fullscreen_clock) || (remaining <= 300 && !paused && max_y > 20)) {
        draw_big_clock(max_y/2 - 3, (max_x - 40)/2, remaining);
    }

    /* Main status line */
    char buf[100];
    int h = remaining / 3600, m = (remaining % 3600) / 60, s = remaining % 60;
    char pct_buf[16] = "";
    if (show_percent && total > 0) {
        int pct = (int)((1.0f - (float)remaining / total) * 100);
        snprintf(pct_buf, sizeof(pct_buf), " %3d%%", pct);
    }

    const char *phase_str = "";
    if (pomodoro_mode) {
        phase_str = (pom_phase == POMODORO_WORK)        ? " [FOCUS]"      :
                    (pom_phase == POMODORO_SHORT_BREAK)  ? " [BREAK]"      :
                                                           " [LONG BREAK]";
    }

    if (paused) {
        snprintf(buf, sizeof(buf), "[ %02d:%02d:%02d%s PAUSED%s ]", h, m, s, pct_buf, phase_str);
        attron(COLOR_PAIR(CP_PAUSE) | A_BOLD | A_BLINK);
    } else if (alert && (alert_mode == ALERT_VISUAL || alert_mode == ALERT_BOTH)
               && (frame / ALERT_BLINK_RATE) % 2 == 0) {
        snprintf(buf, sizeof(buf), "[ %02d:%02d:%02d%s *** ALERT! *** ]", h, m, s, pct_buf);
        attron(COLOR_PAIR(CP_ALERT) | A_BOLD | A_BLINK);
    } else {
        snprintf(buf, sizeof(buf), "[ %02d:%02d:%02d%s%s ]", h, m, s, pct_buf, phase_str);
        attron(COLOR_PAIR(CP_HEAD) | A_BOLD);
    }
    mvprintw(0, (max_x - (int)strlen(buf)) / 2, "%s", buf);
    attroff(A_BOLD | A_BLINK | COLOR_PAIR(CP_HEAD) | COLOR_PAIR(CP_PAUSE) | COLOR_PAIR(CP_ALERT));

    /* Session label */
    if (label && strlen(label) > 0) {
        attron(COLOR_PAIR(CP_DIM) | A_DIM);
        mvprintw(1, (max_x - (int)strlen(label) - 4) / 2, " [%s] ", label);
        attroff(COLOR_PAIR(CP_DIM) | A_DIM);
    }

    /* Segmented progress bar */
    int bar_w = max_x - 12;
    if (bar_w < 4) bar_w = 4;
    int filled = (int)((1.0f - (float)remaining / total) * bar_w);

    attron(COLOR_PAIR(alert ? CP_ALERT : CP_PROGRESS) | A_BOLD);
    mvprintw(max_y - 4, 5, "[");
    for (int i = 0; i < bar_w; i++) {
        if (i < filled) {
            if (current_theme == THEME_RAINBOW) {
                int rp = CP_R1 + (int)((float)i / bar_w * 4);
                attron(COLOR_PAIR(rp) | A_BOLD);
            }
            addch(i % 5 == 4 ? '|' : '=');
            if (current_theme == THEME_RAINBOW)
                attroff(COLOR_PAIR(CP_R1)|COLOR_PAIR(CP_R2)|COLOR_PAIR(CP_R3)|
                        COLOR_PAIR(CP_R4)|COLOR_PAIR(CP_R5)|A_BOLD);
        } else if (i == filled) {
            addch('>');
        } else {
            attron(A_DIM); addch('.'); attroff(A_DIM);
        }
    }
    addch(']');
    attroff(COLOR_PAIR(alert ? CP_ALERT : CP_PROGRESS) | A_BOLD);

    /* Keymap hint */
    attron(COLOR_PAIR(CP_UI) | A_DIM);
    mvprintw(max_y - 3, 2,
             "[P]Pause [R]Reset [S]Save [T]Theme [V]Viz [C]Char [F]BigClock [H]HUD [UP/DN]+-1min [X]DoneTask [Q]Quit");
    attroff(COLOR_PAIR(CP_UI) | A_DIM);

    draw_hud_stats(remaining, total);
    draw_goal_bar();
    draw_current_clock();
    draw_tasks_hud();
    draw_quote();
    draw_pomodoro_hud();
}

/* ═══════════════════════════════════════════════════════════════
   MAIN TIMER LOOP
   ═══════════════════════════════════════════════════════════════ */
void timer_loop(int total_seconds, const char *label) {
    ncurses_init();
    srand((unsigned)time(NULL));
    getmaxyx(stdscr, max_y, max_x);

    int num_cols = max_x / density;
    if (num_cols < 1) num_cols = 1;
    RainColumn *cols = alloc_columns(num_cols);

    time_t start_time    = time(NULL);
    int paused           = 0;
    time_t pause_start   = 0;
    int total_paused     = 0;
    int frame            = 0;
    int alert_beeped     = 0;
    int cancelled        = 0;

    while (1) {
        frame++;

        /* Handle terminal resize */
        int ny, nx;
        getmaxyx(stdscr, ny, nx);
        if (ny != max_y || nx != max_x) {
            max_y = ny; max_x = nx;
            free(cols);
            num_cols = max_x / density;
            if (num_cols < 1) num_cols = 1;
            cols = alloc_columns(num_cols);
            erase();
        }

        int elapsed   = !paused
                        ? (int)difftime(time(NULL), start_time) - total_paused
                        : (int)difftime(pause_start, start_time) - total_paused;
        int remaining = total_seconds - elapsed;
        if (remaining < 0) remaining = 0;

        /* Alert threshold beep */
        if (remaining <= ALERT_THRESHOLD && !alert_beeped && remaining > 0) {
            alert_beeped = 1;
            if (alert_mode == ALERT_BEEP || alert_mode == ALERT_BOTH) beep();
        }

        erase();

        if (!paused) {
            draw_visualization(cols, num_cols, frame, remaining, total_seconds);
        } else {
            /* Frozen columns while paused */
            for (int i = 0; i < num_cols; i++) {
                for (int j = 0; j < cols[i].tail_len; j++) {
                    int dy = cols[i].y - j;
                    if (dy >= 1 && dy < max_y - 4) {
                        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
                        mvaddch(dy, cols[i].x, '|');
                        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
                    }
                }
            }
        }

        draw_timer_ui(remaining, total_seconds, paused, frame, label);
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            cancelled = 1;
            free(cols); endwin(); return;
        } else if (ch == 'r' || ch == 'R') {
            free(cols); endwin();
            timer_loop(total_seconds, label);
            return;
        } else if (ch == 'p' || ch == 'P') {
            if (!paused) { paused = 1; pause_start = time(NULL); global_stats.total_pauses++; }
            else { total_paused += (int)difftime(time(NULL), pause_start); paused = 0; }
        } else if (ch == 's' || ch == 'S') {
            save_state(remaining, total_seconds, label);
            attron(COLOR_PAIR(CP_PAUSE) | A_BOLD);
            mvprintw(max_y/2, (max_x - 20)/2, "  State saved!  ");
            attroff(COLOR_PAIR(CP_PAUSE) | A_BOLD);
            refresh();
            struct timespec ts = {1, 0};
            nanosleep(&ts, NULL);
        } else if (ch == 't' || ch == 'T') {
            current_theme = (Theme)((current_theme + 1) % THEME_COUNT);
            apply_theme(current_theme);
        } else if (ch == 'v' || ch == 'V') {
            viz_mode = (VizMode)((viz_mode + 1) % VIZ_COUNT);
        } else if (ch == 'c' || ch == 'C') {
            charset = (CharsetMode)((charset + 1) % CHARSET_COUNT);
        } else if (ch == 'f' || ch == 'F') {
            fullscreen_clock = !fullscreen_clock;
        } else if (ch == 'h' || ch == 'H') {
            show_stats_hud = !show_stats_hud;
        } else if (ch == '+' || ch == '=') {
            for (int i = 0; i < num_cols; i++) if (cols[i].speed > 1) cols[i].speed--;
        } else if (ch == '-') {
            for (int i = 0; i < num_cols; i++) if (cols[i].speed < 5) cols[i].speed++;
        } else if (ch == KEY_UP) {
            total_seconds += 60;
        } else if (ch == KEY_DOWN) {
            if (total_seconds > 60) total_seconds -= 60;
        } else if (ch == 'x' || ch == 'X') {
            /* Mark the next pending task done */
            for (int i = 0; i < task_count; i++) {
                if (!tasks[i].done) {
                    tasks[i].done       = 1;
                    tasks[i].session_id = session_count;
                    save_tasks();
                    attron(COLOR_PAIR(CP_TASK) | A_BOLD);
                    mvprintw(max_y/2, (max_x - 32)/2, "  Task done: %.20s  ", tasks[i].text);
                    attroff(COLOR_PAIR(CP_TASK) | A_BOLD);
                    refresh();
                    struct timespec ts = {0, 700000000L};
                    nanosleep(&ts, NULL);
                    break;
                }
            }
        }

        if (remaining == 0) break;

        struct timespec ts = {0, FRAME_DELAY * 1000L};
        nanosleep(&ts, NULL);
    }

    free(cols);
    update_stats(total_seconds);
    session_count++;
    add_history(total_seconds, label, !cancelled);
    screen_countdown_final(label);

    /* Post-timer input: restart / next pomodoro / quit */
    nodelay(stdscr, FALSE);
    while (1) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') { endwin(); return; }
        if (ch == 'r' || ch == 'R') { endwin(); timer_loop(total_seconds, label); return; }
        if ((ch == 'p' || ch == 'P') && pomodoro_mode) {
            endwin();
            int next_secs = next_pomodoro();
            const char *next_label = (pom_phase == POMODORO_WORK)       ? "Focus"       :
                                     (pom_phase == POMODORO_SHORT_BREAK) ? "Short Break" :
                                                                           "Long Break";
            timer_loop(next_secs, next_label);
            return;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   STOPWATCH LOOP
   ═══════════════════════════════════════════════════════════════ */
void stopwatch_loop(void) {
    ncurses_init();
    srand((unsigned)time(NULL));
    getmaxyx(stdscr, max_y, max_x);

    int num_cols = max_x / density;
    if (num_cols < 1) num_cols = 1;
    RainColumn *cols = alloc_columns(num_cols);

    time_t start     = time(NULL);
    int paused       = 0;
    time_t pause_start = 0;
    int total_paused = 0;
    int frame        = 0;
    int laps[50], lap_count = 0, last_lap_time = 0;

    while (1) {
        frame++;

        int ny, nx;
        getmaxyx(stdscr, ny, nx);
        if (ny != max_y || nx != max_x) {
            max_y = ny; max_x = nx;
            free(cols);
            num_cols = max_x / density;
            if (num_cols < 1) num_cols = 1;
            cols = alloc_columns(num_cols);
            erase();
        }

        int elapsed = !paused
                      ? (int)difftime(time(NULL), start) - total_paused
                      : (int)difftime(pause_start, start) - total_paused;

        erase();
        draw_visualization(cols, num_cols, frame, 30, 60);

        /* Status line */
        int h = elapsed/3600, m = (elapsed%3600)/60, s = elapsed%60;
        int ms = (frame * 40) % 1000;
        char buf[64];
        snprintf(buf, sizeof(buf),
                 paused ? "[ %02d:%02d:%02d.%03d  PAUSED ]" : "[ %02d:%02d:%02d.%03d ]",
                 h, m, s, ms);
        attron(COLOR_PAIR(paused ? CP_PAUSE : CP_HEAD) | A_BOLD);
        mvprintw(0, (max_x - (int)strlen(buf)) / 2, "%s", buf);
        attroff(COLOR_PAIR(paused ? CP_PAUSE : CP_HEAD) | A_BOLD);

        draw_big_clock(max_y/2 - 3, (max_x - 40)/2, elapsed);

        /* Lap list */
        if (lap_count > 0) {
            attron(COLOR_PAIR(CP_UI) | A_DIM);
            for (int i = lap_count - 1; i >= 0 && i >= lap_count - 5; i--) {
                int ly = max_y - 7 - (lap_count - 1 - i);
                if (ly > 2) {
                    int lh = laps[i]/3600, lm = (laps[i]%3600)/60, ls2 = laps[i]%60;
                    mvprintw(ly, 2, "  Lap %d: %02d:%02d:%02d", i+1, lh, lm, ls2);
                }
            }
            attroff(COLOR_PAIR(CP_UI) | A_DIM);
        }

        draw_tasks_hud();
        draw_quote();
        draw_current_clock();

        attron(COLOR_PAIR(CP_UI) | A_DIM);
        mvprintw(max_y - 2, 2,
                 "[P]Pause  [L]Lap  [R]Reset  [T]Theme  [V]Viz  [C]Charset  [Q]Quit");
        attroff(COLOR_PAIR(CP_UI) | A_DIM);

        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q') { free(cols); endwin(); return; }
        else if (ch == 'r' || ch == 'R') {
            start = time(NULL); total_paused = 0; paused = 0;
            lap_count = 0; last_lap_time = 0;
        } else if (ch == 'p' || ch == 'P') {
            if (!paused) { paused = 1; pause_start = time(NULL); }
            else { total_paused += (int)difftime(time(NULL), pause_start); paused = 0; }
        } else if ((ch == 'l' || ch == 'L') && lap_count < 50) {
            laps[lap_count++] = elapsed - last_lap_time;
            last_lap_time = elapsed;
        } else if (ch == 't' || ch == 'T') {
            current_theme = (Theme)((current_theme + 1) % THEME_COUNT);
            apply_theme(current_theme);
        } else if (ch == 'v' || ch == 'V') {
            viz_mode = (VizMode)((viz_mode + 1) % VIZ_COUNT);
        } else if (ch == 'c' || ch == 'C') {
            charset = (CharsetMode)((charset + 1) % CHARSET_COUNT);
        }

        struct timespec ts = {0, FRAME_DELAY * 1000L};
        nanosleep(&ts, NULL);
    }
}
