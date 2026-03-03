#include "screens.h"
#include "display.h"
#include "storage.h"



/* ─── screens.c = Every interactive TUI screen (main menu, presets, settings, history, stats, tasks, notes, final countdown) ────────────────────────────────────── */



/* ═══════════════════════════════════════════════════════════════
   MAIN MENU
   ═══════════════════════════════════════════════════════════════ */
int screen_main_menu(void) {
    nodelay(stdscr, FALSE);
    const char *logo[] = {
        " __  __    _  _____ ____  ___ __  __   _____ ___ __  __ _____ ____  ",
        "|  \\/  |  / \\|_   _|  _ \\|_ _\\ \\/ /  |_   _|_ _|  \\/  | ____|  _ \\ ",
        "| |\\/| | / _ \\ | | | |_) || | >  <     | |  | || |\\/| |  _| | |_) |",
        "| |  | |/ ___ \\| | |  _ < | |/ . \\     | |  | || |  | | |___|  _ < ",
        "|_|  |_/_/   \\_\\_| |_| \\_\\___/_/\\_\\    |_| |___|_|  |_|_____|_| \\_\\",
    };

    int saved_remaining = 0, saved_total = 0;
    char saved_label[32] = {0};
    int has_save = load_state(&saved_remaining, &saved_total, saved_label);
    int anim_frame = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();
        anim_frame++;

        /* Animated background particles */
        if (anim_frame % 2 == 0) {
            for (int i = 0; i < max_x / 4; i++) {
                int rx = 1 + rand() % (max_x - 2);
                int ry = 1 + rand() % (max_y - 2);
                attron(COLOR_PAIR(CP_DIM) | A_DIM);
                mvaddch(ry, rx, random_char());
                attroff(COLOR_PAIR(CP_DIM) | A_DIM);
            }
        }

        /* Logo */
        int logo_y = (max_y > 30) ? max_y / 2 - 12 : 2;
        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        for (int i = 0; i < 5; i++) {
            int len = (int)strlen(logo[i]);
            int lx = (max_x - len) / 2;
            if (lx < 1) lx = 1;
            if (logo_y + i < max_y - 1)
                mvprintw(logo_y + i, lx, "%s", logo[i]);
        }
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(logo_y + 6, (max_x - 44) / 2,
                 "[ C o u n t d o w n  T i m e r  v%s  -  Enhanced ]", VERSION);
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);

        /* Menu items */
        int my = logo_y + 9;
        struct { char key; const char *desc; int highlight; } items[] = {
            {'1', "Enter time manually",       0},
            {'2', "Time presets",              0},
            {'s', "Stopwatch",                 0},
            {'p', "Pomodoro auto mode",        pomodoro_mode},
            {'t', "Tasks",                     0},
            {'n', "Quick notes",               0},
            {'5', "Statistics & Goals",        0},
            {'3', "Settings",                  0},
            {'4', "Session history",           0},
            {'7', "Resume saved session",      has_save},
            {'q', "Quit",                      0},
        };
        int n_items = (int)(sizeof(items) / sizeof(items[0]));

        for (int i = 0; i < n_items; i++) {
            int py = my + i * 2;
            if (py >= max_y - 4) break;
            if (items[i].highlight) {
                attron(COLOR_PAIR(CP_PAUSE) | A_BOLD);
                mvprintw(py, (max_x - 40) / 2, "  [%c]  %-30s [ON]", items[i].key, items[i].desc);
                attroff(COLOR_PAIR(CP_PAUSE) | A_BOLD);
            } else if (i == 9 && has_save) {
                attron(COLOR_PAIR(CP_POMODORO) | A_BOLD);
                mvprintw(py, (max_x - 52) / 2,
                         "  [7]  Resume: %-12s (%d:%02d:%02d)  ",
                         saved_label,
                         saved_remaining / 3600,
                         (saved_remaining % 3600) / 60,
                         saved_remaining % 60);
                attroff(COLOR_PAIR(CP_POMODORO) | A_BOLD);
            } else {
                attron(COLOR_PAIR(CP_UI));
                mvprintw(py, (max_x - 40) / 2, "  [%c]  %-30s", items[i].key, items[i].desc);
                attroff(COLOR_PAIR(CP_UI));
            }
        }

        /* Footer */
        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2,
                 "Theme:%-10s Charset:%-8s Viz:%-10s Density:%d Poms:%d",
                 theme_names[current_theme], charset_names[charset],
                 viz_names[viz_mode], density,
                 global_stats.pomodoros_completed);
        time_t now = time(NULL);
        struct tm *tmi = localtime(&now);
        char clkbuf[16];
        strftime(clkbuf, sizeof(clkbuf), "%H:%M:%S", tmi);
        mvprintw(max_y - 2, max_x - 12, "%s", clkbuf);
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);

        /* Daily goal */
        if (global_stats.daily_goal_seconds > 0) {
            check_daily_goal();
            int pct = (int)((float)global_stats.today_seconds / global_stats.daily_goal_seconds * 100);
            if (pct > 100) pct = 100;
            attron(COLOR_PAIR(CP_GOAL) | A_DIM);
            mvprintw(max_y - 4, 2, "Daily goal: %d%% (%dm/%dm)",
                     pct,
                     global_stats.today_seconds / 60,
                     global_stats.daily_goal_seconds / 60);
            attroff(COLOR_PAIR(CP_GOAL) | A_DIM);
        }

        /* Quote */
        const char *q = quotes[current_quote];
        int qlen = (int)strlen(q);
        if (max_x > qlen + 4) {
            attron(COLOR_PAIR(CP_DIM) | A_ITALIC);
            mvprintw(max_y - 5, (max_x - qlen) / 2, "\"%s\"", q);
            attroff(COLOR_PAIR(CP_DIM) | A_ITALIC);
        }

        refresh();

        int ch = getch();
        switch (ch) {
            case '1': nodelay(stdscr, TRUE); return 1;
            case '2': nodelay(stdscr, TRUE); return 2;
            case '3': nodelay(stdscr, TRUE); return 3;
            case '4': nodelay(stdscr, TRUE); return 4;
            case '5': nodelay(stdscr, TRUE); return 5;
            case 's': case 'S': nodelay(stdscr, TRUE); return 6;
            case 'p': case 'P':
                pomodoro_mode = !pomodoro_mode;
                pom_phase = POMODORO_WORK;
                pomodoro_count = 0;
                break;
            case 't': case 'T': nodelay(stdscr, TRUE); return 8;
            case 'n': case 'N': nodelay(stdscr, TRUE); return 9;
            case '7': if (has_save) { nodelay(stdscr, TRUE); return 7; } break;
            case 'q': case 'Q': return 0;
        }

        struct timespec ts = {0, 80000000L};
        nanosleep(&ts, NULL);
    }
}

