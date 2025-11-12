#ifndef __MARCHINGCUBES_H__
#define __MARCHINGCUBES_H__

#include "cube.h"

namespace game
{
    struct GridCell
    {
        vec position[8];
        double value[8];
    };

    struct Triangle
    {
        vec vertices[3];
        vec normal;
    };

    class MarchingCubes
    {
    private:
        static const int edgeTable[256];
        static const int triTable[256][16];

        static vec interpolate(double isolevel, const vec &p1, const vec &p2, double v1, double v2);

    public:
        static void polygonize(const GridCell &cell, double isolevel, vector<Triangle> &triangles);
        static vec calculateNormal(const vec &v0, const vec &v1, const vec &v2);
    };
}

#endif
