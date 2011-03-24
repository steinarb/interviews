#include "Plotter.h"
#include <InterViews/2.6/_enter.h>

Plotter::Plotter(
    const char* name, Graphic* gr
) : Plotter_core(name, gr) {}

void Plotter::Reinit () {
    Init();
}