/* ═══════════════════════════════════════════════════════════════
   TIME INPUT
   ═══════════════════════════════════════════════════════════════ */
int screen_time_input(int *out_seconds) {
    nodelay(stdscr, FALSE);
    echo(); curs_set(TRUE);
    char buf[32] = {0};
    int h = 0, m = 0, s = 0;

    erase();
    getmaxyx(stdscr, max_y, max_x);
    draw_decorative_border();

    attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
    mvprintw(max_y/2 - 6, (max_x - 24)/2, "  SET TIMER  ");
    attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

    attron(COLOR_PAIR(CP_UI));
    mvprintw(max_y/2 - 4, (max_x - 44)/2, "  Format: HH:MM:SS  (e.g. 01:30:00)    ");
    mvprintw(max_y/2 - 3, (max_x - 44)/2, "  Shortcuts: 25m, 1h, 90s, 1:30:00     ");
    mvprintw(max_y/2 - 1, (max_x - 20)/2, "  Time: ");
    attroff(COLOR_PAIR(CP_UI));

    if (pomodoro_mode) {
        const char *phase_str = (pom_phase == POMODORO_WORK)       ? "FOCUS (25min)"       :
                                (pom_phase == POMODORO_SHORT_BREAK) ? "SHORT BREAK (5min)"  :
                                                                      "LONG BREAK (15min)";
        attron(COLOR_PAIR(CP_POMODORO) | A_BOLD);
        mvprintw(max_y/2 + 1, (max_x - 44)/2, "  Pomodoro active - next: %s", phase_str);
        attroff(COLOR_PAIR(CP_POMODORO) | A_BOLD);
    }

    mvprintw(max_y/2 + 3, (max_x - 30)/2, "  [ENTER] Confirm  [ESC] Back  ");
    refresh();

    move(max_y/2 - 1, (max_x - 20)/2 + 8);
    if (getnstr(buf, 10) == OK) {
        int parsed = 0;
        if      (sscanf(buf, "%d:%d:%d", &h, &m, &s) == 3) { parsed = 1; }
        else if (sscanf(buf, "%d:%d",    &m, &s) == 2)      { h = 0; parsed = 1; }
        else {
            int val; char unit;
            if (sscanf(buf, "%d%c", &val, &unit) == 2) {
                if      (unit == 'm' || unit == 'M') { h=0; m=val; s=0; parsed=1; }
                else if (unit == 'h' || unit == 'H') { h=val; m=0; s=0; parsed=1; }
                else if (unit == 's' || unit == 'S') { h=0; m=0; s=val; parsed=1; }
            }
        }
        *out_seconds = parsed ? h*3600 + m*60 + s : -1;
    } else {
        *out_seconds = -1;
    }

    noecho(); curs_set(FALSE);
    nodelay(stdscr, TRUE);
    return (*out_seconds > 0) ? 1 : 0;
}

