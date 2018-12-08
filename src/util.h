#pragma once

namespace sk
{
struct point
{
    point() = default;
    point(int _x, int _y) : x(_x), y(_y) {}
    int x = 0;
    int y = 0;
};

struct line
{
    line() = default;
    line(int x0, int y0, int x1, int y1) : start(x0, y0), end(x1, y1) {}
    point start;
    point end;
};
}
