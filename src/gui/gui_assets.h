#ifndef GUI_ASSETS_H
#define GUI_ASSETS_H

#include <string>

namespace GuiAssets {

const std::string INDEX_HTML = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Traveling Salesman Problem - Studio</title>
  <style>
    :root {
      --bg-primary: #0b0f19;
      --bg-secondary: #121829;
      --bg-card: rgba(22, 30, 49, 0.85);
      --bg-card-hover: rgba(30, 41, 68, 0.95);
      --border-color: rgba(255, 255, 255, 0.08);
      --border-focus: #00f0ff;
      --text-main: #f3f4f6;
      --text-muted: #9ca3af;
      --accent-cyan: #00f0ff;
      --accent-purple: #a855f7;
      --accent-green: #10b981;
      --accent-amber: #f59e0b;
      --accent-rose: #f43f5e;
      --node-color: #00f0ff;
      --node-glow: rgba(0, 240, 255, 0.5);
      --path-color: #a855f7;
      --path-glow: rgba(168, 85, 247, 0.4);
      --start-node-color: #10b981;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      user-select: none;
    }

    body {
      background-color: var(--bg-primary);
      color: var(--text-main);
      height: 100vh;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }

    /* Header Bar */
    header {
      background: var(--bg-secondary);
      border-bottom: 1px solid var(--border-color);
      padding: 10px 20px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      height: 56px;
      flex-shrink: 0;
    }