/* ═══════════════════════════════════════════════════════════════
   PRESETS
   ═══════════════════════════════════════════════════════════════ */
int screen_presets(int *out_seconds) {
    nodelay(stdscr, FALSE);
    int sel = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 16)/2, "  PRESETS  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        for (int i = 0; i < num_presets; i++) {
            int py = 5 + i*2;
            if (py >= max_y - 4) break;
            if (i == sel) attron(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attron(presets[i].is_pomodoro ? COLOR_PAIR(CP_POMODORO) : COLOR_PAIR(CP_UI));
            mvprintw(py, (max_x - 54)/2,
                     "  [%2d]  %-16s  %02d:%02d:%02d  (used %dx)%s  ",
                     i + 1, presets[i].label,
                     presets[i].h, presets[i].m, presets[i].s,
                     presets[i].count_used,
                     presets[i].is_pomodoro ? " [P]" : "    ");
            if (i == sel) attroff(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attroff(COLOR_PAIR(CP_UI) | COLOR_PAIR(CP_POMODORO));
        }

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2,
                 "[UP/DOWN] Navigate  [ENTER] Select  [E] Edit  [D] Delete  [N] New  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if      (ch == KEY_UP   && sel > 0)            sel--;
        else if (ch == KEY_DOWN && sel < num_presets-1) sel++;
        else if (ch == '\n' || ch == KEY_ENTER) {
            *out_seconds = presets[sel].h*3600 + presets[sel].m*60 + presets[sel].s;
            presets[sel].count_used++;
            nodelay(stdscr, TRUE);
            return 1;
        } else if (ch == 'e' || ch == 'E') {
            echo(); curs_set(TRUE);
            char nbuf[32] = {0};
            mvprintw(max_y - 2, 2, "New name: ");
            getnstr(nbuf, 31);
            if (strlen(nbuf) > 0) strncpy(presets[sel].label, nbuf, 31);
            noecho(); curs_set(FALSE);
        } else if ((ch == 'n' || ch == 'N') && num_presets < MAX_PRESETS) {
            echo(); curs_set(TRUE);
            char nbuf[32] = {0}, tbuf[16] = {0};
            int nh = 0, nm = 0, ns = 0;
            mvprintw(max_y - 2, 2, "Name: ");
            getnstr(nbuf, 31);
            mvprintw(max_y - 2, 2, "Time (HH:MM:SS): ");
            getnstr(tbuf, 10);
            sscanf(tbuf, "%d:%d:%d", &nh, &nm, &ns);
            if (strlen(nbuf) > 0 && (nh + nm + ns > 0)) {
                presets[num_presets].h = nh;
                presets[num_presets].m = nm;
                presets[num_presets].s = ns;
                strncpy(presets[num_presets].label, nbuf, 31);
                presets[num_presets].count_used  = 0;
                presets[num_presets].is_pomodoro = 0;
                num_presets++;
            }
            noecho(); curs_set(FALSE);
        } else if ((ch == 'd' || ch == 'D') && num_presets > 1) {
            memmove(&presets[sel], &presets[sel+1], sizeof(Preset)*(num_presets-sel-1));
            num_presets--;
            if (sel >= num_presets) sel = num_presets - 1;
        } else if (ch == 27) {
            nodelay(stdscr, TRUE);
            return 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   SETTINGS
   ═══════════════════════════════════════════════════════════════ */
int screen_settings(void) {
    nodelay(stdscr, FALSE);
    int sel = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 18)/2, "  SETTINGS  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        char goal_buf[32];
        snprintf(goal_buf, sizeof(goal_buf), "%dh%02dm",
                 global_stats.daily_goal_seconds / 3600,
                 (global_stats.daily_goal_seconds % 3600) / 60);

        struct { const char *name; const char *val; } opts[] = {
            {"Theme",           theme_names[current_theme]},
            {"Charset",         charset_names[charset]},
            {"Visualization",   viz_names[viz_mode]},
            {"Density",         density==1?"Sparse":density==2?"Normal":"Dense"},
            {"Show % in timer", show_percent    ? "On" : "Off"},
            {"Current clock",   show_clock      ? "On" : "Off"},
            {"Stats HUD",       show_stats_hud  ? "On" : "Off"},
            {"Show tasks",      show_tasks      ? "On" : "Off"},
            {"Motivational quotes", show_quotes ? "On" : "Off"},
            {"Pomodoro mode",   pomodoro_mode   ? "On" : "Off"},
            {"Alert",           alert_mode==ALERT_BEEP?"Beep":alert_mode==ALERT_VISUAL?"Visual":alert_mode==ALERT_BOTH?"Beep+Visual":"Silent"},
            {"Daily goal",      goal_buf},
        };
        int n = (int)(sizeof(opts) / sizeof(opts[0]));

        for (int i = 0; i < n; i++) {
            int py = 5 + i*2;
            if (py >= max_y - 4) break;
            if (i == sel) attron(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attron(COLOR_PAIR(CP_UI));
            mvprintw(py, (max_x - 46)/2, "  %-24s  %-16s  ", opts[i].name, opts[i].val);
            if (i == sel) attroff(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attroff(COLOR_PAIR(CP_UI));
        }

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2, "[UP/DOWN] Navigate  [LEFT/RIGHT] Change  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if      (ch == KEY_UP   && sel > 0)   sel--;
        else if (ch == KEY_DOWN && sel < n-1)  sel++;
        else if (ch == KEY_RIGHT || ch == KEY_LEFT) {
            int d = (ch == KEY_RIGHT) ? 1 : -1;
            switch (sel) {
                case 0:  current_theme = (Theme)((current_theme+d+THEME_COUNT)%THEME_COUNT);
                         apply_theme(current_theme); break;
                case 1:  charset   = (CharsetMode)((charset+d+CHARSET_COUNT)%CHARSET_COUNT); break;
                case 2:  viz_mode  = (VizMode)((viz_mode+d+VIZ_COUNT)%VIZ_COUNT); break;
                case 3:  density   = ((density-1+d+3)%3)+1; break;
                case 4:  show_percent    = !show_percent; break;
                case 5:  show_clock      = !show_clock; break;
                case 6:  show_stats_hud  = !show_stats_hud; break;
                case 7:  show_tasks      = !show_tasks; break;
                case 8:  show_quotes     = !show_quotes; break;
                case 9:  pomodoro_mode   = !pomodoro_mode; break;
                case 10: alert_mode = (AlertMode)((alert_mode+d+4)%4); break;
                case 11:
                    global_stats.daily_goal_seconds += d * 1800;
                    if (global_stats.daily_goal_seconds < 1800)  global_stats.daily_goal_seconds = 1800;
                    if (global_stats.daily_goal_seconds > 28800) global_stats.daily_goal_seconds = 28800;
                    break;
            }
        } else if (ch == 27) {
            nodelay(stdscr, TRUE);
            return 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   HISTORY
   ═══════════════════════════════════════════════════════════════ */
int screen_history(int *out_seconds) {
    nodelay(stdscr, FALSE);
    int sel = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 16)/2, "  HISTORY  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        if (history_count == 0) {
            attron(COLOR_PAIR(CP_TAIL) | A_DIM);
            mvprintw(max_y/2, (max_x - 26)/2, "  No sessions recorded yet  ");
            attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        }

        for (int i = 0; i < history_count; i++) {
            int py = 5 + i*2;
            if (py >= max_y - 4) break;
            int h2 = history[i].seconds / 3600;
            int m2 = (history[i].seconds % 3600) / 60;
            int s2 = history[i].seconds % 60;
            char qbuf[16] = "";
            struct tm *tmi = localtime(&history[i].when);
            if (tmi) strftime(qbuf, sizeof(qbuf), "%H:%M", tmi);
            const char *status = history[i].completed ? "[OK]" : "[--]";

            if (i == sel) attron(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attron(history[i].completed ? COLOR_PAIR(CP_TASK) : COLOR_PAIR(CP_UI));
            mvprintw(py, (max_x - 52)/2, "  %s  %-20s  %02d:%02d:%02d  @%s  ",
                     status, history[i].label, h2, m2, s2, qbuf);
            if (i == sel) attroff(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attroff(COLOR_PAIR(CP_TASK) | COLOR_PAIR(CP_UI));
        }

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2, "[ENTER] Repeat  [D] Clear history  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if      (ch == KEY_UP   && sel > 0)             sel--;
        else if (ch == KEY_DOWN && sel < history_count-1) sel++;
        else if ((ch == '\n' || ch == KEY_ENTER) && history_count > 0) {
            *out_seconds = history[sel].seconds;
            nodelay(stdscr, TRUE);
            return 1;
        } else if (ch == KEY_DC || ch == 'd' || ch == 'D') {
            history_count = 0; sel = 0;
        } else if (ch == 27) {
            nodelay(stdscr, TRUE);
            return 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   STATISTICS
   ═══════════════════════════════════════════════════════════════ */
int screen_statistics(void) {
    nodelay(stdscr, FALSE);
    check_daily_goal();

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 28)/2, "  STATISTICS & GOALS  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        int cy = 5, cx = (max_x - 50)/2;

        attron(COLOR_PAIR(CP_UI));
        mvprintw(cy++, cx, "  Sessions completed:    %d", global_stats.total_sessions);
        cy++;
        mvprintw(cy++, cx, "  Total time:            %dh %02dm %02ds",
                 global_stats.total_seconds / 3600,
                 (global_stats.total_seconds % 3600) / 60,
                 global_stats.total_seconds % 60);
        cy++;
        int avg = global_stats.total_sessions > 0
                  ? global_stats.total_seconds / global_stats.total_sessions : 0;
        mvprintw(cy++, cx, "  Average per session:   %dm %02ds", avg/60, avg%60);
        cy++;
        mvprintw(cy++, cx, "  Longest session:       %dh %02dm %02ds",
                 global_stats.longest_session / 3600,
                 (global_stats.longest_session % 3600) / 60,
                 global_stats.longest_session % 60);
        cy++;
        mvprintw(cy++, cx, "  Pomodoros completed:   %d (= %d hours of focus)",
                 global_stats.pomodoros_completed,
                 global_stats.pomodoros_completed * 25 / 60);
        attroff(COLOR_PAIR(CP_UI));
        cy++;

        attron(COLOR_PAIR(CP_GOAL) | A_BOLD);
        mvprintw(cy++, cx, "  --- DAILY GOAL ---");
        attroff(COLOR_PAIR(CP_GOAL) | A_BOLD);
        attron(COLOR_PAIR(CP_UI));
        int pct = global_stats.daily_goal_seconds > 0
                  ? (int)((float)global_stats.today_seconds / global_stats.daily_goal_seconds * 100) : 0;
        if (pct > 100) pct = 100;
        mvprintw(cy++, cx, "  Goal:     %dh%02dm",
                 global_stats.daily_goal_seconds / 3600,
                 (global_stats.daily_goal_seconds % 3600) / 60);
        mvprintw(cy++, cx, "  Today:    %dh%02dm",
                 global_stats.today_seconds / 3600,
                 (global_stats.today_seconds % 3600) / 60);
        mvprintw(cy++, cx, "  Progress: %d%%", pct);
        attroff(COLOR_PAIR(CP_UI));
        cy++;

        int bar_w   = 30;
        int h_done  = global_stats.total_seconds / 3600;
        int bar_fill = (h_done >= bar_w) ? bar_w : h_done;
        attron(COLOR_PAIR(CP_TAIL) | A_BOLD);
        mvprintw(cy, cx, "  Hours [");
        for (int i = 0; i < bar_w; i++) addch(i < bar_fill ? '#' : '.');
        printw("] %dh", h_done);
        attroff(COLOR_PAIR(CP_TAIL) | A_BOLD);
        cy += 2;

        if (global_stats.daily_goal_seconds > 0) {
            int goal_fill = (int)((float)global_stats.today_seconds / global_stats.daily_goal_seconds * bar_w);
            if (goal_fill > bar_w) goal_fill = bar_w;
            attron(COLOR_PAIR(CP_GOAL) | A_BOLD);
            mvprintw(cy, cx, "  Today  [");
            for (int i = 0; i < bar_w; i++) addch(i < goal_fill ? '=' : '.');
            printw("] %d%%", pct);
            attroff(COLOR_PAIR(CP_GOAL) | A_BOLD);
            cy += 2;
        }

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(cy++, cx, "  Top Presets:");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        int order[MAX_PRESETS];
        for (int i = 0; i < num_presets; i++) order[i] = i;
        for (int i = 0; i < num_presets - 1; i++)
            for (int j = i + 1; j < num_presets; j++)
                if (presets[order[j]].count_used > presets[order[i]].count_used) {
                    int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
                }
        attron(COLOR_PAIR(CP_UI));
        for (int i = 0; i < 3 && i < num_presets; i++) {
            int idx = order[i];
            mvprintw(cy++, cx + 2, "  %d. %-16s  %dx", i+1, presets[idx].label, presets[idx].count_used);
        }
        attroff(COLOR_PAIR(CP_UI));

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2, "[+/-] Adjust daily goal  [R] Reset stats  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if (ch == 27 || ch == 'q') { nodelay(stdscr, TRUE); return 0; }
        else if (ch == '+' || ch == '=') {
            global_stats.daily_goal_seconds += 1800;
            if (global_stats.daily_goal_seconds > 28800) global_stats.daily_goal_seconds = 28800;
        } else if (ch == '-') {
            global_stats.daily_goal_seconds -= 1800;
            if (global_stats.daily_goal_seconds < 1800) global_stats.daily_goal_seconds = 1800;
        } else if (ch == 'r' || ch == 'R') {
            global_stats.total_sessions     = 0;
            global_stats.total_seconds      = 0;
            global_stats.longest_session    = 0;
            global_stats.pomodoros_completed = 0;
            global_stats.today_seconds      = 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   TASKS SCREEN
   ═══════════════════════════════════════════════════════════════ */
int screen_tasks(void) {
    nodelay(stdscr, FALSE);
    int sel = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 14)/2, "  TASKS  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        int done_count = 0;
        for (int i = 0; i < task_count; i++) if (tasks[i].done) done_count++;

        attron(COLOR_PAIR(CP_TASK) | A_DIM);
        mvprintw(4, (max_x - 40)/2, "  %d/%d tasks complete", done_count, task_count);
        attroff(COLOR_PAIR(CP_TASK) | A_DIM);

        if (task_count == 0) {
            attron(COLOR_PAIR(CP_DIM) | A_DIM);
            mvprintw(max_y/2, (max_x - 30)/2, "  No tasks. Press [N] to add one.  ");
            attroff(COLOR_PAIR(CP_DIM) | A_DIM);
        }

        for (int i = 0; i < task_count; i++) {
            int py = 6 + i*2;
            if (py >= max_y - 5) break;
            const char *mark = tasks[i].done ? "[x]" : "[ ]";
            if (i == sel) {
                attron(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
                mvprintw(py, (max_x - 52)/2, "  %s  %-44s", mark, tasks[i].text);
                attroff(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            } else {
                attron(tasks[i].done ? (COLOR_PAIR(CP_DIM)|A_DIM) : COLOR_PAIR(CP_TASK));
                mvprintw(py, (max_x - 52)/2, "  %s  %-44s", mark, tasks[i].text);
                attroff(COLOR_PAIR(CP_TASK) | COLOR_PAIR(CP_DIM) | A_DIM);
            }
        }

        if (task_count > 0) {
            int bar_w  = 30;
            int filled = (int)((float)done_count / task_count * bar_w);
            attron(COLOR_PAIR(CP_TASK) | A_BOLD);
            mvprintw(max_y - 5, (max_x - bar_w - 4)/2, "[");
            for (int i = 0; i < bar_w; i++) addch(i < filled ? '#' : '.');
            printw("] %d%%", (int)((float)done_count / task_count * 100));
            attroff(COLOR_PAIR(CP_TASK) | A_BOLD);
        }

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2,
                 "[SPACE] Toggle  [N] New  [D] Delete  [C] Clear done  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if      (ch == KEY_UP   && sel > 0)            sel--;
        else if (ch == KEY_DOWN && sel < task_count-1) sel++;
        else if (ch == ' ' && task_count > 0) {
            tasks[sel].done = !tasks[sel].done;
            if (tasks[sel].done) tasks[sel].session_id = session_count;
            save_tasks();
        } else if (ch == 'n' || ch == 'N') {
            if (task_count < MAX_TASKS) {
                echo(); curs_set(TRUE);
                char tbuf[64] = {0};
                mvprintw(max_y - 2, 2, "New task: ");
                getnstr(tbuf, 63);
                if (strlen(tbuf) > 0) {
                    strncpy(tasks[task_count].text, tbuf, 63);
                    tasks[task_count].done       = 0;
                    tasks[task_count].session_id = -1;
                    task_count++;
                    save_tasks();
                }
                noecho(); curs_set(FALSE);
            }
        } else if ((ch == 'd' || ch == 'D') && task_count > 0) {
            memmove(&tasks[sel], &tasks[sel+1], sizeof(Task)*(task_count-sel-1));
            task_count--;
            if (sel >= task_count && sel > 0) sel--;
            save_tasks();
        } else if (ch == 'c' || ch == 'C') {
            int n2 = 0;
            for (int i = 0; i < task_count; i++)
                if (!tasks[i].done) tasks[n2++] = tasks[i];
            task_count = n2;
            sel = 0;
            save_tasks();
        } else if (ch == 27) {
            nodelay(stdscr, TRUE);
            return 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   NOTES SCREEN
   ═══════════════════════════════════════════════════════════════ */
int screen_notes(void) {
    nodelay(stdscr, FALSE);
    int sel = 0;

    while (1) {
        erase();
        getmaxyx(stdscr, max_y, max_x);
        draw_decorative_border();

        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvprintw(2, (max_x - 18)/2, "  QUICK NOTES  ");
        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

        if (note_count == 0) {
            attron(COLOR_PAIR(CP_DIM) | A_DIM);
            mvprintw(max_y/2, (max_x - 32)/2, "  No notes. Press [N] to create one.  ");
            attroff(COLOR_PAIR(CP_DIM) | A_DIM);
        }

        for (int i = 0; i < note_count; i++) {
            int py = 5 + i*3;
            if (py >= max_y - 5) break;
            char tbuf[16] = "";
            struct tm *tmi = localtime(&notes[i].when);
            if (tmi) strftime(tbuf, sizeof(tbuf), "%d/%m %H:%M", tmi);
            if (i == sel) attron(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attron(COLOR_PAIR(CP_NOTE));
            mvprintw(py, (max_x - 60)/2, "  [%s] %.52s", tbuf, notes[i].text);
            if (i == sel) attroff(COLOR_PAIR(CP_HEAD) | A_BOLD | A_REVERSE);
            else          attroff(COLOR_PAIR(CP_NOTE));
        }

        attron(COLOR_PAIR(CP_TAIL) | A_DIM);
        mvprintw(max_y - 3, 2, "[N] New note  [D] Delete  [ESC] Back");
        attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
        refresh();

        int ch = getch();
        if      (ch == KEY_UP   && sel > 0)           sel--;
        else if (ch == KEY_DOWN && sel < note_count-1) sel++;
        else if (ch == 'n' || ch == 'N') {
            if (note_count < MAX_NOTES) {
                echo(); curs_set(TRUE);
                char tbuf[128] = {0};
                mvprintw(max_y - 2, 2, "Note: ");
                getnstr(tbuf, 127);
                if (strlen(tbuf) > 0) {
                    if (note_count >= MAX_NOTES) {
                        memmove(notes, notes+1, sizeof(Note)*(MAX_NOTES-1));
                        note_count = MAX_NOTES - 1;
                    }
                    strncpy(notes[note_count].text, tbuf, 127);
                    notes[note_count].when = time(NULL);
                    note_count++;
                    save_notes();
                }
                noecho(); curs_set(FALSE);
            }
        } else if ((ch == 'd' || ch == 'D') && note_count > 0) {
            memmove(&notes[sel], &notes[sel+1], sizeof(Note)*(note_count-sel-1));
            note_count--;
            if (sel >= note_count && sel > 0) sel--;
            save_notes();
        } else if (ch == 27) {
            nodelay(stdscr, TRUE);
            return 0;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   COUNTDOWN FINAL SCREEN
   ═══════════════════════════════════════════════════════════════ */
void screen_countdown_final(const char *label) {
    nodelay(stdscr, TRUE);

    if (alert_mode == ALERT_BEEP || alert_mode == ALERT_BOTH) {
        for (int i = 0; i < 3; i++) {
            beep();
            struct timespec ts = {0, 200000000L};
            nanosleep(&ts, NULL);
        }
    }

    const char *msgs[] = {"3", "2", "1", "TIME IS UP!"};
    for (int i = 0; i < 4; i++) {
        erase();

        if (i == 3 && (alert_mode == ALERT_VISUAL || alert_mode == ALERT_BOTH)) {
            for (int y = 0; y < max_y; y++)
                for (int x = 0; x < max_x; x++) {
                    attron(COLOR_PAIR(CP_ALERT) | A_REVERSE);
                    mvaddch(y, x, random_char());
                    attroff(COLOR_PAIR(CP_ALERT) | A_REVERSE);
                }
        }

        attron(COLOR_PAIR(CP_ALERT) | A_BOLD | A_BLINK);
        int len = (int)strlen(msgs[i]);
        mvprintw(max_y/2, (max_x - len)/2, "%s", msgs[i]);
        attroff(COLOR_PAIR(CP_ALERT) | A_BOLD | A_BLINK);

        if (i == 3) {
            if (label) {
                attron(COLOR_PAIR(CP_UI));
                mvprintw(max_y/2 + 2, (max_x - (int)strlen(label))/2, "%s", label);
                attroff(COLOR_PAIR(CP_UI));
            }

            attron(COLOR_PAIR(CP_TASK) | A_BOLD);
            mvprintw(max_y/2 + 4, (max_x - 42)/2,
                     "  Session #%d complete!  Today total: %dm  ",
                     global_stats.total_sessions,
                     global_stats.today_seconds / 60);
            attroff(COLOR_PAIR(CP_TASK) | A_BOLD);

            if (pomodoro_mode) {
                int prox = next_pomodoro();
                const char *nxt = (pom_phase == POMODORO_WORK)        ? "Time to focus!"      :
                                  (pom_phase == POMODORO_SHORT_BREAK)  ? "Take a 5min break!"  :
                                                                         "Take a 15min break!";
                attron(COLOR_PAIR(CP_POMODORO) | A_BOLD);
                mvprintw(max_y/2 + 6, (max_x - 32)/2, " [Pom] %s (%dm) ", nxt, prox/60);
                attroff(COLOR_PAIR(CP_POMODORO) | A_BOLD);
            }

            const char *q = quotes[rand() % QUOTE_COUNT];
            int qlen = (int)strlen(q);
            if (max_x > qlen + 4) {
                attron(COLOR_PAIR(CP_DIM) | A_ITALIC);
                mvprintw(max_y/2 + 8, (max_x - qlen)/2, "\"%s\"", q);
                attroff(COLOR_PAIR(CP_DIM) | A_ITALIC);
            }
        }

        attron(COLOR_PAIR(CP_UI) | A_DIM);
        mvprintw(max_y/2 + 10, (max_x - 40)/2, "[R] Restart  [P] Next (Pom)  [Q] Quit");
        attroff(COLOR_PAIR(CP_UI) | A_DIM);
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q' || ch == 'r' || ch == 'R' || ch == 'p' || ch == 'P') {
            ungetch(ch); return;
        }

        if (i < 3) {
            beep();
            struct timespec ts = {0, 700000000L};
            nanosleep(&ts, NULL);
        } else {
            nodelay(stdscr, FALSE);
            int c2 = getch();
            ungetch(c2);
            nodelay(stdscr, TRUE);
        }
    }
}
