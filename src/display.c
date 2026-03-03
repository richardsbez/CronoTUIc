
 #include "display.h"



/* ─── display.c = ncurses init, theme application, shared HUD elements (clock, goal bar, tasks overlay, quotes) ────────────────────────────────────── */



/* ═══════════════════════════════════════════════════════════════
   NCURSES INIT & THEME
   ═══════════════════════════════════════════════════════════════ */
void ncurses_init(void) {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    use_default_colors();
    apply_theme(current_theme);
}

void apply_theme(Theme t) {
    current_theme = t;
    short fg_tail, fg_head, fg_alert, fg_glitch;

    switch (t) {
        case THEME_BLUE:
            fg_tail=COLOR_BLUE;    fg_head=COLOR_CYAN;    fg_alert=COLOR_RED;     fg_glitch=COLOR_WHITE;   break;
        case THEME_RED:
            fg_tail=COLOR_RED;     fg_head=COLOR_WHITE;   fg_alert=COLOR_YELLOW;  fg_glitch=COLOR_MAGENTA; break;
        case THEME_GOLD:
            fg_tail=COLOR_YELLOW;  fg_head=COLOR_WHITE;   fg_alert=COLOR_RED;     fg_glitch=COLOR_CYAN;    break;
        case THEME_CYAN:
            fg_tail=COLOR_CYAN;    fg_head=COLOR_WHITE;   fg_alert=COLOR_RED;     fg_glitch=COLOR_YELLOW;  break;
        case THEME_PURPLE:
            fg_tail=COLOR_MAGENTA; fg_head=COLOR_WHITE;   fg_alert=COLOR_RED;     fg_glitch=COLOR_CYAN;    break;
        case THEME_RAINBOW:
            fg_tail=COLOR_GREEN;   fg_head=COLOR_WHITE;   fg_alert=COLOR_RED;     fg_glitch=COLOR_CYAN;    break;
        case THEME_HACKER:
            fg_tail=COLOR_GREEN;   fg_head=COLOR_GREEN;   fg_alert=COLOR_RED;     fg_glitch=COLOR_WHITE;   break;
        case THEME_SUNSET:
            fg_tail=COLOR_RED;     fg_head=COLOR_YELLOW;  fg_alert=COLOR_MAGENTA; fg_glitch=COLOR_WHITE;   break;
        default:
            fg_tail=COLOR_GREEN;   fg_head=COLOR_WHITE;   fg_alert=COLOR_RED;     fg_glitch=COLOR_CYAN;    break;
    }

    init_pair(CP_TAIL,     fg_tail,       -1);
    init_pair(CP_HEAD,     fg_head,       -1);
    init_pair(CP_UI,       COLOR_CYAN,    -1);
    init_pair(CP_PAUSE,    COLOR_YELLOW,  -1);
    init_pair(CP_ALERT,    fg_alert,      -1);
    init_pair(CP_DIM,      COLOR_BLACK,   -1);
    init_pair(CP_PROGRESS, fg_tail,       -1);
    init_pair(CP_TITLE,    fg_head,       -1);
    init_pair(CP_GLITCH,   fg_glitch,     -1);
    init_pair(CP_CLOCK,    COLOR_WHITE,   -1);
    init_pair(CP_R1,       COLOR_RED,     -1);
    init_pair(CP_R2,       COLOR_YELLOW,  -1);
    init_pair(CP_R3,       COLOR_GREEN,   -1);
    init_pair(CP_R4,       COLOR_CYAN,    -1);
    init_pair(CP_R5,       COLOR_MAGENTA, -1);
    init_pair(CP_STATS,    COLOR_WHITE,   -1);
    init_pair(CP_TASK,     COLOR_GREEN,   -1);
    init_pair(CP_NOTE,     COLOR_YELLOW,  -1);
    init_pair(CP_POMODORO, COLOR_RED,     -1);
    init_pair(CP_GOAL,     COLOR_CYAN,    -1);
    init_pair(CP_FIREFLY,  COLOR_YELLOW,  -1);
}

/* ═══════════════════════════════════════════════════════════════
   SHARED COLUMN HELPERS
   ═══════════════════════════════════════════════════════════════ */