    .brand {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    .brand-icon {
      width: 32px;
      height: 32px;
      background: linear-gradient(135deg, var(--accent-cyan), var(--accent-purple));
      border-radius: 8px;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 18px;
      box-shadow: 0 0 12px var(--node-glow);
    }

    .brand-title {
      font-size: 17px;
      font-weight: 700;
      letter-spacing: -0.3px;
      background: linear-gradient(90deg, #ffffff, #cbd5e1);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    .brand-badge {
      font-size: 11px;
      background: rgba(0, 240, 255, 0.12);
      color: var(--accent-cyan);
      border: 1px solid rgba(0, 240, 255, 0.25);
      padding: 2px 8px;
      border-radius: 12px;
      font-weight: 600;
    }

    .header-actions {
      display: flex;
      align-items: center;
      gap: 8px;
    }

    /* Buttons */
    button, .btn {
      background: var(--bg-card);
      color: var(--text-main);
      border: 1px solid var(--border-color);
      padding: 7px 14px;
      border-radius: 6px;
      font-size: 13px;
      font-weight: 500;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
      transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    }

    button:hover, .btn:hover {
      background: var(--bg-card-hover);
      border-color: rgba(255, 255, 255, 0.2);
      transform: translateY(-1px);
    }

    button:active, .btn:active {
      transform: translateY(0);
    }

    .btn-primary {
      background: linear-gradient(135deg, #00f0ff, #0070f3);
      color: #000;
      font-weight: 700;
      border: none;
      box-shadow: 0 2px 10px rgba(0, 240, 255, 0.25);
    }

    .btn-primary:hover {
      background: linear-gradient(135deg, #38f8ff, #1a85ff);
      box-shadow: 0 4px 16px rgba(0, 240, 255, 0.4);
    }

    .btn-accent {
      background: linear-gradient(135deg, var(--accent-purple), #7c3aed);
      color: #fff;
      border: none;
    }

    .btn-danger {
      background: rgba(244, 63, 94, 0.15);
      color: var(--accent-rose);
      border-color: rgba(244, 63, 94, 0.3);
    }

    .btn-danger:hover {
      background: rgba(244, 63, 94, 0.25);
    }

    .btn-sm {
      padding: 4px 10px;
      font-size: 12px;
    }

    /* Main Container */
    .app-body {
      display: flex;
      flex: 1;
      overflow: hidden;
      position: relative;
    }

    /* Left: Canvas Area */
    .canvas-container {
      flex: 1;
      position: relative;
      background: radial-gradient(circle at center, #111726 0%, #080b12 100%);
      overflow: hidden;
      display: flex;
    }

    #tspCanvas {
      width: 100%;
      height: 100%;
      display: block;
      cursor: grab;
    }

    #tspCanvas.grabbing {
      cursor: grabbing;
    }

    #tspCanvas.crosshair {
      cursor: crosshair;
    }

    /* Floating HUD Badge */
    .hud-metrics {
      position: absolute;
      top: 16px;
      left: 16px;
      background: var(--bg-card);
      backdrop-filter: blur(12px);
      border: 1px solid var(--border-color);
      border-radius: 8px;
      padding: 10px 16px;
      display: flex;
      gap: 20px;
      z-index: 10;
      box-shadow: 0 4px 20px rgba(0,0,0,0.4);
      pointer-events: none;
    }

    .hud-item {
      display: flex;
      flex-direction: column;
    }

    .hud-label {
      font-size: 10px;
      color: var(--text-muted);
      text-transform: uppercase;
      font-weight: 700;
      letter-spacing: 0.5px;
    }

    .hud-val {
      font-size: 18px;
      font-weight: 700;
      color: #fff;
    }

    .hud-val.highlight {
      color: var(--accent-cyan);
      text-shadow: 0 0 8px rgba(0, 240, 255, 0.4);
    }

    /* Floating Tools Toolbar */
    .canvas-toolbar {
      position: absolute;
      bottom: 20px;
      left: 50%;
      transform: translateX(-50%);
      background: var(--bg-card);
      backdrop-filter: blur(12px);
      border: 1px solid var(--border-color);
      border-radius: 30px;
      padding: 6px 12px;
      display: flex;
      align-items: center;
      gap: 8px;
      z-index: 10;
      box-shadow: 0 4px 24px rgba(0, 0, 0, 0.5);
    }

    .tool-btn {
      background: transparent;
      border: none;
      padding: 6px 10px;
      border-radius: 20px;
      color: var(--text-muted);
      font-size: 12px;
    }

    .tool-btn:hover {
      background: rgba(255, 255, 255, 0.1);
      color: var(--text-main);
      transform: none;
    }

    .tool-btn.active {
      background: var(--accent-cyan);
      color: #000;
      font-weight: 700;
    }

    .toolbar-divider {
      width: 1px;
      height: 18px;
      background: var(--border-color);
    }

    /* Right: Control Sidebar */
    .sidebar {
      width: 380px;
      background: var(--bg-secondary);
      border-left: 1px solid var(--border-color);
      display: flex;
      flex-direction: column;
      overflow-y: auto;
      flex-shrink: 0;
    }

    .sidebar::-webkit-scrollbar {
      width: 6px;
    }
    .sidebar::-webkit-scrollbar-thumb {
      background: rgba(255, 255, 255, 0.15);
      border-radius: 3px;
    }

    .panel-section {
      padding: 16px;
      border-bottom: 1px solid var(--border-color);
    }

    .section-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 12px;
    }

    .section-title {
      font-size: 13px;
      font-weight: 700;
      text-transform: uppercase;
      letter-spacing: 0.6px;
      color: var(--text-muted);
      display: flex;
      align-items: center;
      gap: 6px;
    }

    /* Form Controls */
    .form-group {
      margin-bottom: 12px;
    }

    .form-group:last-child {
      margin-bottom: 0;
    }

    label {
      display: block;
      font-size: 12px;
      color: var(--text-muted);
      margin-bottom: 5px;
      font-weight: 500;
    }

    select, input[type="text"], input[type="number"] {
      width: 100%;
      background: #111827;
      border: 1px solid var(--border-color);
      border-radius: 6px;
      color: #f8fafc;
      padding: 9px 12px;
      font-size: 13px;
      font-weight: 500;
      font-family: inherit;
      outline: none;
      transition: all 0.2s ease;
    }

    select {
      cursor: pointer;
      appearance: none;
      -webkit-appearance: none;
      -moz-appearance: none;
      padding-right: 34px;
      background-image: url("data:image/svg+xml;charset=UTF-8,%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='none' stroke='%2300f0ff' stroke-width='2.5' stroke-linecap='round' stroke-linejoin='round'%3e%3cpolyline points='6 9 12 15 18 9'%3e%3c/polyline%3e%3c/svg%3e");
      background-repeat: no-repeat;
      background-position: right 10px center;
      background-size: 16px 16px;
    }

    select:focus, input[type="text"]:focus, input[type="number"]:focus {
      border-color: var(--accent-cyan);
      box-shadow: 0 0 10px rgba(0, 240, 255, 0.25);
    }

    option {
      background-color: #0f172a !important;
      color: #f8fafc !important;
      padding: 10px 12px !important;
      font-size: 13px !important;
      font-weight: 500 !important;
    }

    optgroup {
      background-color: #020617 !important;
      color: #38bdf8 !important;
      font-weight: 800 !important;
      font-size: 12px !important;
      padding: 8px 6px 4px 6px !important;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }

    .algo-badge-card {
      margin-top: 8px;
      background: linear-gradient(135deg, rgba(0, 240, 255, 0.08) 0%, rgba(168, 85, 247, 0.08) 100%);
      border: 1px solid rgba(0, 240, 255, 0.25);
      border-radius: 6px;
      padding: 10px 12px;
      transition: all 0.2s ease;
    }

    .algo-badge-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 4px;
      gap: 6px;
    }

    .algo-badge-name {
      font-weight: 700;
      font-size: 13px;
      color: var(--accent-cyan);
    }

    .algo-badge-tag {
      font-size: 10px;
      font-weight: 700;
      text-transform: uppercase;
      padding: 2px 6px;
      border-radius: 10px;
      background: rgba(168, 85, 247, 0.25);
      color: #c084fc;
      border: 1px solid rgba(168, 85, 247, 0.4);
      white-space: nowrap;
    }

    .algo-badge-desc {
      font-size: 11.5px;
      color: #94a3b8;
      line-height: 1.4;
    }

    .input-row {
      display: flex;
      gap: 8px;
      align-items: center;
    }

    .range-row {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    input[type="range"] {
      flex: 1;
      accent-color: var(--accent-cyan);
      cursor: pointer;
    }

    .badge-val {
      min-width: 40px;
      text-align: right;
      font-weight: 700;
      font-size: 13px;
      color: var(--accent-cyan);
    }

    /* Execution Card */
    .execute-box {
      background: linear-gradient(180deg, rgba(30, 41, 68, 0.4) 0%, rgba(18, 24, 41, 0.8) 100%);
      border: 1px solid rgba(0, 240, 255, 0.2);
      border-radius: 8px;
      padding: 14px;
      margin-top: 10px;
    }

    .btn-execute {
      width: 100%;
      padding: 12px;
      font-size: 15px;
      justify-content: center;
      background: linear-gradient(90deg, #00f0ff, #a855f7);
      color: #000;
      font-weight: 800;
      border: none;
      box-shadow: 0 4px 18px rgba(0, 240, 255, 0.3);
      position: relative;
      overflow: hidden;
    }

    .btn-execute:hover {
      box-shadow: 0 6px 24px rgba(168, 85, 247, 0.5);
      filter: brightness(1.1);
    }

    .btn-execute.computing {
      pointer-events: none;
      background: #374151;
      color: #9ca3af;
    }

    .spinner {
      display: inline-block;
      width: 16px;
      height: 16px;
      border: 2px solid rgba(255, 255, 255, 0.3);
      border-top-color: #fff;
      border-radius: 50%;
      animation: spin 0.8s linear infinite;
    }

    @keyframes spin {
      to { transform: rotate(360deg); }
    }

    /* Results Card */
    .result-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
      margin-top: 10px;
    }

    .result-tile {
      background: var(--bg-primary);
      border: 1px solid var(--border-color);
      border-radius: 6px;
      padding: 8px 10px;
    }

    .tile-title {
      font-size: 10px;
      color: var(--text-muted);
      text-transform: uppercase;
      font-weight: 600;
    }

    .tile-val {
      font-size: 15px;
      font-weight: 700;
      margin-top: 2px;
      color: #fff;
    }

    .tile-val.green {
      color: var(--accent-green);
    }

    /* History Table */
    .history-table-container {
      max-height: 180px;
      overflow-y: auto;
      border: 1px solid var(--border-color);
      border-radius: 6px;
      margin-top: 8px;
    }

    table {
      width: 100%;
      border-collapse: collapse;
      font-size: 11px;
    }

    th {
      background: var(--bg-primary);
      color: var(--text-muted);
      text-align: left;
      padding: 6px 8px;
      font-weight: 600;
      position: sticky;
      top: 0;
      border-bottom: 1px solid var(--border-color);
    }

    td {
      padding: 6px 8px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.04);
      color: var(--text-main);
    }

    tr:hover td {
      background: rgba(255, 255, 255, 0.03);
      cursor: pointer;
    }

    .tag-best {
      background: rgba(16, 185, 129, 0.2);
      color: var(--accent-green);
      padding: 1px 4px;
      border-radius: 4px;
      font-weight: 700;
      font-size: 9px;
    }

    /* Drag Drop Overlay */
    .drop-overlay {
      position: absolute;
      inset: 0;
      background: rgba(11, 15, 25, 0.85);
      backdrop-filter: blur(8px);
      border: 2px dashed var(--accent-cyan);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      z-index: 100;
      opacity: 0;
      pointer-events: none;
      transition: opacity 0.2s;
    }

    .drop-overlay.active {
      opacity: 1;
      pointer-events: all;
    }

    .drop-title {
      font-size: 20px;
      font-weight: 700;
      color: var(--accent-cyan);
      margin-top: 10px;
    }

    /* Tooltip */
    #tooltip {
      position: absolute;
      background: rgba(15, 23, 42, 0.95);
      border: 1px solid var(--accent-cyan);
      color: #fff;
      padding: 4px 8px;
      border-radius: 4px;
      font-size: 11px;
      pointer-events: none;
      z-index: 50;
      display: none;
      white-space: nowrap;
      box-shadow: 0 4px 12px rgba(0,0,0,0.5);
    }
  </style>
</head>
<body>

