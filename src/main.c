#define _POSIX_C_SOURCE 200809L
#include "matrix_timer.h"
#include "display.h"
#include "screens.h"
#include "timer.h"
#include "storage.h"





/* ─── main.c = Thin entry point: loads data, shows menu, dispatches actions ────────────────────────────────────── */




int main(void) {
    ncurses_init();
    getmaxyx(stdscr, max_y, max_x);

    load_tasks();
    load_notes();
    check_daily_goal();

    srand((unsigned)time(NULL));
    current_quote = rand() % QUOTE_COUNT;

    while (1) {
        int action = screen_main_menu();
        int seconds = 0;
        char label[32] = "Custom";

        if (action == 0) break;

        switch (action) {
            case 1: {
                if (!screen_time_input(&seconds) || seconds <= 0) {
                    ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
                }
                snprintf(label, sizeof(label), "Manual");
                break;
            }
            case 2: {
                if (!screen_presets(&seconds)) {
                    ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
                }
                snprintf(label, sizeof(label), "Preset");
                break;
            }
            case 3: {
                screen_settings();
                ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
            }
            case 4: {
                if (!screen_history(&seconds) || history_count == 0) {
                    ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
                }
                snprintf(label, sizeof(label), "Repeated");
                break;
            }
            case 5: {
                screen_statistics();
                ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
            }
            case 6: {
                endwin();
                stopwatch_loop();
                ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
            }
            case 7: {
                int r = 0, t = 0;
                char lb[32] = {0};
                if (!load_state(&r, &t, lb)) {
                    ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
                }
                seconds = t;
                strncpy(label, lb, 31);
                break;
            }
            case 8: {
                screen_tasks();
                ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
            }
            case 9: {
                screen_notes();
                ncurses_init(); getmaxyx(stdscr, max_y, max_x); continue;
            }
        }

        /* Pomodoro auto-timing when no time was explicitly set */
        if (pomodoro_mode && action != 1 && action != 7 && seconds == 0) {
            if      (pom_phase == POMODORO_WORK)        seconds = 25 * 60;
            else if (pom_phase == POMODORO_SHORT_BREAK) seconds = 5  * 60;
            else                                        seconds = 15 * 60;
        }

        if (seconds > 0) {
            endwin();
            timer_loop(seconds, label);
            ncurses_init();
            getmaxyx(stdscr, max_y, max_x);
        }
    }

    save_tasks();
    save_notes();
    endwin();

    /* Goodbye summary */
    printf("\n\033[1;32m");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║      MATRIX TIMER v%s  -  Goodbye!        ║\n", VERSION);
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  Sessions: %-5d  Pomodoros: %-5d          ║\n",
           global_stats.total_sessions, global_stats.pomodoros_completed);
    printf("║  Total time:  %3dh %02dm                     ║\n",
           global_stats.total_seconds / 3600,
           (global_stats.total_seconds % 3600) / 60);
    printf("║  Today:  %3dh %02dm / goal %3dh %02dm        ║\n",
           global_stats.today_seconds / 3600,
           (global_stats.today_seconds % 3600) / 60,
           global_stats.daily_goal_seconds / 3600,
           (global_stats.daily_goal_seconds % 3600) / 60);
    printf("╚════════════════════════════════════════════╝\n");
    printf("\033[0m\n");
    return 0;
}

