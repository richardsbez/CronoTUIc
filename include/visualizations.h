#ifndef VISUALIZATIONS_H
#define VISUALIZATIONS_H

#include "matrix_timer.h"

void draw_rain(RainColumn *cols, int count, int frame);
void draw_vortex(RainColumn *cols, int count, int frame);
void draw_pulse(int frame, int remaining, int total);
void draw_wave(RainColumn *cols, int count, int frame);
void draw_static(int frame);
void draw_matrix3d(RainColumn *cols, int count, int frame);
void draw_fractal(int frame);
void draw_firefly(RainColumn *cols, int count, int frame);

/* Dispatch to the current viz_mode */
void draw_visualization(RainColumn *cols, int count, int frame,
                         int remaining, int total);

#endif /* VISUALIZATIONS_H */





/* ─── visualizations.h = All visual effect modes ──────────────────────────────────────────── */