void reset_column(RainColumn *col, int x_pos) {
    col->x          = x_pos;
    col->y          = -(rand() % 25);
    col->speed      = 1 + (rand() % 3);
    col->tail_len   = MIN_TAIL + rand() % (MAX_TAIL - MIN_TAIL + 1);
    col->counter    = 0;
    col->glitch     = rand() % 8 == 0;
    col->angle      = ((float)(rand() % 628)) / 100.0f;
    col->radius     = 1.0f + (rand() % 20);
    col->color_offset = rand() % 5;
    col->z          = 0.5f + ((float)(rand() % 100)) / 100.0f;
    col->vx         = ((float)(rand() % 20 - 10)) / 10.0f;
    col->vy         = ((float)(rand() % 20 - 10)) / 10.0f;
    col->brightness = 50 + rand() % 50;
}

char random_char(void) {
    static const char *charsets[] = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$+-*/=%\"#&_(),.;:?!\\|{}<>[]@^~",
        "0123456789+-*/=<>",
        "01",
        "ｦｧｨｩｪｫｬｭｮｯｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝ",
        "⠁⠂⠃⠄⠅⠆⠇⠈⠉⠊⠋⠌⠍⠎⠏⠐⠑⠒⠓⠔⠕⠖⠗⠘⠙⠚⠛⠜⠝⠞⠟",
        "0123456789ABCDEF",
        "ACGT",
        "ᚠᚡᚢᚣᚤᚥᚦᚧᚨᚩᚪᚫᚬᚭᚮᚯᚰᚱᚲᚳᚴᚵᚶᚷᚸᚹᚺᚻᚼᚽᚾᚿᛀᛁᛂᛃᛄᛅᛆᛇᛈᛉᛊᛋᛌᛍᛎᛏᛐᛑᛒᛓ",
        "★☆◆◇●○▲△▼▽■□♦♣♠♥♡♢⚡⚽⚾⛄⛅☀☁☂☃❄❅❆",
    };
    const char *s = charsets[charset];
    int len = (int)strlen(s);
    if (len == 0) return '?';
    return s[rand() % len];
}

/* ═══════════════════════════════════════════════════════════════
   DECORATIVE BORDER
   ═══════════════════════════════════════════════════════════════ */
void draw_decorative_border(void) {
    attron(COLOR_PAIR(CP_TAIL) | A_DIM);
    for (int x = 0; x < max_x; x++) {
        mvaddch(0, x, '-');
        mvaddch(max_y-1, x, '-');
    }
    for (int y = 0; y < max_y; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, max_x-1, '|');
    }
    mvaddch(0, 0, '+');          mvaddch(0, max_x-1, '+');
    mvaddch(max_y-1, 0, '+');    mvaddch(max_y-1, max_x-1, '+');
    attroff(COLOR_PAIR(CP_TAIL) | A_DIM);
}

/* ═══════════════════════════════════════════════════════════════
   BIG CLOCK
   ═══════════════════════════════════════════════════════════════ */
