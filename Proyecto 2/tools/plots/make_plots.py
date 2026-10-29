#!/usr/bin/env python3
"""Generate the figures of the Maze Craze experimental report.

Reads the three CSV files the project tools produce and writes one PNG per
figure. No CSV is modified.

Usage:
    python3 tools/plots/make_plots.py
    python3 tools/plots/make_plots.py --lang es --out-dir report/figures_es
    python3 tools/plots/make_plots.py --only fig2 fig5

Inputs (in --data-dir):
    maze_comparison.csv    120 rows, tools/MazeAnalysis.cpp
    path_comparison.csv    180 rows, tools/PathComparison.cpp
    benchmark_results.csv   36 rows, tools/BenchmarkRunner.cpp

Outputs (in --out-dir): fig1 .. fig7 as PNG at 200 dpi, numbered in the order
they appear in the report. Every label is drawn from the TEXT table below, so
--lang en and --lang es give the same figures with the text of each report.
"""

import argparse
import os
import sys

import pandas as pd
import matplotlib.pyplot as plt

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mazestyle import (apply_style, tidy_axes, save, SERIES, GENERATOR,
                       LEVEL_COLORS, MARKERS, LINESTYLES, SURFACE, INK_SOFT)

# Fixed series order: never cycled, never reassigned by a filter
ALGORITHMS = ["BFS", "Dijkstra", "A*"]

# The three measurements the report excludes from the growth table. Their
# visited node counts are normal, so the cause is the operating system
# scheduler and not the algorithm. Identified by (seed, rows, mud rate,
# algorithm). Re-running the benchmarks invalidates this list: the script
# warns when it does not find all three.
OUTLIERS = [
    (7, 30, 0.5, "A*"),
    (7, 30, 0.5, "Dijkstra"),
    (2026, 40, 0.5, "BFS"),
]

# Every string that reaches the reader, in both report languages
TEXT = {
    "en": {
        "cells": "Maze cells (V)",
        "cpu_ms": "CPU time (ms)",
        "per_cell": "Time per cell (µs)",
        "visited": "Cells opened (average)",
        "whole_maze": "whole maze",
        "fig_cpu_total": "Search time against maze size",
        "fig_per_cell": "Dijkstra pays a growing cost per cell; BFS and A* do not",
        "fig_visited": "A* opens fewer cells; BFS and Dijkstra open almost all",
        "mud": "mud",
        "astar_pct": "Cells opened by A* (% of the maze)",
        "maze_size": "Maze size",
        "fig_astar_mud": "The advantage of A* shrinks as the mud grows",
        "optimal_cost": "Optimal cost (Dijkstra and A*)",
        "bfs_cost": "Cost BFS returns",
        "bfs_equals": "BFS = optimal",
        "empty_region": "No point lands here:\nBFS is never cheaper",
        "fig_bfs_title": "BFS minimises steps, not cost\n{worse} of {total} runs above the diagonal",
        "path_cost": "Path cost",
        "dead_ends": "Dead ends",
        "fig_gen_cost": "Corner-to-corner path cost",
        "fig_gen_ends": "Dead ends",
        "fig_gen_title": "Both generators converge once cycles are added (20x20 mazes)",
        "turns": "Turns",
        "fig_match_cost": "Path cost between players",
        "fig_match_turns": "Match length",
        "fig_match_mud": "Effect of the mud",
        "mud_rate": "Mud rate",
        "fig_match_title": "36 complete matches: the generator decides how long a match lasts",
    },
    "es": {
        "cells": "Celdas del laberinto (V)",
        "cpu_ms": "Tiempo de CPU (ms)",
        "per_cell": "Tiempo por celda (µs)",
        "visited": "Nodos abiertos (promedio)",
        "whole_maze": "laberinto completo",
        "fig_cpu_total": "Tiempo de búsqueda contra el tamaño del laberinto",
        "fig_per_cell": "Dijkstra paga un costo por celda creciente; BFS y A* no",
        "fig_visited": "A* abre menos celdas; BFS y Dijkstra abren casi todo",
        "mud": "barro",
        "astar_pct": "Celdas abiertas por A* (% del laberinto)",
        "maze_size": "Tamaño del laberinto",
        "fig_astar_mud": "La ventaja de A* se reduce conforme aumenta el barro",
        "optimal_cost": "Costo óptimo (Dijkstra y A*)",
        "bfs_cost": "Costo que devuelve BFS",
        "bfs_equals": "BFS = óptimo",
        "empty_region": "Ningún punto cae aquí:\nBFS nunca es más barato",
        "fig_bfs_title": "BFS minimiza pasos, no costo\n{worse} de {total} corridas por encima de la diagonal",
        "path_cost": "Costo de ruta",
        "dead_ends": "Callejones sin salida",
        "fig_gen_cost": "Costo de la ruta esquina a esquina",
        "fig_gen_ends": "Callejones sin salida",
        "fig_gen_title": "Los dos generadores convergen al agregar ciclos (laberintos de 20x20)",
        "turns": "Turnos",
        "fig_match_cost": "Costo de la ruta entre jugadores",
        "fig_match_turns": "Duración de la partida",
        "fig_match_mud": "Efecto del barro",
        "mud_rate": "Tasa de barro",
        "fig_match_title": "36 partidas completas: el generador decide cuánto dura una partida",
    },
}