  <!-- Header -->
  <header>
    <div class="brand">
      <div class="brand-icon">📍</div>
      <div class="brand-title">TSP Solver Studio</div>
      <div class="brand-badge">C++ Native WebView</div>
    </div>
    <div class="header-actions">
      <input type="file" id="tspFileInput" accept=".tsp" style="display:none">
      <input type="file" id="tourFileInput" accept=".tour" style="display:none">
      
      <button onclick="document.getElementById('tspFileInput').click()">📂 Load .tsp</button>
      <button onclick="document.getElementById('tourFileInput').click()">📂 Load .tour</button>
      <button onclick="exportTsp()">💾 Save .tsp</button>
      <button onclick="exportTour()">💾 Save .tour</button>
      <button onclick="exportImage()" class="btn-sm">📸 Export PNG</button>
    </div>
  </header>

  <!-- App Body -->
  <div class="app-body">
    
    <!-- Canvas Area -->
    <div class="canvas-container" id="canvasContainer">
      
      <!-- HUD Overlay -->
      <div class="hud-metrics">
        <div class="hud-item">
          <span class="hud-label">Best Tour Cost (L)</span>
          <span class="hud-val highlight" id="hudCost">0.00</span>
        </div>
        <div class="hud-item">
          <span class="hud-label">Cities (N)</span>
          <span class="hud-val" id="hudNodes">0</span>
        </div>
        <div class="hud-item">
          <span class="hud-label">Zoom Level</span>
          <span class="hud-val" id="hudZoom">100%</span>
        </div>
      </div>

      <!-- Canvas -->
      <canvas id="tspCanvas"></canvas>

      <!-- Floating Toolbar -->
      <div class="canvas-toolbar">
        <button class="tool-btn active" id="toolPan" onclick="setMode('pan')">🖐️ Pan</button>
        <button class="tool-btn" id="toolAdd" onclick="setMode('add')">➕ Add City</button>
        <div class="toolbar-divider"></div>
        <button class="tool-btn" onclick="zoomIn()">🔍 In</button>
        <button class="tool-btn" onclick="zoomOut()">🔍 Out</button>
        <button class="tool-btn" onclick="fitView()">🎯 Fit</button>
        <div class="toolbar-divider"></div>
        <button class="tool-btn" id="btnLabels" onclick="toggleLabels()">🏷️ Labels</button>
        <button class="tool-btn" id="btnPlay" onclick="togglePlayback()">⏯️ Play</button>
      </div>

      <!-- Drag and drop zone -->
      <div class="drop-overlay" id="dropOverlay">
        <div style="font-size: 48px;">📂</div>
        <div class="drop-title">Drop .tsp or .tour file here</div>
      </div>

      <!-- Tooltip -->
      <div id="tooltip"></div>
    </div>

    <!-- Right Controls Sidebar -->
    <aside class="sidebar">
      
      <!-- Section 1: Map Generation & Presets -->
      <div class="panel-section">
        <div class="section-header">
          <span class="section-title">🗺️ Map and Graph Manager</span>
        </div>

        <div class="form-group">
          <label>Standard Benchmarks</label>
          <select id="presetSelect" onchange="onPresetChange()">
            <option value="">-- Choose Benchmark --</option>
            <option value="berlin52">Berlin 52 (52 Cities, Opt: 7542)</option>
            <option value="kroA100">KroA 100 (100 Cities, Opt: 21282)</option>
            <option value="a280">A280 (280 Cities, Opt: 2579)</option>
          </select>
        </div>

        <div class="form-group">
          <label>Random Map Generator</label>
          <div class="range-row">
            <input type="range" id="nodeCountSlider" min="5" max="300" value="30" oninput="document.getElementById('nodeCountVal').innerText = this.value">
            <span class="badge-val" id="nodeCountVal">30</span>
          </div>
        </div>

        <div class="form-group">
          <label>Distribution Pattern</label>
          <select id="mapPattern">
            <option value="uniform">Uniform Random</option>
            <option value="clustered">Metropolitan Clusters</option>
            <option value="circle">Circular / Ring</option>
          </select>
        </div>

        <div class="input-row" style="margin-top: 10px;">
          <button onclick="generateMap()" class="btn-primary" style="flex: 1;">🎲 Generate Map</button>
          <button onclick="clearMap()" class="btn-danger" title="Clear all nodes">🗑️</button>
        </div>
      </div>

      <!-- Section 2: Algorithm Configuration -->
      <div class="panel-section">
        <div class="section-header">
          <span class="section-title">🧠 Algorithm Selection</span>
        </div>

        <div class="form-group">
          <label>Algorithm Technique</label>
          <select id="algoSelect" onchange="onAlgoChange()">
            <optgroup label="Constructive &amp; Heuristic">
              <option value="greedy">Greedy Search (Deterministic)</option>
              <option value="greedyls">Greedy + Local Search (greedyls)</option>
              <option value="greedyls+">Greedy + Extended LS (greedyls+)</option>
              <option value="grasp">GRASP (Greedy Randomized)</option>
              <option value="grasp+">GRASP Extended</option>
            </optgroup>
            <optgroup label="Neighborhood &amp; Local Search">
              <option value="rs">Random Search (rs)</option>
              <option value="ls">Local Search (ls)</option>
              <option value="vnd">Variable Neighborhood Descent (vnd)</option>
              <option value="bmb">Basic Multiboot Search (bmb)</option>
              <option value="ils">Iterated Local Search (ils)</option>
              <option value="vns">Variable Neighborhood Search (vns)</option>
              <option value="tabu">Tabu Search (tabu)</option>
            </optgroup>
            <optgroup label="Metaheuristics &amp; Evolutionary">
              <option value="sa" selected>Simulated Annealing (sa)</option>
              <option value="ga">Genetic Algorithm (ga)</option>
              <option value="ma">Memetic Algorithm (ma)</option>
            </optgroup>
            <optgroup label="Parallel Metaheuristics">
              <option value="psa">Parallel Simulated Annealing (psa)</option>
              <option value="pga">Parallel Genetic Algorithm (pga)</option>
            </optgroup>
          </select>
          <div id="algoBadgeCard" class="algo-badge-card">
            <div class="algo-badge-header">
              <span id="algoBadgeName" class="algo-badge-name">Simulated Annealing (sa)</span>
              <span id="algoBadgeTag" class="algo-badge-tag">Metaheuristic</span>
            </div>
            <div id="algoBadgeDesc" class="algo-badge-desc">Probabilistically accepts worsening moves to escape local optima.</div>
          </div>
        </div>