void draw_big_clock(int start_y, int start_x, int remaining) {
    int h = remaining / 3600;
    int m = (remaining % 3600) / 60;
    int s = remaining % 60;
    int digits[6] = { h/10, h%10, m/10, m%10, s/10, s%10 };
    int alert = (remaining <= ALERT_THRESHOLD && remaining > 0);

    for (int row = 0; row < 5; row++) {
        int cx = start_x;
        for (int d = 0; d < 6; d++) {
            if (alert) attron(COLOR_PAIR(CP_ALERT) | A_BOLD);
            else        attron(COLOR_PAIR(CP_TITLE) | A_BOLD);

            if (start_y + row >= 0 && start_y + row < max_y && cx >= 0 && cx < max_x - 5)
                mvprintw(start_y + row, cx, "%s", big_digits[digits[d]][row]);
            cx += 5;

            if (alert) attroff(COLOR_PAIR(CP_ALERT) | A_BOLD);
            else        attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

            if (d == 1 || d == 3) {
                attron(COLOR_PAIR(CP_UI) | A_BOLD);
                mvprintw(start_y + row, cx, (row == 1 || row == 3) ? ":" : " ");
                cx += 2;
                attroff(COLOR_PAIR(CP_UI) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   HUD ELEMENTS
   ═══════════════════════════════════════════════════════════════ */
void draw_current_clock(void) {
    if (!show_clock) return;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    attron(COLOR_PAIR(CP_DIM) | A_DIM);
    mvprintw(0, max_x - (int)strlen(buf) - 2, "%s", buf);
    attroff(COLOR_PAIR(CP_DIM) | A_DIM);
}

void draw_hud_stats(int remaining, int total) {
    if (!show_stats_hud) return;
    (void)remaining; (void)total;
    attron(COLOR_PAIR(CP_STATS) | A_DIM);
    mvprintw(1, 2, "Sessions:%d  Total:%dh%02dm  Today:%dm/%dm  Poms:%d",
             global_stats.total_sessions,
             global_stats.total_seconds / 3600,
             (global_stats.total_seconds % 3600) / 60,
             global_stats.today_seconds / 60,
             global_stats.daily_goal_seconds / 60,
             global_stats.pomodoros_completed);
    attroff(COLOR_PAIR(CP_STATS) | A_DIM);
}

void draw_goal_bar(void) {
    if (global_stats.daily_goal_seconds <= 0) return;
    int prog  = global_stats.today_seconds;
    int goal  = global_stats.daily_goal_seconds;
    int bar_w = 20;
    int filled = (prog >= goal) ? bar_w : (int)((float)prog / goal * bar_w);

    attron(COLOR_PAIR(CP_GOAL) | A_DIM);
    mvprintw(0, 2, "GOAL[");
    attroff(COLOR_PAIR(CP_GOAL) | A_DIM);

    for (int i = 0; i < bar_w; i++) {
        if (i < filled) { attron(COLOR_PAIR(CP_GOAL) | A_BOLD);  addch('='); }
        else             { attron(COLOR_PAIR(CP_DIM)  | A_DIM);   addch('.'); }
        attroff(COLOR_PAIR(CP_GOAL) | COLOR_PAIR(CP_DIM) | A_BOLD | A_DIM);
    }

    attron(COLOR_PAIR(CP_GOAL) | A_DIM);
    printw("]%d%%", (int)((float)prog / goal * 100));
    attroff(COLOR_PAIR(CP_GOAL) | A_DIM);
}

void draw_tasks_hud(void) {
    if (!show_tasks || task_count == 0) return;
    int start_y = max_y - 7 - task_count;
    if (start_y < 3) start_y = 3;

    attron(COLOR_PAIR(CP_TASK) | A_DIM);
    mvprintw(start_y, max_x - 30, "── TASKS ──");
    attroff(COLOR_PAIR(CP_TASK) | A_DIM);

    for (int i = 0; i < task_count && i < 5; i++) {
        int py = start_y + 1 + i;
        if (py >= max_y - 5) break;
        if (tasks[i].done) {
            attron(COLOR_PAIR(CP_DIM) | A_DIM);
            mvprintw(py, max_x - 29, "[x] %.22s", tasks[i].text);
        } else {
            attron(COLOR_PAIR(CP_TASK));
            mvprintw(py, max_x - 29, "[ ] %.22s", tasks[i].text);
        }
        attroff(COLOR_PAIR(CP_TASK) | COLOR_PAIR(CP_DIM) | A_DIM);
    }
}

void draw_quote(void) {
    if (!show_quotes) return;
    quote_timer++;
    if (quote_timer > 500) {
        quote_timer = 0;
        current_quote = (current_quote + 1) % QUOTE_COUNT;
    }
    const char *q = quotes[current_quote];
    int len = (int)strlen(q);
    int qx = (max_x - len) / 2;
    if (qx < 1) qx = 1;
    int qy = max_y - 5;
    if (qy < 2) return;
    attron(COLOR_PAIR(CP_DIM) | A_ITALIC);
    mvprintw(qy, qx, "\"%s\"", q);
    attroff(COLOR_PAIR(CP_DIM) | A_ITALIC);
}

void draw_pomodoro_hud(void) {
    if (!pomodoro_mode) return;
    const char *phase_names[] = {"FOCUS", "SHORT BREAK", "LONG BREAK"};
    attron(COLOR_PAIR(CP_POMODORO) | A_BOLD);
    mvprintw(0, max_x / 2 - 8, " [%s] #%d ", phase_names[pom_phase], pomodoro_count);
    attroff(COLOR_PAIR(CP_POMODORO) | A_BOLD);
}