# --- Data loading -----------------------------------------------------------

def load_csv(data_dir, name):
    path = os.path.join(data_dir, name)
    if not os.path.exists(path):
        print(f"[WARNING] {path} not found, its figures are skipped")
        return None
    return pd.read_csv(path)


def drop_outliers(df):
    """Return the dataframe without the three documented outliers."""
    mask = pd.Series(False, index=df.index)
    for seed, rows, mud, algorithm in OUTLIERS:
        mask |= ((df["seed"] == seed) & (df["rows"] == rows)
                 & (df["mudRate"] == mud) & (df["algorithm"] == algorithm))
    removed = int(mask.sum())
    if removed != len(OUTLIERS):
        print(f"[WARNING] expected {len(OUTLIERS)} outliers but found "
              f"{removed}; review the OUTLIERS list")
    return df[~mask].copy()


# --- Search figures (path_comparison.csv) -----------------------------------

def plot_cpu_total(paths, out_dir, t):
    """Total CPU time against the number of cells, per algorithm."""
    df = drop_outliers(paths)
    df = df.assign(cells=df["rows"] * df["cols"])
    grouped = df.groupby(["cells", "algorithm"])["cpuTimeMs"].mean().unstack()

    fig, ax = plt.subplots(figsize=(6.4, 4.0))
    for name in ALGORITHMS:
        ax.plot(grouped.index, grouped[name], label=name,
                color=SERIES[name], marker=MARKERS[name],
                linestyle=LINESTYLES[name])
        # Direct label on the last point: identity never rests on color alone
        ax.annotate(name, (grouped.index[-1], grouped[name].iloc[-1]),
                    xytext=(6, 0), textcoords="offset points",
                    va="center", fontsize=9, color=INK_SOFT)

    ax.set_xlabel(t["cells"])
    ax.set_ylabel(t["cpu_ms"])
    ax.set_title(t["fig_cpu_total"])
    ax.set_xticks(sorted(df["cells"].unique()))
    ax.set_xlim(right=grouped.index[-1] * 1.14)
    ax.legend(loc="upper left")
    tidy_axes(ax)
    save(fig, os.path.join(out_dir, "fig6_cpu_total.png"))