        <!-- Dynamic Parameters -->
        <div id="paramIterations" class="form-group">
          <label>Repetitions / Generations (N)</label>
          <input type="number" id="inputCount" value="2000" min="1" max="100000">
        </div>

        <div id="paramSeed" class="form-group">
          <label>Random Seed</label>
          <div class="input-row">
            <input type="number" id="inputSeed" value="0" placeholder="0 = Auto (Time)">
            <button onclick="randomizeSeed()" title="Randomize seed">🎲</button>
          </div>
        </div>

        <div id="paramPopSize" class="form-group" style="display:none">
          <label>Population Width (d)</label>
          <input type="number" id="inputPopSize" value="30" min="2" max="1000">
        </div>

        <div id="paramNeighbor" class="form-group">
          <label>Neighbor Operator</label>
          <select id="selectNeighbor">
            <option value="swap">Swap 2 Nodes</option>
            <option value="invert">Reverse Subpath (Invert)</option>
          </select>
        </div>

        <div id="paramScheme" class="form-group" style="display:none">
          <label>Evolutionary Scheme</label>
          <select id="selectScheme">
            <option value="gener">Generational</option>
            <option value="stat">Stationary</option>
          </select>
        </div>

        <div id="paramHybrid" class="form-group" style="display:none">
          <label>Memetic Hybridization</label>
          <select id="selectHybrid">
            <option value="all,all">All generations, all chromosomes</option>
            <option value="all,2">All generations, 2 best</option>
            <option value="10,all">Every 10 generations, all chromosomes</option>
            <option value="10,2">Every 10 generations, 2 best</option>
          </select>
        </div>

        <div id="paramProcesses" class="form-group" style="display:none">
          <label>Parallel Processes (p)</label>
          <input type="number" id="inputProcesses" value="4" min="2" max="32">
        </div>

        <div id="paramLatency" class="form-group" style="display:none">
          <label>Migration Latency (l)</label>
          <input type="number" id="inputLatency" value="2" min="1" max="100">
        </div>

        <div id="paramTopology" class="form-group" style="display:none">
          <label>Island Topology</label>
          <select id="selectTopology">
            <option value="ring">Ring</option>
            <option value="star">Star</option>
          </select>
        </div>

        <!-- Execute Action -->
        <div class="execute-box">
          <button class="btn-execute" id="btnExecute" onclick="runAlgorithm()">
            <span id="btnExecText">▶️ Execute Algorithm</span>
          </button>
        </div>
      </div>

      <!-- Section 3: Metrics & Comparison -->
      <div class="panel-section">
        <div class="section-header">
          <span class="section-title">📊 Execution Results</span>
        </div>

        <div class="result-grid">
          <div class="result-tile">
            <div class="tile-title">Current Cost</div>
            <div class="tile-val" id="resCost">0.0</div>
          </div>
          <div class="result-tile">
            <div class="tile-title">Initial Cost</div>
            <div class="tile-val" id="resInitialCost">-</div>
          </div>
          <div class="result-tile">
            <div class="tile-title">Improvement</div>
            <div class="tile-val green" id="resImprovement">-</div>
          </div>
          <div class="result-tile">
            <div class="tile-title">Execution Time</div>
            <div class="tile-val" id="resTime">-</div>
          </div>
        </div>

        <div class="section-header" style="margin-top: 16px; margin-bottom: 6px;">
          <span class="section-title">📜 Run History</span>
          <button onclick="clearHistory()" class="btn-sm" style="padding: 2px 6px;">Clear</button>
        </div>

        <div class="history-table-container">
          <table>
            <thead>
              <tr>
                <th>Algo</th>
                <th>Cost</th>
                <th>Time</th>
                <th></th>
              </tr>
            </thead>
            <tbody id="historyTbody">
              <!-- History rows -->
            </tbody>
          </table>
        </div>
      </div>

    </aside>
  </div>

  <script>
    // State management
    let graph = { size: 0, cost: 0, vertices: [], edges: [] };
    let runHistory = [];
    let bestCostOverall = Infinity;
    let mode = 'pan'; // 'pan' | 'add'
    let showLabels = true;
    let isPlaying = false;
    let playIndex = 0;
    let playTimer = null;

    // Viewport transform (Pan & Zoom)
    let zoom = 1.0;
    let panX = 0;
    let panY = 0;
    let isDragging = false;
    let dragStartX = 0;
    let dragStartY = 0;
    let draggedNodeIndex = -1;

    // Canvas setup
    const canvas = document.getElementById('tspCanvas');
    const ctx = canvas.getContext('2d');
    const tooltip = document.getElementById('tooltip');

    function resizeCanvas() {
      const dpr = window.devicePixelRatio || 1;
      const rect = canvas.getBoundingClientRect();
      canvas.width = rect.width * dpr;
      canvas.height = rect.height * dpr;
      ctx.scale(dpr, dpr);
      draw();
    }

    window.addEventListener('resize', resizeCanvas);

    // Initial setup
    window.addEventListener('DOMContentLoaded', async () => {
      resizeCanvas();
      setupDragDrop();
      setupCanvasEvents();
      setupFileInputs();
      onAlgoChange();
      await fetchGraphState();
      fitView();
    });

    // Native Bridge IPC Wrapper
    async function callNative(fnName, argsObj = {}) {
      const payload = typeof argsObj === 'string' ? argsObj : JSON.stringify(argsObj);
      for (let retry = 0; retry < 50; retry++) {
        if (typeof window[fnName] === 'function') break;
        await new Promise(r => setTimeout(r, 20));
      }
      if (typeof window[fnName] === 'function') {
        try {
          const res = await window[fnName](payload);
          if (typeof res === 'string') {
            try { return JSON.parse(res); } catch (_) { return { status: 'ok', raw: res }; }
          }
          return (res && typeof res === 'object') ? res : { status: 'ok', data: res };
        } catch (e) {
          console.error("Native call error: " + fnName, e);
          return { status: 'error', message: e ? (e.message || String(e)) : 'Execution failed' };
        }
      } else {
        console.warn("Native function not found: " + fnName);
        return { status: 'error', message: 'Native function ' + fnName + ' is not ready.' };
      }
    }

    async function fetchGraphState() {
      const res = await callNative('native_getGraphState');
      if (res && res.status === 'ok') {
        updateGraph(res);
      }
    }

    function updateGraph(data) {
      graph = data;
      document.getElementById('hudCost').innerText = data.cost ? data.cost.toFixed(2) : '0.00';
      document.getElementById('hudNodes').innerText = data.size;
      document.getElementById('resCost').innerText = data.cost ? data.cost.toFixed(2) : '0.00';
      draw();
    }

    // Canvas Drawing Engine
    function toScreenX(x) { return x * zoom + panX; }
    function toScreenY(y) { return y * zoom + panY; }
    function toWorldX(sx) { return (sx - panX) / zoom; }
    function toWorldY(sy) { return (sy - panY) / zoom; }

