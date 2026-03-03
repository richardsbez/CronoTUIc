Matrix-style stopwatch

<img width="798" height="633" alt="image" src="https://github.com/user-attachments/assets/f976cf13-e167-4018-bcb1-22ddcba8e7f4" />

<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Matrix Timer v3.0</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Orbitron:wght@400;700;900&family=VT323&display=swap" rel="stylesheet">
<style>
  :root {
    --green:      #00ff41;
    --green-dim:  #00c032;
    --green-glow: #00ff4133;
    --green-dark: #003b0f;
    --black:      #020a03;
    --surface:    #050f06;
    --surface2:   #0a1a0c;
    --border:     #00ff4122;
    --border-bright: #00ff4155;
    --text-dim:   #4a8f52;
    --text-mid:   #7fc786;
    --yellow:     #ffe066;
    --cyan:       #00ffe0;
    --red:        #ff3b3b;
  }

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  html { scroll-behavior: smooth; }

  body {
    background: var(--black);
    color: var(--green);
    font-family: 'Share Tech Mono', monospace;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
  }

  /* ── Rain canvas background ── */
  #rain-canvas {
    position: fixed;
    top: 0; left: 0;
    width: 100%; height: 100%;
    pointer-events: none;
    z-index: 0;
    opacity: 0.18;
  }

  /* ── Scanline overlay ── */
  body::after {
    content: '';
    position: fixed;
    inset: 0;
    background: repeating-linear-gradient(
      to bottom,
      transparent 0px,
      transparent 2px,
      rgba(0,0,0,0.08) 2px,
      rgba(0,0,0,0.08) 4px
    );
    pointer-events: none;
    z-index: 9999;
  }

  /* ── Layout ── */
  .wrap {
    position: relative;
    z-index: 1;
    max-width: 900px;
    margin: 0 auto;
    padding: 0 24px 80px;
  }

  /* ── Hero ── */
  .hero {
    padding: 72px 0 48px;
    text-align: center;
    position: relative;
  }

  .hero-pre {
    font-family: 'VT323', monospace;
    font-size: clamp(10px, 1.8vw, 14px);
    color: var(--green-dim);
    line-height: 1.15;
    letter-spacing: 0.05em;
    white-space: pre;
    display: inline-block;
    text-shadow: 0 0 8px var(--green-dim);
    animation: flickerIn 0.6s ease forwards;
    margin-bottom: 28px;
  }

  .hero-title {
    font-family: 'Orbitron', monospace;
    font-size: clamp(28px, 6vw, 60px);
    font-weight: 900;
    letter-spacing: 0.12em;
    color: var(--green);
    text-shadow:
      0 0 10px var(--green),
      0 0 30px var(--green-dim),
      0 0 60px var(--green-glow);
    animation: pulseGlow 3s ease-in-out infinite;
    margin-bottom: 12px;
  }

  .hero-version {
    font-family: 'Orbitron', monospace;
    font-size: 13px;
    letter-spacing: 0.3em;
    color: var(--text-dim);
    border: 1px solid var(--border-bright);
    display: inline-block;
    padding: 4px 16px;
    margin-bottom: 28px;
    background: var(--green-dark);
  }

  .hero-desc {
    font-size: 16px;
    color: var(--text-mid);
    max-width: 560px;
    margin: 0 auto 36px;
    line-height: 1.7;
    letter-spacing: 0.03em;
  }

  .badge-row {
    display: flex;
    gap: 10px;
    justify-content: center;
    flex-wrap: wrap;
    margin-bottom: 48px;
  }

  .badge {
    font-family: 'Share Tech Mono', monospace;
    font-size: 12px;
    letter-spacing: 0.08em;
    padding: 4px 12px;
    border: 1px solid;
    display: inline-flex;
    align-items: center;
    gap: 6px;
    transition: all 0.2s;
  }

  .badge-green  { border-color: var(--green-dim);  color: var(--green);  background: #00ff411a; }
  .badge-cyan   { border-color: #00ffe055;          color: var(--cyan);   background: #00ffe011; }
  .badge-yellow { border-color: #ffe06655;          color: var(--yellow); background: #ffe06611; }

  .badge:hover { transform: translateY(-2px); filter: brightness(1.3); }

  /* ── Terminal demo ── */
  .terminal {
    background: var(--surface);
    border: 1px solid var(--border-bright);
    box-shadow:
      0 0 0 1px var(--border),
      0 0 40px var(--green-glow),
      inset 0 0 80px #00000080;
    margin-bottom: 64px;
    position: relative;
    overflow: hidden;
  }

  .terminal::before {
    content: '';
    position: absolute;
    inset: 0;
    background: repeating-linear-gradient(
      90deg,
      transparent,
      transparent 2px,
      #00ff4104 2px,
      #00ff4104 4px
    );
    pointer-events: none;
  }

  .terminal-bar {
    background: var(--surface2);
    border-bottom: 1px solid var(--border-bright);
    padding: 10px 16px;
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .dot { width: 11px; height: 11px; border-radius: 50%; }
  .dot-r { background: #ff5f57; }
  .dot-y { background: #febc2e; }
  .dot-g { background: #28c840; }

  .terminal-label {
    font-size: 12px;
    color: var(--text-dim);
    letter-spacing: 0.15em;
    margin-left: 8px;
  }

  .terminal-body {
    padding: 24px 28px;
    font-family: 'VT323', monospace;
    font-size: 18px;
    line-height: 1.6;
    color: var(--green);
    min-height: 180px;
  }

  .term-prompt { color: var(--text-dim); }
  .term-cmd    { color: var(--green); }
  .term-out    { color: var(--text-mid); }
  .term-ok     { color: #28c840; }
  .term-warn   { color: var(--yellow); }
  .term-cursor {
    display: inline-block;
    width: 10px; height: 18px;
    background: var(--green);
    vertical-align: bottom;
    animation: blink 1s step-end infinite;
  }

  /* ── Section headers ── */
  .section { margin-bottom: 56px; }

  .section-header {
    display: flex;
    align-items: center;
    gap: 12px;
    margin-bottom: 24px;
    padding-bottom: 10px;
    border-bottom: 1px solid var(--border-bright);
  }

  .section-num {
    font-family: 'Orbitron', monospace;
    font-size: 11px;
    color: var(--text-dim);
    letter-spacing: 0.2em;
  }

  .section-title {
    font-family: 'Orbitron', monospace;
    font-size: 15px;
    font-weight: 700;
    letter-spacing: 0.2em;
    color: var(--green);
    text-shadow: 0 0 12px var(--green-dim);
  }

  .section-line {
    flex: 1;
    height: 1px;
    background: linear-gradient(to right, var(--border-bright), transparent);
  }

  /* ── Feature grid ── */
  .feature-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
    gap: 16px;
  }

  .feature-card {
    background: var(--surface);
    border: 1px solid var(--border);
    padding: 20px;
    transition: all 0.25s ease;
    position: relative;
    overflow: hidden;
  }

  .feature-card::before {
    content: '';
    position: absolute;
    top: 0; left: 0;
    width: 3px; height: 100%;
    background: var(--green-dim);
    transform: scaleY(0);
    transform-origin: bottom;
    transition: transform 0.25s ease;
  }

  .feature-card:hover {
    border-color: var(--border-bright);
    background: var(--surface2);
    box-shadow: 0 0 20px var(--green-glow);
    transform: translateY(-2px);
  }

  .feature-card:hover::before { transform: scaleY(1); }

  .feature-icon {
    font-size: 22px;
    margin-bottom: 10px;
    display: block;
  }

  .feature-name {
    font-family: 'Orbitron', monospace;
    font-size: 11px;
    letter-spacing: 0.18em;
    color: var(--green);
    margin-bottom: 6px;
  }

  .feature-desc {
    font-size: 13px;
    color: var(--text-mid);
    line-height: 1.6;
  }

  /* ── Install block ── */
  .install-block {
    background: var(--surface);
    border: 1px solid var(--border-bright);
    padding: 28px 32px;
    box-shadow: 0 0 30px var(--green-glow);
  }

  .install-step {
    display: flex;
    gap: 20px;
    align-items: flex-start;
    margin-bottom: 20px;
  }

  .install-step:last-child { margin-bottom: 0; }

  .step-num {
    font-family: 'Orbitron', monospace;
    font-size: 10px;
    color: var(--black);
    background: var(--green);
    width: 22px; height: 22px;
    display: flex;
    align-items: center;
    justify-content: center;
    flex-shrink: 0;
    margin-top: 2px;
    letter-spacing: 0;
  }

  .step-body { flex: 1; }

  .step-title {
    font-family: 'Orbitron', monospace;
    font-size: 11px;
    letter-spacing: 0.15em;
    color: var(--text-mid);
    margin-bottom: 8px;
  }

  .code-line {
    background: #000;
    border: 1px solid var(--border);
    padding: 8px 14px;
    font-family: 'Share Tech Mono', monospace;
    font-size: 14px;
    color: var(--green);
    margin-bottom: 4px;
    display: flex;
    align-items: center;
    gap: 10px;
    cursor: pointer;
    transition: all 0.15s;
    user-select: none;
    position: relative;
  }

  .code-line:hover {
    border-color: var(--green-dim);
    background: var(--green-dark);
  }

  .code-line .prompt { color: var(--text-dim); flex-shrink: 0; }
  .code-line .cmd    { color: var(--green); }
  .code-line .comment { color: var(--text-dim); font-size: 12px; margin-left: auto; }

  .copy-toast {
    position: absolute;
    right: 12px;
    background: var(--green);
    color: var(--black);
    font-size: 11px;
    padding: 2px 8px;
    pointer-events: none;
    opacity: 0;
    transition: opacity 0.2s;
    font-family: 'Orbitron', monospace;
    letter-spacing: 0.1em;
  }

  .code-line.copied .copy-toast { opacity: 1; }
  .code-line.copied .cmd { opacity: 0.4; }

  /* ── Keybind table ── */
  .key-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
    gap: 8px;
  }

  .key-row {
    display: flex;
    align-items: center;
    gap: 12px;
    background: var(--surface);
    border: 1px solid var(--border);
    padding: 10px 14px;
    transition: all 0.2s;
  }

  .key-row:hover {
    border-color: var(--green-dim);
    background: var(--surface2);
  }

  .key-badge {
    font-family: 'Orbitron', monospace;
    font-size: 11px;
    background: var(--green-dark);
    border: 1px solid var(--green-dim);
    color: var(--green);
    padding: 3px 8px;
    letter-spacing: 0.1em;
    flex-shrink: 0;
    min-width: 40px;
    text-align: center;
  }

  .key-action {
    font-size: 13px;
    color: var(--text-mid);
  }

  /* ── Viz & Theme pills ── */
  .pill-group { display: flex; flex-wrap: wrap; gap: 8px; }

  .pill {
    font-size: 12px;
    letter-spacing: 0.08em;
    padding: 5px 14px;
    border: 1px solid var(--border-bright);
    color: var(--green-dim);
    background: var(--surface);
    transition: all 0.2s;
    cursor: default;
  }

  .pill:hover {
    color: var(--green);
    border-color: var(--green);
    background: var(--surface2);
    box-shadow: 0 0 10px var(--green-glow);
  }

  /* ── Architecture tree ── */
  .arch-tree {
    background: var(--surface);
    border: 1px solid var(--border-bright);
    padding: 24px 28px;
    font-family: 'Share Tech Mono', monospace;
    font-size: 14px;
    line-height: 2;
  }

  .tree-dir   { color: var(--green); }
  .tree-file  { color: var(--text-mid); }
  .tree-anno  { color: var(--text-dim); font-size: 12px; }
  .tree-conn  { color: #2a5a2a; }

  /* ── Layer table ── */
  .layer-table { width: 100%; border-collapse: collapse; }

  .layer-table th {
    font-family: 'Orbitron', monospace;
    font-size: 10px;
    letter-spacing: 0.2em;
    color: var(--text-dim);
    text-align: left;
    padding: 8px 16px;
    border-bottom: 1px solid var(--border-bright);
    background: var(--surface2);
  }

  .layer-table td {
    padding: 12px 16px;
    font-size: 13px;
    color: var(--text-mid);
    border-bottom: 1px solid var(--border);
    vertical-align: top;
  }

  .layer-table tr:hover td { background: var(--surface2); color: var(--green); }

  .layer-name {
    font-family: 'Orbitron', monospace;
    font-size: 11px;
    letter-spacing: 0.12em;
    color: var(--green);
  }

  .layer-files {
    font-family: 'Share Tech Mono', monospace;
    font-size: 12px;
    color: var(--cyan);
  }

  /* ── Footer ── */
  .footer {
    border-top: 1px solid var(--border-bright);
    padding: 32px 0;
    text-align: center;
    color: var(--text-dim);
    font-size: 13px;
    letter-spacing: 0.08em;
    line-height: 2;
  }

  .footer a {
    color: var(--green-dim);
    text-decoration: none;
    transition: color 0.2s;
  }

  .footer a:hover { color: var(--green); }

  /* ── Animations ── */
  @keyframes blink {
    0%, 100% { opacity: 1; }
    50%       { opacity: 0; }
  }

  @keyframes pulseGlow {
    0%, 100% { text-shadow: 0 0 10px var(--green), 0 0 30px var(--green-dim), 0 0 60px var(--green-glow); }
    50%       { text-shadow: 0 0 20px var(--green), 0 0 50px var(--green-dim), 0 0 100px var(--green-glow); }
  }

  @keyframes flickerIn {
    0%   { opacity: 0; filter: brightness(3); }
    20%  { opacity: 1; }
    30%  { opacity: 0.4; }
    100% { opacity: 1; filter: brightness(1); }
  }

  @keyframes fadeUp {
    from { opacity: 0; transform: translateY(20px); }
    to   { opacity: 1; transform: translateY(0); }
  }

  .fade-up { animation: fadeUp 0.5s ease forwards; }
  .fade-up-1 { animation: fadeUp 0.5s 0.1s ease both; }
  .fade-up-2 { animation: fadeUp 0.5s 0.2s ease both; }
  .fade-up-3 { animation: fadeUp 0.5s 0.3s ease both; }
  .fade-up-4 { animation: fadeUp 0.5s 0.4s ease both; }

  /* ── Responsive ── */
  @media (max-width: 600px) {
    .hero-pre { font-size: 7px; }
    .terminal-body { font-size: 14px; padding: 16px; }
    .install-block { padding: 20px; }
  }
</style>
</head>
<body>

<canvas id="rain-canvas"></canvas>

<div class="wrap">

  <!-- ── HERO ── -->
  <header class="hero">
    <pre class="hero-pre">
 __  __    _  _____ ____  ___ __  __   _____ ___ __  __ _____ ____
|  \/  |  / \|_   _|  _ \|_ _\ \/ /  |_   _|_ _|  \/  | ____|  _ \
| |\/| | / _ \ | | | |_) || | >  <     | |  | || |\/| |  _| | |_) |
| |  | |/ ___ \| | |  _ < | |/ . \     | |  | || |  | | |___|  _ <
|_|  |_/_/   \_\_| |_| \_\___/_/\_\    |_| |___|_|  |_|_____|_| \_\</pre>

    <h1 class="hero-title">MATRIX TIMER</h1>
    <div class="hero-version">VERSION 3.0 · C17 · NCURSES</div>
    <p class="hero-desc fade-up-1">
      A terminal countdown timer with Matrix-style visual effects.<br>
      Built in C with ncurses — no dependencies beyond the terminal itself.
    </p>
    <div class="badge-row fade-up-2">
      <span class="badge badge-green">⬢ C17</span>
      <span class="badge badge-green">⬢ ncurses</span>
      <span class="badge badge-cyan">◈ 8 Visualizations</span>
      <span class="badge badge-cyan">◈ 9 Themes</span>
      <span class="badge badge-yellow">◉ Pomodoro</span>
      <span class="badge badge-yellow">◉ Stopwatch</span>
    </div>
  </header>

  <!-- ── TERMINAL DEMO ── -->
  <div class="terminal fade-up-3">
    <div class="terminal-bar">
      <div class="dot dot-r"></div>
      <div class="dot dot-y"></div>
      <div class="dot dot-g"></div>
      <span class="terminal-label">matrix_timer — bash</span>
    </div>
    <div class="terminal-body" id="term-body">
      <div><span class="term-prompt">user@arch ~$ </span><span class="term-cmd" id="term-line"></span></div>
      <div id="term-output"></div>
    </div>
  </div>

  <!-- ── QUICK INSTALL ── -->
  <section class="section fade-up-4">
    <div class="section-header">
      <span class="section-num">01</span>
      <span class="section-title">INSTALL &amp; RUN</span>
      <div class="section-line"></div>
    </div>

    <div class="install-block">
      <div class="install-step">
        <div class="step-num">1</div>
        <div class="step-body">
          <div class="step-title">DEPENDENCIES</div>
          <div class="code-line" onclick="copyLine(this, 'sudo apt install libncurses-dev build-essential')">
            <span class="prompt">$</span>
            <span class="cmd">sudo apt install libncurses-dev build-essential</span>
            <span class="comment"># Ubuntu/Debian</span>
            <span class="copy-toast">COPIED</span>
          </div>
          <div class="code-line" onclick="copyLine(this, 'brew install ncurses')">
            <span class="prompt">$</span>
            <span class="cmd">brew install ncurses</span>
            <span class="comment"># macOS</span>
            <span class="copy-toast">COPIED</span>
          </div>
        </div>
      </div>

      <div class="install-step">
        <div class="step-num">2</div>
        <div class="step-body">
          <div class="step-title">CLONE</div>
          <div class="code-line" onclick="copyLine(this, 'git clone https://github.com/your-user/matrix-timer && cd matrix-timer')">
            <span class="prompt">$</span>
            <span class="cmd">git clone https://github.com/your-user/matrix-timer</span>
            <span class="copy-toast">COPIED</span>
          </div>
        </div>
      </div>

      <div class="install-step">
        <div class="step-num">3</div>
        <div class="step-body">
          <div class="step-title">BUILD &amp; LAUNCH</div>
          <div class="code-line" onclick="copyLine(this, 'make run')">
            <span class="prompt">$</span>
            <span class="cmd">make run</span>
            <span class="comment"># build + launch</span>
            <span class="copy-toast">COPIED</span>
          </div>
          <div class="code-line" onclick="copyLine(this, 'make clean')">
            <span class="prompt">$</span>
            <span class="cmd">make clean</span>
            <span class="comment"># remove build artifacts</span>
            <span class="copy-toast">COPIED</span>
          </div>
        </div>
      </div>
    </div>
  </section>

  <!-- ── FEATURES ── -->
  <section class="section">
    <div class="section-header">
      <span class="section-num">02</span>
      <span class="section-title">FEATURES</span>
      <div class="section-line"></div>
    </div>

    <div class="feature-grid">
      <div class="feature-card">
        <span class="feature-icon">🌧</span>
        <div class="feature-name">8 VISUALIZATIONS</div>
        <div class="feature-desc">Matrix Rain, Vortex, Pulse, Wave, Static, 3D Matrix, Fractal, Firefly — cycle on the fly.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">🎨</span>
        <div class="feature-name">9 COLOR THEMES</div>
        <div class="feature-desc">Green Matrix, Ice Blue, Fire Red, Gold, Neon Cyan, Purple Haze, Rainbow, Hacker Dark, Sunset.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">🍅</span>
        <div class="feature-name">POMODORO MODE</div>
        <div class="feature-desc">Automatic work/short break/long break sequencing with HUD display and session counter.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">⏱</span>
        <div class="feature-name">STOPWATCH</div>
        <div class="feature-desc">Full stopwatch with lap times, pause/resume and the same visualization engine.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">✅</span>
        <div class="feature-name">TASK MANAGER</div>
        <div class="feature-desc">In-timer task HUD, mark done with a single keypress, persisted between sessions.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">📊</span>
        <div class="feature-name">STATISTICS</div>
        <div class="feature-desc">Session history, daily goal tracking, pomodoro count, total focus time — all in a TUI dashboard.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">💾</span>
        <div class="feature-name">STATE PERSISTENCE</div>
        <div class="feature-desc">Save a running timer to disk and resume it exactly where you left off.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">🔤</span>
        <div class="feature-name">9 CHARSETS</div>
        <div class="feature-desc">Matrix, Digits, Binary, Katakana, Braille, Hex, DNA, Norse Runes, Unicode Symbols.</div>
      </div>
      <div class="feature-card">
        <span class="feature-icon">📝</span>
        <div class="feature-name">QUICK NOTES</div>
        <div class="feature-desc">Timestamped notes saved between sessions — capture thoughts without leaving the terminal.</div>
      </div>
    </div>
  </section>

  <!-- ── VISUALIZATIONS ── -->
  <section class="section">
    <div class="section-header">
      <span class="section-num">03</span>
      <span class="section-title">VISUALIZATIONS &amp; THEMES</span>
      <div class="section-line"></div>
    </div>

    <div style="margin-bottom: 20px;">
      <div class="step-title" style="margin-bottom: 10px; font-family: 'Orbitron', monospace; font-size: 11px; letter-spacing: 0.15em; color: var(--text-dim);">VISUALIZATION MODES</div>
      <div class="pill-group">
        <span class="pill">Matrix Rain</span>
        <span class="pill">Vortex</span>
        <span class="pill">Pulse</span>
        <span class="pill">Wave</span>
        <span class="pill">Static</span>
        <span class="pill">Matrix 3D</span>
        <span class="pill">Fractal</span>
        <span class="pill">Firefly</span>
      </div>
    </div>

    <div style="margin-bottom: 20px;">
      <div class="step-title" style="margin-bottom: 10px; font-family: 'Orbitron', monospace; font-size: 11px; letter-spacing: 0.15em; color: var(--text-dim);">COLOR THEMES</div>
      <div class="pill-group">
        <span class="pill" style="color: #00ff41; border-color: #00ff4155;">Green Matrix</span>
        <span class="pill" style="color: #87ceeb; border-color: #87ceeb55;">Ice Blue</span>
        <span class="pill" style="color: #ff4444; border-color: #ff444455;">Fire Red</span>
        <span class="pill" style="color: #ffd700; border-color: #ffd70055;">Gold</span>
        <span class="pill" style="color: #00ffe0; border-color: #00ffe055;">Neon Cyan</span>
        <span class="pill" style="color: #bf5fff; border-color: #bf5fff55;">Purple Haze</span>
        <span class="pill" style="color: #ff88ff; border-color: #ff88ff55;">Rainbow</span>
        <span class="pill" style="color: #00ff41; border-color: #00ff4155;">Hacker Dark</span>
        <span class="pill" style="color: #ff8844; border-color: #ff884455;">Sunset</span>
      </div>
    </div>

    <div>
      <div class="step-title" style="margin-bottom: 10px; font-family: 'Orbitron', monospace; font-size: 11px; letter-spacing: 0.15em; color: var(--text-dim);">CHARACTER SETS</div>
      <div class="pill-group">
        <span class="pill">Matrix</span>
        <span class="pill">Digits</span>
        <span class="pill">Binary</span>
        <span class="pill">Katakana　ｶﾀｶﾅ</span>
        <span class="pill">Braille ⠿</span>
        <span class="pill">Hex 0xFF</span>
        <span class="pill">DNA ACGT</span>
        <span class="pill">Runes ᚠᛁᚢ</span>
        <span class="pill">Symbols ★☆◆</span>
      </div>
    </div>
  </section>

  <!-- ── KEY BINDINGS ── -->
  <section class="section">
    <div class="section-header">
      <span class="section-num">04</span>
      <span class="section-title">KEY BINDINGS</span>
      <div class="section-line"></div>
    </div>

    <div class="key-grid">
      <div class="key-row"><span class="key-badge">P</span><span class="key-action">Pause / Resume</span></div>
      <div class="key-row"><span class="key-badge">R</span><span class="key-action">Restart timer</span></div>
      <div class="key-row"><span class="key-badge">S</span><span class="key-action">Save state to disk</span></div>
      <div class="key-row"><span class="key-badge">T</span><span class="key-action">Cycle color theme</span></div>
      <div class="key-row"><span class="key-badge">V</span><span class="key-action">Cycle visualization</span></div>
      <div class="key-row"><span class="key-badge">C</span><span class="key-action">Cycle charset</span></div>
      <div class="key-row"><span class="key-badge">F</span><span class="key-action">Fullscreen clock</span></div>
      <div class="key-row"><span class="key-badge">H</span><span class="key-action">Toggle stats HUD</span></div>
      <div class="key-row"><span class="key-badge">X</span><span class="key-action">Mark next task done</span></div>
      <div class="key-row"><span class="key-badge">L</span><span class="key-action">Lap (stopwatch)</span></div>
      <div class="key-row"><span class="key-badge">↑ ↓</span><span class="key-action">±1 minute</span></div>
      <div class="key-row"><span class="key-badge">Q</span><span class="key-action">Quit</span></div>
    </div>
  </section>

  <!-- ── ARCHITECTURE ── -->
  <section class="section">
    <div class="section-header">
      <span class="section-num">05</span>
      <span class="section-title">ARCHITECTURE</span>
      <div class="section-line"></div>
    </div>

    <div class="arch-tree" style="margin-bottom: 24px;">
<span class="tree-dir">matrix_timer/</span>
<span class="tree-conn">├── </span><span class="tree-file">Makefile</span>
<span class="tree-conn">├── </span><span class="tree-file">README.html</span>
<span class="tree-conn">├── </span><span class="tree-dir">include/</span>
<span class="tree-conn">│   ├── </span><span class="tree-file">matrix_timer.h</span>   <span class="tree-anno">← types, constants, extern declarations</span>
<span class="tree-conn">│   ├── </span><span class="tree-file">display.h</span>         <span class="tree-anno">← ncurses init, theme, HUD elements</span>
<span class="tree-conn">│   ├── </span><span class="tree-file">visualizations.h</span>  <span class="tree-anno">← all visual effect modes</span>
<span class="tree-conn">│   ├── </span><span class="tree-file">screens.h</span>         <span class="tree-anno">← TUI menus and screens</span>
<span class="tree-conn">│   ├── </span><span class="tree-file">timer.h</span>           <span class="tree-anno">← timer and stopwatch loops</span>
<span class="tree-conn">│   └── </span><span class="tree-file">storage.h</span>         <span class="tree-anno">← persistence, stats, pomodoro</span>
<span class="tree-conn">└── </span><span class="tree-dir">src/</span>
<span class="tree-conn">    ├── </span><span class="tree-file">main.c</span>            <span class="tree-anno">← entry point, top-level dispatch</span>
<span class="tree-conn">    ├── </span><span class="tree-file">state.c</span>           <span class="tree-anno">← global state definitions</span>
<span class="tree-conn">    ├── </span><span class="tree-file">display.c</span>         <span class="tree-anno">← ncurses/theme setup, HUD rendering</span>
<span class="tree-conn">    ├── </span><span class="tree-file">visualizations.c</span>  <span class="tree-anno">← Rain, Vortex, Pulse, Wave, 3D, Fractal…</span>
<span class="tree-conn">    ├── </span><span class="tree-file">screens.c</span>         <span class="tree-anno">← menus, presets, settings, history…</span>
<span class="tree-conn">    ├── </span><span class="tree-file">timer.c</span>           <span class="tree-anno">← core timer loop, stopwatch loop</span>
<span class="tree-conn">    └── </span><span class="tree-file">storage.c</span>         <span class="tree-anno">← save/load, stats, pomodoro flow</span>
    </div>

    <table class="layer-table">
      <thead>
        <tr>
          <th>LAYER</th>
          <th>FILES</th>
          <th>RESPONSIBILITY</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td><span class="layer-name">TYPES &amp; STATE</span></td>
          <td><span class="layer-files">matrix_timer.h, state.c</span></td>
          <td>Enums, structs, global variables and defaults</td>
        </tr>
        <tr>
          <td><span class="layer-name">RENDERING</span></td>
          <td><span class="layer-files">display.c, visualizations.c</span></td>
          <td>All ncurses drawing — HUDs, themes, effects</td>
        </tr>
        <tr>
          <td><span class="layer-name">UI / MENUS</span></td>
          <td><span class="layer-files">screens.c</span></td>
          <td>Every interactive TUI screen and menu</td>
        </tr>
        <tr>
          <td><span class="layer-name">LOGIC</span></td>
          <td><span class="layer-files">timer.c</span></td>
          <td>Timer and stopwatch event loops</td>
        </tr>
        <tr>
          <td><span class="layer-name">PERSISTENCE</span></td>
          <td><span class="layer-files">storage.c</span></td>
          <td>File I/O, statistics, daily goal, pomodoro sequence</td>
        </tr>
        <tr>
          <td><span class="layer-name">ENTRY</span></td>
          <td><span class="layer-files">main.c</span></td>
          <td>Startup, data loading, top-level action dispatch</td>
        </tr>
      </tbody>
    </table>
  </section>

  <!-- ── FOOTER ── -->
  <footer class="footer">
    <div>Built with C17 · ncurses · libm</div>
    <div style="margin-top: 8px; color: #2a5a2a;">
      Matrix Timer v3.0 &nbsp;·&nbsp; MIT License
    </div>
  </footer>

</div><!-- /wrap -->

<!-- ── SCRIPTS ── -->
<script>
/* ── Matrix rain canvas ── */
(function () {
  const canvas = document.getElementById('rain-canvas');
  const ctx    = canvas.getContext('2d');

  const CHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$+-*/=%#&_|{}<>@^~ｦｧｨｩｪｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁ';
  let cols, drops, fontSize;

  function resize () {
    canvas.width  = window.innerWidth;
    canvas.height = window.innerHeight;
    fontSize = 14;
    cols = Math.floor(canvas.width / fontSize) + 1;
    drops = Array.from({ length: cols }, () => Math.random() * -50);
  }

  function tick () {
    ctx.fillStyle = 'rgba(2, 10, 3, 0.05)';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    for (let i = 0; i < cols; i++) {
      const char = CHARS[Math.floor(Math.random() * CHARS.length)];
      const x = i * fontSize;
      const y = drops[i] * fontSize;

      ctx.font = fontSize + 'px "Share Tech Mono", monospace';
      ctx.fillStyle = Math.random() > 0.95 ? '#ffffff' : '#00ff41';
      ctx.fillText(char, x, y);

      if (y > canvas.height && Math.random() > 0.975)
        drops[i] = 0;
      drops[i] += 0.5;
    }
  }

  resize();
  window.addEventListener('resize', resize);
  setInterval(tick, 50);
})();

/* ── Terminal typewriter ── */
(function () {
  const sequences = [
    { cmd: 'make run', delay: 60, outputs: [
      { text: 'cc -std=c17 -Wall -Wextra -O2 -Iinclude -c src/display.c', cls: 'term-out', d: 80 },
      { text: 'cc -std=c17 -Wall -Wextra -O2 -Iinclude -c src/timer.c',   cls: 'term-out', d: 80 },
      { text: 'cc -std=c17 -Wall -Wextra -O2 -Iinclude -c src/screens.c', cls: 'term-out', d: 80 },
      { text: 'cc ... -o matrix_timer -lncurses -lm',                      cls: 'term-out', d: 80 },
      { text: '✓  Build successful',                                        cls: 'term-ok',  d: 200 },
      { text: '▶  Launching matrix_timer...',                               cls: 'term-warn', d: 400 },
    ]},
  ];

  const cmdEl = document.getElementById('term-line');
  const outEl = document.getElementById('term-output');
  let seq = sequences[0];
  let charIdx = 0;

  function typeCmd () {
    if (charIdx < seq.cmd.length) {
      cmdEl.textContent += seq.cmd[charIdx++];
      setTimeout(typeCmd, seq.delay);
    } else {
      setTimeout(() => showOutputs(0), 400);
    }
  }

  function showOutputs (idx) {
    if (idx >= seq.outputs.length) {
      setTimeout(reset, 3500);
      return;
    }
    const o = seq.outputs[idx];
    const line = document.createElement('div');
    line.className = o.cls;
    line.textContent = o.text;
    outEl.appendChild(line);
    setTimeout(() => showOutputs(idx + 1), o.d);
  }

  function reset () {
    setTimeout(() => {
      cmdEl.textContent = '';
      outEl.innerHTML   = '';
      charIdx = 0;
      typeCmd();
    }, 800);
  }

  setTimeout(typeCmd, 1200);
})();

/* ── Copy-to-clipboard ── */
function copyLine (el, text) {
  navigator.clipboard.writeText(text).then(() => {
    el.classList.add('copied');
    setTimeout(() => el.classList.remove('copied'), 1400);
  });
}
</script>

</body>
</html>