def plot_cpu_por_celda(paths, out_dir, t):
    """Time per cell: the figure that carries the complexity argument."""
    df = drop_outliers(paths)
    df = df.assign(cells=df["rows"] * df["cols"])
    # Microseconds per cell: 0.000151 ms does not read on an axis
    df = df.assign(per_cell=df["cpuTimeMs"] / df["cells"] * 1000.0)
    grouped = df.groupby(["cells", "algorithm"])["per_cell"].mean().unstack()

    fig, ax = plt.subplots(figsize=(6.4, 4.0))
    for name in ALGORITHMS:
        ax.plot(grouped.index, grouped[name], label=name,
                color=SERIES[name], marker=MARKERS[name],
                linestyle=LINESTYLES[name])
        ax.annotate(name, (grouped.index[-1], grouped[name].iloc[-1]),
                    xytext=(6, 0), textcoords="offset points",
                    va="center", fontsize=9, color=INK_SOFT)

    ax.set_xlabel(t["cells"])
    ax.set_ylabel(t["per_cell"])
    ax.set_title(t["fig_per_cell"])
    ax.set_xticks(sorted(df["cells"].unique()))
    ax.set_xlim(right=grouped.index[-1] * 1.14)
    ax.set_ylim(bottom=0)
    ax.legend(loc="lower left")
    tidy_axes(ax)
    save(fig, os.path.join(out_dir, "fig7_cpu_por_celda.png"))


def plot_nodos_visitados(paths, out_dir, t):
    """Cells opened against the total number of cells in the maze."""
    df = paths.assign(cells=paths["rows"] * paths["cols"])
    grouped = df.groupby(["cells", "algorithm"])["visitedNodes"].mean().unstack()

    fig, ax = plt.subplots(figsize=(6.4, 4.0))
    # Reference: opening the entire maze
    ax.plot(grouped.index, grouped.index, color="#c3c2b7", linewidth=1.2,
            linestyle=":", zorder=1)
    ax.annotate(t["whole_maze"], (grouped.index[-1], grouped.index[-1]),
                xytext=(-4, 8), textcoords="offset points",
                ha="right", fontsize=8, color="#898781")

    for name in ALGORITHMS:
        ax.plot(grouped.index, grouped[name], label=name,
                color=SERIES[name], marker=MARKERS[name],
                linestyle=LINESTYLES[name], zorder=2)

    ax.set_xlabel(t["cells"])
    ax.set_ylabel(t["visited"])
    ax.set_title(t["fig_visited"])
    ax.set_xticks(sorted(df["cells"].unique()))
    ax.legend(loc="upper left")
    tidy_axes(ax)
    save(fig, os.path.join(out_dir, "fig3_nodos_visitados.png"))


def plot_astar_vs_barro(paths, out_dir, t):
    """Percentage of the maze A* opens, by size and mud rate."""
    df = paths[paths["algorithm"] == "A*"].copy()
    df["cells"] = df["rows"] * df["cols"]
    df["pct"] = df["visitedNodes"] / df["cells"] * 100.0
    grouped = df.groupby(["cells", "mudRate"])["pct"].mean().unstack()

    sizes = list(grouped.index)
    mud_rates = list(grouped.columns)
    x = range(len(sizes))
    width = 0.26

    fig, ax = plt.subplots(figsize=(6.4, 4.0))
    for i, mud in enumerate(mud_rates):
        offset = (i - (len(mud_rates) - 1) / 2) * width
        bars = ax.bar([p + offset for p in x], grouped[mud], width * 0.92,
                      label=f"{t['mud']} {mud}", color=LEVEL_COLORS[i],
                      edgecolor=SURFACE, linewidth=1.5)
        # Direct label on every bar: relief for the low-contrast aqua slot
        ax.bar_label(bars, fmt="%.0f%%", padding=2, fontsize=8,
                     color=INK_SOFT)

    ax.set_xticks(list(x))
    ax.set_xticklabels([f"{int(c ** 0.5)}x{int(c ** 0.5)}" for c in sizes])
    ax.set_xlabel(t["maze_size"])
    ax.set_ylabel(t["astar_pct"])
    ax.set_title(t["fig_astar_mud"])
    ax.set_ylim(0, 100)
    ax.legend(loc="upper left", ncol=3)
    tidy_axes(ax)
    save(fig, os.path.join(out_dir, "fig4_astar_vs_barro.png"))