    function draw() {
      const rect = canvas.getBoundingClientRect();
      ctx.clearRect(0, 0, rect.width, rect.height);

      // Draw subtle grid
      drawGrid(rect.width, rect.height);

      if (!graph.vertices || graph.vertices.length === 0) {
        ctx.fillStyle = '#64748b';
        ctx.font = '14px sans-serif';
        ctx.textAlign = 'center';
        ctx.fillText('No nodes loaded. Click "Generate Map" or drop a .tsp file.', rect.width / 2, rect.height / 2);
        return;
      }

      // Draw Tour Paths (Edges)
      if (graph.edges && graph.edges.length === graph.vertices.length && graph.vertices.length > 1) {
        ctx.save();
        ctx.beginPath();
        const firstIdx = graph.edges[0];
        const v0 = graph.vertices[firstIdx];
        if (v0) {
          ctx.moveTo(toScreenX(v0.x), toScreenY(v0.y));
          const maxDraw = isPlaying ? playIndex : graph.edges.length;
          for (let i = 1; i < maxDraw; i++) {
            const vi = graph.vertices[graph.edges[i]];
            if (vi) ctx.lineTo(toScreenX(vi.x), toScreenY(vi.y));
          }
          if (!isPlaying || playIndex === graph.edges.length) {
            ctx.lineTo(toScreenX(v0.x), toScreenY(v0.y)); // Close loop
          }
        }
        ctx.strokeStyle = '#a855f7';
        ctx.lineWidth = Math.max(1.5, 2.5 * Math.min(1.0, zoom));
        ctx.shadowColor = 'rgba(168, 85, 247, 0.6)';
        ctx.shadowBlur = 8;
        ctx.stroke();
        ctx.restore();
      }

      // Draw City Nodes
      const nodeRadius = Math.max(3.5, Math.min(8, 5 * zoom));
      for (let i = 0; i < graph.vertices.length; i++) {
        const v = graph.vertices[i];
        const sx = toScreenX(v.x);
        const sy = toScreenY(v.y);

        if (sx < -20 || sx > rect.width + 20 || sy < -20 || sy > rect.height + 20) continue;

        ctx.save();
        ctx.beginPath();
        ctx.arc(sx, sy, nodeRadius, 0, Math.PI * 2);

        // Highlight start node or regular node
        if (i === 0) {
          ctx.fillStyle = '#10b981';
          ctx.shadowColor = 'rgba(16, 185, 129, 0.8)';
          ctx.shadowBlur = 10;
        } else {
          ctx.fillStyle = '#00f0ff';
          ctx.shadowColor = 'rgba(0, 240, 255, 0.7)';
          ctx.shadowBlur = 6;
        }
        ctx.fill();
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = 1.2;
        ctx.stroke();
        ctx.restore();

        // Node labels
        if (showLabels && graph.vertices.length <= 150) {
          ctx.fillStyle = '#cbd5e1';
          ctx.font = '10px sans-serif';
          ctx.fillText(v.id || (i + 1), sx + nodeRadius + 3, sy + 3);
        }
      }

      // Animated Playback Marker
      if (isPlaying && graph.edges.length > 0) {
        const currEdgeIdx = graph.edges[playIndex % graph.edges.length];
        const currV = graph.vertices[currEdgeIdx];
        if (currV) {
          const sx = toScreenX(currV.x);
          const sy = toScreenY(currV.y);
          ctx.save();
          ctx.beginPath();
          ctx.arc(sx, sy, nodeRadius * 2.2, 0, Math.PI * 2);
          ctx.fillStyle = 'rgba(245, 158, 11, 0.4)';
          ctx.strokeStyle = '#f59e0b';
          ctx.lineWidth = 2;
          ctx.shadowColor = '#f59e0b';
          ctx.shadowBlur = 15;
          ctx.fill();
          ctx.stroke();
          ctx.restore();
        }
      }
    }

