"""Shared style for the figures of the Maze Craze experimental report.

Defines the palette, the matplotlib settings and a few helpers used by the
plotting scripts. The categorical palette has three slots (blue, orange,
aqua) and is colorblind safe: the three are distinguishable under
deuteranopia, protanopia and tritanopia. Each series also carries its own
marker and line style, so the figures stay readable when printed in black
and white.
"""

import matplotlib
matplotlib.use("Agg") 
import matplotlib.pyplot as plt



SURFACE = "#fcfcfb"   # Figure background
INK = "#0b0b0b"       # Titles
INK_SOFT = "#52514e"  # Axis labels and secondary text
MUTED = "#898781"     # Tick marks
GRID = "#e1e0d9"      # Grid lines
AXIS = "#c3c2b7"      # Axes

SERIES = {
    "BFS": "#2a78d6",       # blue
    "Dijkstra": "#eb6834",  # orange
    "A*": "#1baf7a",        # aqua
}

# The generators reuse the first two slots
GENERATOR = {
    "DFS": "#2a78d6",
    "BFS": "#eb6834",
}

# Mud rates and extra edge probabilities, in order
LEVEL_COLORS = ["#2a78d6", "#eb6834", "#1baf7a"]

# Secondary encoding: every series has its own marker and stroke
MARKERS = {"BFS": "o", "Dijkstra": "s", "A*": "^"}
LINESTYLES = {"BFS": "-", "Dijkstra": "--", "A*": "-."}


def apply_style():
    """Apply the global matplotlib settings shared by every figure."""
    plt.rcParams.update({
        "figure.facecolor": SURFACE,
        "axes.facecolor": SURFACE,
        "savefig.facecolor": SURFACE,
        "font.family": "sans-serif",
        "font.sans-serif": ["DejaVu Sans", "Arial", "Helvetica"],
        "font.size": 10,
        "axes.titlesize": 11,
        "axes.titleweight": "bold",
        "axes.titlecolor": INK,
        "axes.labelsize": 10,
        "axes.labelcolor": INK_SOFT,
        "axes.edgecolor": AXIS,
        "axes.linewidth": 0.8,
        "xtick.color": MUTED,
        "ytick.color": MUTED,
        "xtick.labelcolor": INK_SOFT,
        "ytick.labelcolor": INK_SOFT,
        "xtick.labelsize": 9,
        "ytick.labelsize": 9,
        "legend.frameon": False,
        "legend.fontsize": 9,
        "legend.labelcolor": INK_SOFT,
        "lines.linewidth": 2.0,
        "lines.markersize": 7,
        "grid.color": GRID,
        "grid.linewidth": 0.8,
        "figure.dpi": 200,
        "savefig.dpi": 200,
        "savefig.bbox": "tight",
    })


def tidy_axes(ax, grid_axis="y"):
    """Drop the top and right spines and leave a recessive grid."""
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.set_axisbelow(True)
    if grid_axis:
        ax.grid(True, axis=grid_axis)


def save(fig, path):
    """Write the figure to disk and report it on the console."""
    fig.savefig(path)
    plt.close(fig)
    print(f"  wrote {path}")