def plot_bfs_no_minimiza(paths, out_dir, t):
    """The cost BFS returns against the optimal cost, run by run."""
    key = ["seed", "rows", "cols", "mudRate"]
    bfs = paths[paths["algorithm"] == "BFS"].set_index(key)["pathCost"]
    opt = paths[paths["algorithm"] == "Dijkstra"].set_index(key)["pathCost"]
    merged = pd.DataFrame({"bfs": bfs, "optimal": opt}).reset_index()

    fig, ax = plt.subplots(figsize=(5.6, 5.2))
    low = min(merged["optimal"].min(), merged["bfs"].min()) - 4
    high = max(merged["optimal"].max(), merged["bfs"].max()) + 4
    # The diagonal: below it is impossible, above it BFS overpays
    ax.plot([low, high], [low, high], color="#c3c2b7", linewidth=1.2,
            linestyle=":", zorder=1)
    # The lower right triangle is empty by construction, so it is labelled
    # instead of the diagonal, where the text would collide with the points
    ax.annotate(t["bfs_equals"], (high, high), xytext=(-8, -16),
                textcoords="offset points", ha="right", fontsize=8,
                color="#898781", rotation=45, rotation_mode="anchor")
    ax.text(high - (high - low) * 0.04, low + (high - low) * 0.08,
            t["empty_region"], ha="right", va="bottom", fontsize=8.5,
            color="#898781", linespacing=1.4)

    markers = ["o", "s", "^"]
    for i, mud in enumerate(sorted(merged["mudRate"].unique())):
        subset = merged[merged["mudRate"] == mud]
        ax.scatter(subset["optimal"], subset["bfs"], s=42,
                   label=f"{t['mud']} {mud}", color=LEVEL_COLORS[i],
                   marker=markers[i], edgecolor=SURFACE, linewidth=1.0,
                   zorder=2, alpha=0.9)

    worse = int((merged["bfs"] > merged["optimal"]).sum())
    ax.set_xlabel(t["optimal_cost"])
    ax.set_ylabel(t["bfs_cost"])
    ax.set_title(t["fig_bfs_title"].format(worse=worse, total=len(merged)))
    ax.set_xlim(low, high)
    ax.set_ylim(low, high)
    ax.set_aspect("equal")
    ax.legend(loc="upper left")
    tidy_axes(ax, grid_axis="both")
    save(fig, os.path.join(out_dir, "fig2_bfs_no_minimiza.png"))


# --- Generation figure (maze_comparison.csv) --------------------------------

def plot_generacion(mazes, out_dir, t):
    """Maze shape: path cost and dead ends against the extra edges."""
    df = mazes[(mazes["rows"] == 20) & (mazes["cols"] == 20)].copy()
    cost = df.groupby(["extraEdgeProb", "algorithm"])["pathCost"].mean().unstack()
    ends = df.groupby(["extraEdgeProb", "algorithm"])["deadEnds"].mean().unstack()

    fig, axes = plt.subplots(1, 2, figsize=(9.0, 4.0))
    panels = [
        (axes[0], cost, t["fig_gen_cost"], t["path_cost"]),
        (axes[1], ends, t["fig_gen_ends"], t["dead_ends"]),
    ]
    for ax, table, title, ylabel in panels:
        for name in ["DFS", "BFS"]:
            ax.plot(table.index, table[name], label=name,
                    color=GENERATOR[name],
                    marker="o" if name == "DFS" else "s",
                    linestyle="-" if name == "DFS" else "--")
        ax.set_xlabel("EXTRA_EDGE_PROB")
        ax.set_ylabel(ylabel)
        ax.set_title(title)
        ax.set_xticks(sorted(df["extraEdgeProb"].unique()))
        ax.set_ylim(bottom=0)
        ax.legend()
        tidy_axes(ax)

    fig.suptitle(t["fig_gen_title"], fontsize=11, fontweight="bold",
                 color="#0b0b0b")
    fig.tight_layout()
    save(fig, os.path.join(out_dir, "fig1_generacion.png"))


# --- Match figure (benchmark_results.csv) -----------------------------------

