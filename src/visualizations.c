#include "visualizations.h"
#include "display.h"



/* ─── visualizations.c = All 8 visual effects (Rain, Vortex, Pulse, Wave, Static, 3D, Fractal, Firefly) + a single dispatch function ────────────────────────────────────── */



/* ═══════════════════════════════════════════════════════════════
   CLASSIC MATRIX RAIN
   ═══════════════════════════════════════════════════════════════ */
void draw_rain(RainColumn *cols, int count, int frame) {
    rainbow_frame = frame;
    for (int i = 0; i < count; i++) {
        cols[i].counter++;
        if (cols[i].counter >= cols[i].speed) {
            cols[i].y++;
            cols[i].counter = 0;
        }
        if (cols[i].y - cols[i].tail_len > max_y)
            reset_column(&cols[i], i * density);

        for (int j = 0; j < cols[i].tail_len; j++) {
            int dy = cols[i].y - j;
            if (dy >= 1 && dy < max_y - 4) {
                char c = random_char();
                if (cols[i].glitch && rand() % 3 == 0) {
                    attron(COLOR_PAIR(CP_GLITCH) | A_BOLD);
                    mvaddch(dy, cols[i].x, c);
                    attroff(COLOR_PAIR(CP_GLITCH) | A_BOLD);
                } else if (j == 0) {
                    attron(COLOR_PAIR(CP_HEAD) | A_BOLD);
                    mvaddch(dy, cols[i].x, c);
                    attroff(COLOR_PAIR(CP_HEAD) | A_BOLD);
                } else if (j < 3) {
                    int rp = (current_theme == THEME_RAINBOW)
                             ? CP_R1 + ((i + rainbow_frame / 5) % 5)
                             : CP_TAIL;
                    attron(COLOR_PAIR(rp) | A_BOLD);
                    mvaddch(dy, cols[i].x, c);
                    attroff(COLOR_PAIR(rp) | A_BOLD);
                } else {
                    int pair = (j > cols[i].tail_len * 2 / 3) ? CP_DIM : CP_TAIL;
                    attron(COLOR_PAIR(pair));
                    mvaddch(dy, cols[i].x, c);
                    attroff(COLOR_PAIR(pair));
                }
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   VORTEX
   ═══════════════════════════════════════════════════════════════ */
void draw_vortex(RainColumn *cols, int count, int frame) {
    float t = (float)frame * 0.05f;
    int cx = max_x / 2;
    int cy = (max_y - 4) / 2 + 1;

    for (int i = 0; i < count; i++) {
        cols[i].angle  += 0.03f + cols[i].speed * 0.01f;
        cols[i].radius += 0.2f;
        if (cols[i].radius > (float)(max_x / 2)) {
            cols[i].radius = 1.0f + (rand() % 5);
            cols[i].angle  = ((float)(rand() % 628)) / 100.0f;
        }
        int px = (int)(cx + cols[i].radius * cosf(cols[i].angle + t));
        int py = (int)(cy + cols[i].radius * 0.5f * sinf(cols[i].angle + t));
        if (px >= 1 && px < max_x-1 && py >= 1 && py < max_y-3) {
            char c = random_char();
            int depth = (int)cols[i].radius;
            int pair = (current_theme == THEME_RAINBOW)
                       ? CP_R1 + (i % 5)
                       : (depth < 5) ? CP_HEAD : (depth < 15) ? CP_TAIL : CP_DIM;
            attron(COLOR_PAIR(pair) | (depth < 5 ? A_BOLD : 0));
            mvaddch(py, px, c);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   PULSE
   ═══════════════════════════════════════════════════════════════ */
void draw_pulse(int frame, int remaining, int total) {
    int cx = max_x / 2;
    int cy = (max_y - 4) / 2 + 1;
    float progress = (total > 0) ? (float)remaining / total : 1.0f;
    float pulse = (float)frame * 0.15f;

    for (int y = 1; y < max_y - 3; y++) {
        for (int x = 1; x < max_x - 1; x++) {
            float dist = sqrtf((float)((x-cx)*(x-cx)) + (float)((y-cy)*(y-cy)) * 4.0f);
            float wave = sinf(dist * 0.3f - pulse);
            float thresh = 0.5f + progress * 0.4f;
            if (wave > thresh) {
                char c = random_char();
                int intensity = (int)((wave - thresh) * 10);
                int pair = (current_theme == THEME_RAINBOW)
                           ? CP_R1 + (int)(dist / 5) % 5
                           : (intensity > 3) ? CP_HEAD : (intensity > 1) ? CP_TAIL : CP_DIM;
                attron(COLOR_PAIR(pair) | (intensity > 4 ? A_BOLD : 0));
                mvaddch(y, x, c);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   WAVE
   ═══════════════════════════════════════════════════════════════ */
void draw_wave(RainColumn *cols, int count, int frame) {
    float t = (float)frame * 0.05f;
    for (int i = 0; i < count; i++) {
        int wave_offset = (int)(sinf((float)i * 0.3f + t) * (max_y / 4));
        int base_y = max_y / 2 + wave_offset;
        for (int j = 0; j < cols[i].tail_len / 2; j++) {
            int dy = base_y - j;
            if (dy >= 1 && dy < max_y - 3) {
                char c = random_char();
                int pair = (current_theme == THEME_RAINBOW)
                           ? CP_R1 + ((i + j) % 5)
                           : (j == 0) ? CP_HEAD : (j < 3) ? CP_TAIL : CP_DIM;
                attron(COLOR_PAIR(pair) | (j < 2 ? A_BOLD : 0));
                mvaddch(dy, cols[i].x, c);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   STATIC
   ═══════════════════════════════════════════════════════════════ */
void draw_static(int frame) {
    (void)frame;
    for (int y = 1; y < max_y - 3; y++) {
        for (int x = 1; x < max_x - 1; x++) {
            if (rand() % 3 == 0) {
                char c = random_char();
                int r = rand() % 10;
                int pair = (current_theme == THEME_RAINBOW)
                           ? CP_R1 + (rand() % 5)
                           : (r > 7) ? CP_HEAD : (r > 4) ? CP_TAIL : CP_DIM;
                attron(COLOR_PAIR(pair) | (r > 8 ? A_BOLD : 0));
                mvaddch(y, x, c);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   MATRIX 3D (perspective)
   ═══════════════════════════════════════════════════════════════ */
void draw_matrix3d(RainColumn *cols, int count, int frame) {
    int cx = max_x / 2;
    int cy = (max_y - 4) / 2;
    (void)frame;

    for (int i = 0; i < count; i++) {
        cols[i].counter++;
        if (cols[i].counter >= cols[i].speed) {
            cols[i].y++;
            cols[i].counter = 0;
        }
        if (cols[i].y - cols[i].tail_len > max_y)
            reset_column(&cols[i], i * density);

        float dx = (float)(cols[i].x - cx) / cx;
        float perspective = 1.0f - 0.5f * fabsf(dx);

        for (int j = 0; j < cols[i].tail_len; j++) {
            int dy = cols[i].y - j;
            int screen_y = cy + (int)((dy - cy) * perspective);
            if (screen_y >= 1 && screen_y < max_y - 4 &&
                cols[i].x >= 1 && cols[i].x < max_x - 1) {
                char c = random_char();
                int pair;
                if      (j == 0)             pair = CP_HEAD;
                else if (perspective > 0.8f) pair = CP_TAIL;
                else if (perspective > 0.5f) pair = CP_DIM;
                else                         continue;
                attron(COLOR_PAIR(pair) | (j < 2 ? A_BOLD : 0));
                mvaddch(screen_y, cols[i].x, c);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   FRACTAL
   ═══════════════════════════════════════════════════════════════ */
void draw_fractal(int frame) {
    float t      = (float)frame * 0.02f;
    float zoom   = 1.5f + sinf(t * 0.3f) * 0.5f;
    float cx_f   = -0.5f + cosf(t * 0.1f) * 0.3f;
    float cy_f   =  0.0f + sinf(t * 0.07f) * 0.3f;
    int max_iter = 8;

    for (int y = 1; y < max_y - 4; y++) {
        for (int x = 1; x < max_x - 1; x++) {
            float real = cx_f + ((float)(x - max_x/2)) / (max_x * zoom);
            float imag = cy_f + ((float)(y - max_y/2)) / (max_y * zoom * 2);
            float zr = 0, zi = 0;
            int iter = 0;
            while (zr*zr + zi*zi < 4 && iter < max_iter) {
                float tmp = zr*zr - zi*zi + real;
                zi = 2*zr*zi + imag;
                zr = tmp;
                iter++;
            }
            if (iter < max_iter) {
                char c = random_char();
                int pair = (current_theme == THEME_RAINBOW)
                           ? CP_R1 + (iter % 5)
                           : (iter > 5) ? CP_HEAD : (iter > 2) ? CP_TAIL : CP_DIM;
                attron(COLOR_PAIR(pair) | (iter > 6 ? A_BOLD : 0));
                mvaddch(y, x, c);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   FIREFLY
   ═══════════════════════════════════════════════════════════════ */
void draw_firefly(RainColumn *cols, int count, int frame) {
    for (int i = 0; i < count; i++) {
        cols[i].radius += cols[i].vx * 0.3f;
        cols[i].angle  += cols[i].vy * 0.05f;

        if ((int)cols[i].radius < 1 || (int)cols[i].radius >= max_x - 1)
            cols[i].vx = -cols[i].vx;

        float fy = (float)max_y / 2 + sinf(cols[i].angle) * ((float)max_y / 3);
        if (fy < 1 || fy >= max_y - 4)
            cols[i].vy = -cols[i].vy;

        cols[i].brightness = (int)(50 + 50 * sinf((float)frame * 0.1f + cols[i].color_offset));

        int px = (int)cols[i].radius;
        int py = (int)fy;

        if (px >= 1 && px < max_x-1 && py >= 1 && py < max_y-4) {
            int bright = cols[i].brightness;
            int pair;
            char c;
            if      (bright > 80) { pair = CP_HEAD;    c = '*'; }
            else if (bright > 50) { pair = CP_FIREFLY;  c = '+'; }
            else if (bright > 20) { pair = CP_TAIL;     c = '.'; }
            else                  { pair = CP_DIM;      c = ' '; }

            if (current_theme == THEME_RAINBOW) pair = CP_R1 + (i % 5);

            attron(COLOR_PAIR(pair) | (bright > 70 ? A_BOLD : 0));
            mvaddch(py, px, c);
            attroff(COLOR_PAIR(pair) | A_BOLD);

            if (bright > 60 && py + 1 < max_y - 4) {
                attron(COLOR_PAIR(CP_DIM) | A_DIM);
                mvaddch(py + 1, px, '.');
                attroff(COLOR_PAIR(CP_DIM) | A_DIM);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
   DISPATCH
   ═══════════════════════════════════════════════════════════════ */
void draw_visualization(RainColumn *cols, int count, int frame,
                         int remaining, int total) {
    switch (viz_mode) {
        case VIZ_RAIN:     draw_rain(cols, count, frame);              break;
        case VIZ_VORTEX:   draw_vortex(cols, count, frame);            break;
        case VIZ_PULSE:    draw_pulse(frame, remaining, total);         break;
        case VIZ_WAVE:     draw_wave(cols, count, frame);              break;
        case VIZ_STATIC:   draw_static(frame);                          break;
        case VIZ_MATRIX3D: draw_matrix3d(cols, count, frame);          break;
        case VIZ_FRACTAL:  draw_fractal(frame);                         break;
        case VIZ_FIREFLY:  draw_firefly(cols, count, frame);           break;
        default:           break;
    }
}
