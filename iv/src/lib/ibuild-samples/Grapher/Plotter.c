#include "Plotter.h"
#include <IV-2_6/_enter.h>

Plotter::Plotter(
    const char* name, Graphic* gr, Coord pad, Alignment a
) : Plotter_core(name, gr, pad, a) {}

void Plotter::Reinit () {
    Init();
}