def plot_partidas(matches, out_dir, t):
    """What the generator and the mud mean for the player."""
    matches = matches.copy()
    matches["size"] = (matches["rows"].astype(str) + "x"
                       + matches["cols"].astype(str))
    size_order = sorted(matches["size"].unique(),
                        key=lambda s: int(s.split("x")[0]))

    cost = (matches.groupby(["size", "generator"])["pathCost"].mean()
            .unstack().reindex(size_order))
    turns = (matches.groupby(["size", "generator"])["turns"].mean()
             .unstack().reindex(size_order))
    by_mud = matches.groupby(["mudRate", "generator"])["pathCost"].mean().unstack()

    fig, axes = plt.subplots(1, 3, figsize=(12.0, 4.0))
    x = range(len(size_order))
    width = 0.34

    for ax, table, title, ylabel in [
        (axes[0], cost, t["fig_match_cost"], t["path_cost"]),
        (axes[1], turns, t["fig_match_turns"], t["turns"]),
    ]:
        for i, name in enumerate(["DFS", "BFS"]):
            offset = (i - 0.5) * width
            bars = ax.bar([p + offset for p in x], table[name], width * 0.92,
                          label=name, color=GENERATOR[name],
                          edgecolor=SURFACE, linewidth=1.5)
            ax.bar_label(bars, fmt="%.1f", padding=2, fontsize=8,
                         color=INK_SOFT)
        ax.set_xticks(list(x))
        ax.set_xticklabels(size_order)
        ax.set_xlabel(t["maze_size"])
        ax.set_ylabel(ylabel)
        ax.set_title(title)
        # Headroom so the legend does not sit on top of the bars
        ax.set_ylim(top=table.to_numpy().max() * 1.28)
        ax.legend(loc="upper left")
        tidy_axes(ax)

    ax = axes[2]
    for name in ["DFS", "BFS"]:
        ax.plot(by_mud.index, by_mud[name], label=name,
                color=GENERATOR[name],
                marker="o" if name == "DFS" else "s",
                linestyle="-" if name == "DFS" else "--")
    ax.set_xlabel(t["mud_rate"])
    ax.set_ylabel(t["path_cost"])
    ax.set_title(t["fig_match_mud"])
    ax.set_xticks(sorted(matches["mudRate"].unique()))
    ax.set_ylim(bottom=0)
    ax.legend()
    tidy_axes(ax)

    fig.suptitle(t["fig_match_title"], fontsize=11, fontweight="bold",
                 color="#0b0b0b")
    fig.tight_layout()
    save(fig, os.path.join(out_dir, "fig5_partidas.png"))


# --- Entry point ------------------------------------------------------------

FIGURES = {
    # The key is the figure number in the report, in reading order
    "fig1": ("mazes", plot_generacion),
    "fig2": ("paths", plot_bfs_no_minimiza),
    "fig3": ("paths", plot_nodos_visitados),
    "fig4": ("paths", plot_astar_vs_barro),
    "fig5": ("matches", plot_partidas),
    "fig6": ("paths", plot_cpu_total),
    "fig7": ("paths", plot_cpu_por_celda),
}


def main():
    parser = argparse.ArgumentParser(
        description="Generate the figures of the Maze Craze report")
    parser.add_argument("--data-dir", default="data",
                        help="Folder holding the three CSV files")
    parser.add_argument("--out-dir", default="report/figures",
                        help="Output folder")
    parser.add_argument("--lang", default="en", choices=sorted(TEXT),
                        help="Language of every label in the figures")
    parser.add_argument("--only", nargs="+", choices=sorted(FIGURES),
                        help="Generate only these figures")
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)
    apply_style()
    text = TEXT[args.lang]

    data = {
        "mazes": load_csv(args.data_dir, "maze_comparison.csv"),
        "paths": load_csv(args.data_dir, "path_comparison.csv"),
        "matches": load_csv(args.data_dir, "benchmark_results.csv"),
    }

    wanted = args.only if args.only else sorted(FIGURES)
    print(f"Writing figures to {args.out_dir}/ in '{args.lang}'")
    written = 0
    for name in wanted:
        source, function = FIGURES[name]
        if data[source] is None:
            continue
        function(data[source], args.out_dir, text)
        written += 1

    print(f"Done: {written} figures")
    return 0


if __name__ == "__main__":
    sys.exit(main())