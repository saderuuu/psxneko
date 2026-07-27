#ifndef UTILS_H
#define UTILS_H

#define FX_SHIFT 12
#define FX_ONE   (1L << FX_SHIFT)

#define INDEX_2D_TO_1D(row, col, cols) ((row) * (cols) + (col))

#define INDEX_1D_TO_ROW(idx, cols) ((idx) / (cols))
#define INDEX_1D_TO_COL(idx, cols) ((idx) % (cols))

#endif