    function drawGrid(w, h) {
      const gridSize = 50 * zoom;
      if (gridSize < 15) return;
      ctx.save();
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.03)';
      ctx.lineWidth = 1;
      const startX = panX % gridSize;
      const startY = panY % gridSize;
      ctx.beginPath();
      for (let x = startX; x < w; x += gridSize) {
        ctx.moveTo(x, 0); ctx.lineTo(x, h);
      }
      for (let y = startY; y < h; y += gridSize) {
        ctx.moveTo(0, y); ctx.lineTo(w, y);
      }
      ctx.stroke();
      ctx.restore();
    }

    // Viewport Controls
    function zoomIn() { setZoom(zoom * 1.25); }
    function zoomOut() { setZoom(zoom / 1.25); }
    function setZoom(newZoom) {
      const rect = canvas.getBoundingClientRect();
      const cx = rect.width / 2;
      const cy = rect.height / 2;
      const wx = toWorldX(cx);
      const wy = toWorldY(cy);
      zoom = Math.max(0.05, Math.min(20.0, newZoom));
      panX = cx - wx * zoom;
      panY = cy - wy * zoom;
      document.getElementById('hudZoom').innerText = Math.round(zoom * 100) + '%';
      draw();
    }

    function fitView() {
      if (!graph.vertices || graph.vertices.length === 0) return;
      let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
      for (const v of graph.vertices) {
        minX = Math.min(minX, v.x);
        maxX = Math.max(maxX, v.x);
        minY = Math.min(minY, v.y);
        maxY = Math.max(maxY, v.y);
      }
      const rect = canvas.getBoundingClientRect();
      const gw = Math.max(10, maxX - minX);
      const gh = Math.max(10, maxY - minY);
      const scaleX = (rect.width - 80) / gw;
      const scaleY = (rect.height - 80) / gh;
      zoom = Math.min(scaleX, scaleY, 2.5);
      panX = (rect.width - gw * zoom) / 2 - minX * zoom;
      panY = (rect.height - gh * zoom) / 2 - minY * zoom;
      document.getElementById('hudZoom').innerText = Math.round(zoom * 100) + '%';
      draw();
    }

    function setMode(newMode) {
      mode = newMode;
      document.getElementById('toolPan').classList.toggle('active', mode === 'pan');
      document.getElementById('toolAdd').classList.toggle('active', mode === 'add');
      canvas.className = mode === 'add' ? 'crosshair' : '';
    }

    function toggleLabels() {
      showLabels = !showLabels;
      document.getElementById('btnLabels').classList.toggle('active', showLabels);
      draw();
    }

    function togglePlayback() {
      isPlaying = !isPlaying;
      document.getElementById('btnPlay').classList.toggle('active', isPlaying);
      if (isPlaying) {
        playIndex = 1;
        playTimer = setInterval(() => {
          playIndex++;
          if (playIndex > graph.edges.length) {
            playIndex = 1;
          }
          draw();
        }, Math.max(15, 600 / graph.vertices.length));
      } else {
        clearInterval(playTimer);
        draw();
      }
    }

    // Canvas Events (Mouse & Drag)
    function setupCanvasEvents() {
      canvas.addEventListener('wheel', (e) => {
        e.preventDefault();
        const factor = e.deltaY < 0 ? 1.15 : 0.85;
        const rect = canvas.getBoundingClientRect();
        const mx = e.clientX - rect.left;
        const my = e.clientY - rect.top;
        const wx = toWorldX(mx);
        const wy = toWorldY(my);
        zoom = Math.max(0.05, Math.min(20.0, zoom * factor));
        panX = mx - wx * zoom;
        panY = my - wy * zoom;
        document.getElementById('hudZoom').innerText = Math.round(zoom * 100) + '%';
        draw();
      });

      canvas.addEventListener('mousedown', (e) => {
        const rect = canvas.getBoundingClientRect();
        const mx = e.clientX - rect.left;
        const my = e.clientY - rect.top;
        const wx = toWorldX(mx);
        const wy = toWorldY(my);

        if (mode === 'add') {
          callNative('native_addNode', { x: Math.round(wx), y: Math.round(wy) }).then(updateGraph);
          return;
        }

        // Check if clicking a node to drag
        draggedNodeIndex = findNodeAt(mx, my);
        isDragging = true;
        dragStartX = mx - panX;
        dragStartY = my - panY;

        if (draggedNodeIndex === -1) {
          canvas.classList.add('grabbing');
        }
      });

      window.addEventListener('mousemove', (e) => {
        const rect = canvas.getBoundingClientRect();
        const mx = e.clientX - rect.left;
        const my = e.clientY - rect.top;

        if (isDragging) {
          if (draggedNodeIndex >= 0) {
            const wx = toWorldX(mx);
            const wy = toWorldY(my);
            graph.vertices[draggedNodeIndex].x = Math.round(wx);
            graph.vertices[draggedNodeIndex].y = Math.round(wy);
            draw();
          } else {
            panX = mx - dragStartX;
            panY = my - dragStartY;
            draw();
          }
        } else {
          // Hover tooltip
          const hoverIdx = findNodeAt(mx, my);
          if (hoverIdx >= 0) {
            const v = graph.vertices[hoverIdx];
            tooltip.style.display = 'block';
            tooltip.style.left = (e.clientX + 12) + 'px';
            tooltip.style.top = (e.clientY + 12) + 'px';
            tooltip.innerHTML = `<strong>City ${v.id || hoverIdx+1}</strong><br>X: ${v.x}, Y: ${v.y}`;
          } else {
            tooltip.style.display = 'none';
          }
        }
      });

      window.addEventListener('mouseup', () => {
        if (isDragging && draggedNodeIndex >= 0) {
          const v = graph.vertices[draggedNodeIndex];
          callNative('native_updateNodePosition', { index: draggedNodeIndex, x: v.x, y: v.y }).then(updateGraph);
        }
        isDragging = false;
        draggedNodeIndex = -1;
        canvas.classList.remove('grabbing');
      });
    }

    function findNodeAt(sx, sy) {
      if (!graph.vertices) return -1;
      const hitRadius = Math.max(8, 10 * zoom);
      for (let i = graph.vertices.length - 1; i >= 0; i--) {
        const v = graph.vertices[i];
        const vx = toScreenX(v.x);
        const vy = toScreenY(v.y);
        const dist = Math.hypot(sx - vx, sy - vy);
        if (dist <= hitRadius) return i;
      }
      return -1;
    }

    const ALGO_METADATA = {
      'greedy': { name: 'Greedy Search (greedy)', tag: 'Constructive', desc: 'Connects nearest unvisited cities step-by-step.' },
      'greedyls': { name: 'Greedy + Local Search', tag: 'Hybrid Heuristic', desc: 'Greedy initial tour refined with 2-opt local search.' },
      'greedyls+': { name: 'Greedy + Extended LS', tag: 'Iterative Heuristic', desc: 'Runs greedy from every starting city with 2-opt refinement.' },
      'grasp': { name: 'GRASP (Randomized Greedy)', tag: 'Constructive Metaheuristic', desc: 'Constructs probabilistic greedy tours followed by local search.' },
      'grasp+': { name: 'GRASP Extended', tag: 'Constructive Metaheuristic', desc: 'Enhanced GRASP with expanded candidate list exploration.' },
      'rs': { name: 'Random Search (rs)', tag: 'Baseline Search', desc: 'Samples random permutations to find the lowest cost cycle.' },
      'ls': { name: 'Local Search (2-Opt)', tag: 'Neighborhood Search', desc: 'Iteratively swaps pairs of edges to remove path crossings.' },
      'vnd': { name: 'Variable Neighborhood Descent', tag: 'Deterministic Descent', desc: 'Systematically explores multiple neighborhood operators.' },
      'bmb': { name: 'Basic Multiboot (BMB)', tag: 'Multistart Search', desc: 'Restarts 2-opt local search from multiple random initial solutions.' },
      'ils': { name: 'Iterated Local Search (ILS)', tag: 'Perturbation Search', desc: 'Perturbs local optima (double-bridge) and applies 2-opt search.' },
      'vns': { name: 'Variable Neighborhood Search (VNS)', tag: 'Stochastic Neighborhood', desc: 'Combines stochastic shaking across neighborhoods with local descent.' },
      'tabu': { name: 'Tabu Search (tabu)', tag: 'Trajectory Metaheuristic', desc: 'Neighborhood search with short-term tabu list and long-term memory restarts.' },
      'sa': { name: 'Simulated Annealing (sa)', tag: 'Thermodynamic Metaheuristic', desc: 'Probabilistically accepts worsening moves to escape local optima.' },
      'ga': { name: 'Genetic Algorithm (ga)', tag: 'Evolutionary Algorithm', desc: 'Evolves a population of tours using crossover, mutation, and selection.' },
      'ma': { name: 'Memetic Algorithm (ma)', tag: 'Hybrid Evolutionary', desc: 'Genetic algorithm enhanced with local search optimization on individuals.' },
      'psa': { name: 'Parallel Simulated Annealing', tag: 'Parallel Metaheuristic', desc: 'Executes concurrent SA chains with periodic solution migration.' },
      'pga': { name: 'Parallel Genetic Algorithm', tag: 'Island Model GA', desc: 'Simulates multiple evolving islands with ring or star topology migration.' }
    };

    // Dynamic Parameter Visibility & Info Card
    function onAlgoChange() {
      const algo = document.getElementById('algoSelect').value;
      const show = (id, visible) => document.getElementById(id).style.display = visible ? 'block' : 'none';

      show('paramIterations', !['greedy', 'ls', 'greedyls'].includes(algo));
      show('paramSeed', algo !== 'greedy' && algo !== 'greedyls');
      show('paramPopSize', ['ga', 'ma', 'pga'].includes(algo));
      show('paramNeighbor', algo === 'sa');
      show('paramScheme', algo === 'ga');
      show('paramHybrid', algo === 'ma');
      show('paramProcesses', ['psa', 'pga'].includes(algo));
      show('paramLatency', ['psa', 'pga'].includes(algo));
      show('paramTopology', algo === 'pga');

      // Update Info Badge Card
      const meta = ALGO_METADATA[algo] || { name: algo, tag: 'Algorithm', desc: '' };
      document.getElementById('algoBadgeName').innerText = meta.name;
      document.getElementById('algoBadgeTag').innerText = meta.tag;
      document.getElementById('algoBadgeDesc').innerText = meta.desc;

      // Update button text
      const btnText = document.getElementById('btnExecText');
      if (btnText && !document.getElementById('btnExecute').classList.contains('computing')) {
        btnText.innerText = `▶️ Execute ${meta.name.split(' ')[0]}`;
      }

      // Adjust defaults
      if (algo === 'greedyls+') document.getElementById('inputCount').value = 5;
      else if (['bmb', 'grasp', 'ils', 'vns', 'tabu', 'pga'].includes(algo)) document.getElementById('inputCount').value = 50;
      else if (algo === 'grasp+') document.getElementById('inputCount').value = 10;
      else if (algo === 'psa') document.getElementById('inputCount').value = 20;
      else if (['rs', 'vnd', 'sa', 'ga', 'ma'].includes(algo)) document.getElementById('inputCount').value = 2000;
    }

    function randomizeSeed() {
      document.getElementById('inputSeed').value = Math.floor(Math.random() * 1000000) + 1;
    }

    // Map Management
    async function generateMap() {
      const count = parseInt(document.getElementById('nodeCountSlider').value);
      const mode = document.getElementById('mapPattern').value;
      const rect = canvas.getBoundingClientRect();
      const res = await callNative('native_generateRandomMap', {
        count: count,
        width: rect.width || 800,
        height: rect.height || 600,
        mode: mode,
        seed: 0
      });
      if (res && res.status === 'ok') {
        updateGraph(res);
        fitView();
      }
    }

    async function clearMap() {
      const res = await callNative('native_clearMap');
      if (res && res.status === 'ok') {
        updateGraph(res);
      }
    }

    async function onPresetChange() {
      const preset = document.getElementById('presetSelect').value;
      if (!preset) return;
      const res = await callNative('native_loadPreset', { name: preset });
      if (res && res.status === 'ok') {
        updateGraph(res);
        fitView();
      } else {
        alert((res && res.message) || "Failed to load preset");
      }
    }

    // Algorithm Execution
    async function runAlgorithm() {
      const btn = document.getElementById('btnExecute');
      const btnText = document.getElementById('btnExecText');
      btn.classList.add('computing');
      btnText.innerHTML = '<span class="spinner"></span> Running...';

      const algo = document.getElementById('algoSelect').value;
      const count = parseInt(document.getElementById('inputCount').value) || 2000;
      const seed = parseInt(document.getElementById('inputSeed').value) || 0;
      const size = parseInt(document.getElementById('inputPopSize').value) || 30;
      const method = document.getElementById('selectNeighbor').value;
      const scheme = document.getElementById('selectScheme').value;
      const hybridization = document.getElementById('selectHybrid').value;
      const processes = parseInt(document.getElementById('inputProcesses').value) || 4;
      const migrLatency = parseInt(document.getElementById('inputLatency').value) || 2;
      const topology = document.getElementById('selectTopology').value;

      const params = {
        algorithm: algo,
        count: count,
        seed: seed,
        size: size,
        method: method,
        scheme: scheme,
        hybridization: hybridization,
        processes: processes,
        migrLatency: migrLatency,
        topology: topology
      };

      try {
        const res = await callNative('native_runAlgorithm', params);
        if (res && res.status === 'ok') {
          const costVal = (typeof res.cost === 'number') ? res.cost : (parseFloat(res.cost) || 0);
          const initVal = (typeof res.initialCost === 'number') ? res.initialCost : (parseFloat(res.initialCost) || costVal);
          const timeVal = (typeof res.timeMs === 'number') ? res.timeMs : (parseFloat(res.timeMs) || 0);

          graph.cost = costVal;
          if (res.edges && Array.isArray(res.edges)) {
            graph.edges = res.edges;
          }
          updateGraph(graph);

          // Update metrics
          document.getElementById('resInitialCost').innerText = initVal > 0 ? initVal.toFixed(1) : '-';
          if (initVal > 0 && costVal > 0 && initVal >= costVal) {
            const diff = initVal - costVal;
            const pct = ((diff / initVal) * 100).toFixed(1);
            document.getElementById('resImprovement').innerText = `-${pct}%`;
          } else {
            document.getElementById('resImprovement').innerText = '-';
          }
          document.getElementById('resTime').innerText = `${timeVal.toFixed(1)} ms`;

          addHistoryEntry({
            algorithm: res.algorithm || algo,
            cost: costVal,
            timeMs: timeVal,
            edges: graph.edges
          });
        } else {
          alert((res && res.message) || "Algorithm execution failed");
        }
      } catch (e) {
        console.error("Algorithm run error:", e);
      } finally {
        btn.classList.remove('computing');
        const currentMeta = ALGO_METADATA[document.getElementById('algoSelect').value] || { name: 'Algorithm' };
        btnText.innerText = `▶️ Execute ${currentMeta.name.split(' ')[0]}`;
      }
    }

    function addHistoryEntry(entryData) {
      if (!entryData) return;
      const costVal = (typeof entryData.cost === 'number') ? entryData.cost : (parseFloat(entryData.cost) || 0);
      const timeVal = (typeof entryData.timeMs === 'number') ? entryData.timeMs : (parseFloat(entryData.timeMs) || 0);
      const isBest = costVal > 0 && costVal < bestCostOverall;
      if (isBest) bestCostOverall = costVal;

      runHistory.unshift({
        algo: entryData.algorithm || document.getElementById('algoSelect').value,
        cost: costVal,
        timeMs: timeVal,
        edges: (entryData.edges && Array.isArray(entryData.edges)) ? [...entryData.edges] : [],
        isBest: isBest
      });

      renderHistory();
    }

    function renderHistory() {
      const tbody = document.getElementById('historyTbody');
      tbody.innerHTML = '';
      runHistory.slice(0, 10).forEach((entry, idx) => {
        const tr = document.createElement('tr');
        const costStr = (typeof entry.cost === 'number' && !isNaN(entry.cost)) ? entry.cost.toFixed(1) : '0.0';
        const timeStr = (typeof entry.timeMs === 'number' && !isNaN(entry.timeMs)) ? entry.timeMs.toFixed(0) : '0';
        tr.innerHTML = `
          <td><strong>${entry.algo}</strong></td>
          <td>${costStr} ${entry.isBest ? '<span class="tag-best">BEST</span>' : ''}</td>
          <td>${timeStr}ms</td>
          <td><button class="btn-sm" onclick="restoreTour(${idx})">Apply</button></td>
        `;
        tbody.appendChild(tr);
      });
    }

    function restoreTour(idx) {
      if (runHistory[idx]) {
        graph.edges = [...runHistory[idx].edges];
        graph.cost = runHistory[idx].cost;
        updateGraph(graph);
      }
    }

    function clearHistory() {
      runHistory = [];
      bestCostOverall = Infinity;
      renderHistory();
    }

    // File Drag & Drop
    function setupDragDrop() {
      const overlay = document.getElementById('dropOverlay');
      window.addEventListener('dragover', (e) => { e.preventDefault(); overlay.classList.add('active'); });
      window.addEventListener('dragleave', (e) => { if (e.clientX <= 0 || e.clientY <= 0) overlay.classList.remove('active'); });
      window.addEventListener('drop', (e) => {
        e.preventDefault();
        overlay.classList.remove('active');
        if (e.dataTransfer.files.length > 0) {
          handleFile(e.dataTransfer.files[0]);
        }
      });
    }

    function setupFileInputs() {
      document.getElementById('tspFileInput').addEventListener('change', (e) => {
        if (e.target.files.length > 0) handleFile(e.target.files[0]);
      });
      document.getElementById('tourFileInput').addEventListener('change', (e) => {
        if (e.target.files.length > 0) handleFile(e.target.files[0]);
      });
    }

    function handleFile(file) {
      const reader = new FileReader();
      reader.onload = async (e) => {
        const content = e.target.result;
        if (file.name.endsWith('.tour')) {
          const res = await callNative('native_loadTourString', { content: content });
          if (res && res.status === 'ok') updateGraph(res);
          else alert((res && res.message) || "Failed to load tour");
        } else {
          const res = await callNative('native_loadTspString', { content: content });
          if (res && res.status === 'ok') {
            updateGraph(res);
            fitView();
          } else alert((res && res.message) || "Failed to load TSP");
        }
      };
      reader.readAsText(file);
    }

    async function exportTsp() {
      const res = await callNative('native_getTspContent');
      if (res && res.content) {
        downloadFile(res.content, 'problem.tsp', 'text/plain');
      }
    }

    async function exportTour() {
      const res = await callNative('native_getTourContent');
      if (res && res.content) {
        downloadFile(res.content, 'solution.tour', 'text/plain');
      }
    }

    function exportImage() {
      const link = document.createElement('a');
      link.download = 'tsp_solution.png';
      link.href = canvas.toDataURL('image/png');
      link.click();
    }

    function downloadFile(text, filename, type) {
      const blob = new Blob([text], { type: type });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = filename;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }

    // Automated UI Simulator & Test Suite
    let isSelfTesting = false;
    async function runAutomatedSelfTest() {
      if (isSelfTesting) return;
      isSelfTesting = true;
      console.log("=== STARTING AUTOMATED UI SELF-TEST ===");
      let passed = 0;
      let failed = 0;

      async function testCase(name, fn) {
        try {
          console.log(`[TEST-RUN] ${name}...`);
          await fn();
          console.log(`[TEST-PASS] ${name}`);
          passed++;
        } catch (e) {
          console.error(`[TEST-FAIL] ${name}:`, e);
          failed++;
        }
      }

      // Test 1: Fetch initial state
      await testCase("Initial Graph State", async () => {
        await fetchGraphState();
        if (!graph.vertices || graph.vertices.length < 3) throw new Error("Invalid vertex count: " + (graph.vertices ? graph.vertices.length : 0));
      });

      // Test 2: Presets
      await testCase("Load Preset berlin52", async () => {
        document.getElementById('presetSelect').value = 'berlin52';
        await onPresetChange();
        if (graph.vertices.length !== 52) throw new Error(`Expected 52 vertices, got ${graph.vertices.length}`);
      });

      // Test 3: Generate Maps
      await testCase("Generate Uniform Map (30 cities)", async () => {
        document.getElementById('nodeCountSlider').value = 30;
        document.getElementById('mapPattern').value = 'uniform';
        await generateMap();
        if (graph.vertices.length !== 30) throw new Error("Uniform map generation failed");
      });

      await testCase("Generate Cluster Map (25 cities)", async () => {
        document.getElementById('nodeCountSlider').value = 25;
        document.getElementById('mapPattern').value = 'clusters';
        await generateMap();
        if (graph.vertices.length !== 25) throw new Error("Cluster map generation failed");
      });

      await testCase("Generate Circle Map (20 cities)", async () => {
        document.getElementById('nodeCountSlider').value = 20;
        document.getElementById('mapPattern').value = 'circle';
        await generateMap();
        if (graph.vertices.length !== 20) throw new Error("Circle map generation failed");
      });

      // Test 4: Node adding and editing
      await testCase("Add Custom Node", async () => {
        const initialLen = graph.vertices.length;
        const res = await callNative('native_addNode', { x: 500, y: 300 });
        if (res.status !== 'ok') throw new Error(res.message);
        updateGraph(res);
        if (graph.vertices.length !== initialLen + 1) throw new Error("Node addition count mismatch");
      });

      // Test 5: UI Controls & Viewport
      await testCase("Viewport & Display Controls", async () => {
        zoomIn();
        zoomOut();
        fitView();
        toggleLabels();
        toggleLabels();
        togglePlayback();
        await new Promise(r => setTimeout(r, 100));
        togglePlayback();
      });

      // Test 6: Execute all 16 Algorithms sequentially
      const algos = ['greedy', 'rs', 'ls', 'vnd', 'sa', 'greedyls', 'greedyls+', 'bmb', 'grasp', 'grasp+', 'ils', 'vns', 'ga', 'ma', 'psa', 'pga'];
      for (const algo of algos) {
        await testCase(`Execute Algorithm [${algo}]`, async () => {
          document.getElementById('algoSelect').value = algo;
          onAlgoChange();
          document.getElementById('inputCount').value = 50; // Fast iteration for testing
          document.getElementById('inputPopSize').value = 10;
          document.getElementById('inputProcesses').value = 2;
          await runAlgorithm();
          if (!graph.cost || graph.cost <= 0) throw new Error(`Invalid cost for ${algo}: ${graph.cost}`);
        });
      }

      // Test 7: Tour & TSP export
      await testCase("Export Functions", async () => {
        const tspRes = await callNative('native_getTspContent');
        if (!tspRes || !tspRes.content) throw new Error("Failed to get TSP content");
        const tourRes = await callNative('native_getTourContent');
        if (!tourRes || !tourRes.content) throw new Error("Failed to get Tour content");
      });

      console.log(`=== TEST SUMMARY: ${passed} PASSED, ${failed} FAILED ===`);
      await callNative('native_onTestFinished', { passed: passed, failed: failed });
    }
  </script>
</body>
</html>
)rawliteral";

} // namespace GuiAssets

#endif // GUI_ASSETS_H
