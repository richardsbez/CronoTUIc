<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<div class="page">

  <h1 class="title">matrix-timer</h1>
  <div class="subtitle">terminal countdown timer with matrix visual effects — C17 / ncurses</div>

  <h2>Install</h2>
  <pre><code># Ubuntu / Debian
sudo apt install libncurses-dev build-essential

# macOS
brew install ncurses</code></pre>

  <h2>Build</h2>
  <pre><code>git clone https://github.com/your-user/matrix-timer
cd matrix-timer
make        # build
make run    # build and launch
make clean  # remove artifacts</code></pre>

  <h2>Usage</h2>
  <p>Launch with <code>make run</code>. From the main menu you can set a timer manually, pick a preset, use the stopwatch, or enter Pomodoro mode. All settings are changed live during a session.</p>

  <h2>Keybindings</h2>
  <table>
    <thead><tr><th>Key</th><th>Action</th></tr></thead>
    <tbody>
      <tr><td><code>P</code></td><td>Pause / Resume</td></tr>
      <tr><td><code>R</code></td><td>Restart</td></tr>
      <tr><td><code>S</code></td><td>Save state to disk</td></tr>
      <tr><td><code>T</code></td><td>Cycle color theme</td></tr>
      <tr><td><code>V</code></td><td>Cycle visualization</td></tr>
      <tr><td><code>C</code></td><td>Cycle charset</td></tr>
      <tr><td><code>F</code></td><td>Toggle fullscreen clock</td></tr>
      <tr><td><code>H</code></td><td>Toggle stats HUD</td></tr>
      <tr><td><code>X</code></td><td>Mark next task done</td></tr>
      <tr><td><code>L</code></td><td>Lap (stopwatch only)</td></tr>
      <tr><td><code>↑ / ↓</code></td><td>+/- 1 minute</td></tr>
      <tr><td><code>Q</code></td><td>Quit</td></tr>
    </tbody>
  </table>

  <h2>Features</h2>
  <ul>
    <li>8 visualization modes: Rain, Vortex, Pulse, Wave, Static, 3D, Fractal, Firefly</li>
    <li>9 color themes</li>
    <li>9 character sets — Katakana, Braille, Runes, DNA, Hex…</li>
    <li>Pomodoro mode with automatic work / break sequencing</li>
    <li>Stopwatch with lap times</li>
    <li>Task manager with in-timer HUD</li>
    <li>Session history and statistics</li>
    <li>Daily focus goal tracking</li>
    <li>State save and resume</li>
    <li>Quick notes with timestamps</li>
  </ul>

  <h2>Project structure</h2>
  <pre><code>src/
  main.c            entry point, top-level dispatch
  state.c           global state and defaults
  display.c         ncurses init, theme, HUD elements
  visualizations.c  all visual effect modes
  screens.c         menus — presets, settings, history…
  timer.c           timer and stopwatch loops
  storage.c         persistence, stats, pomodoro flow

include/
  matrix_timer.h    types, constants, extern declarations
  display.h
  visualizations.h
  screens.h
  timer.h
  storage.h</code></pre>

  <hr>

  <div class="footer">MIT License</div>

</div>
</body>
</html>
