#include "celero/Celero.h"

CELERO_MAIN

BASELINE(DemoSimple, Baseline, 10, 1000000)
{
    celero::DoNotOptimizeAway(1);
}
