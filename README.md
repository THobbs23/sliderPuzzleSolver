# sliderPuzzleSolver
AI program that solves 8 tiled slider puzzles

This program solves 8 tiled slider puzzles using one of four chosen algorithms: depth first, breadth first, A* using the Hamming Distance heuristic, and A* using the Manhattan Distance heuristic.

The user can choose to solve an initialized known good configuration, a randomized configuration, or load in their own. If unsolvable, this is reported to the user on the screen and in the .csv file that is used to compile search results. If solvable, the sequence of tiles to be swapped with the empty space is reported on the screen and written out to the .csv file.

Included in the repository is a large results.csv, which I used to extract some successful searches to compare some averages in smallResultsComparison.xlsx. These averages are the depth at which a solution is found, total nodes expanded, and time to solve.
