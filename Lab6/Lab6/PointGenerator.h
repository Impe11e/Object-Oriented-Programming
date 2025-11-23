#pragma once

#include <vector>
#include "Point.h"
#include <random>
#include <algorithm>

static std::vector<PointI> GeneratePoints(int n, int xMin, int xMax, int yMin, int yMax)
{
    std::vector<PointI> pts;
    if (n <= 0) return pts;
    if (xMin > xMax) std::swap(xMin, xMax);
    if (yMin > yMax) std::swap(yMin, yMax);
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> dx(xMin, xMax);
    std::uniform_int_distribution<int> dy(yMin, yMax);
    for (int i = 0; i < n; ++i) {
        pts.push_back({ dx(eng), dy(eng) });
    }
    return pts;
}

// Generate up to capacity unique integer pairs (x,y). If n > capacity, caller should cap n.
static std::vector<PointI> GenerateUniquePoints(int n, int xMin, int xMax, int yMin, int yMax)
{
    std::vector<PointI> pts;
    if (n <= 0) return pts;
    if (xMin > xMax) std::swap(xMin, xMax);
    if (yMin > yMax) std::swap(yMin, yMax);
    long long nx = (long long)xMax - (long long)xMin + 1;
    long long ny = (long long)yMax - (long long)yMin + 1;
    long long capacity = nx * ny;
    if (capacity <= 0) return pts;
    if ((long long)n > capacity) n = (int)capacity;
    // build array of all possible pairs
    pts.reserve((size_t)capacity);
    for (int x = xMin; x <= xMax; ++x) {
        for (int y = yMin; y <= yMax; ++y) {
            pts.push_back({x,y});
        }
    }
    // shuffle and take first n
    std::random_device rd;
    std::mt19937 eng(rd());
    std::shuffle(pts.begin(), pts.end(), eng);
    pts.resize(n);
    return pts;
}

// Generate n points with unique x values. If n > number of distinct x values, caller should cap n.
static std::vector<PointI> GeneratePointsUniqueX(int n, int xMin, int xMax, int yMin, int yMax)
{
    std::vector<PointI> pts;
    if (n <= 0) return pts;
    if (xMin > xMax) std::swap(xMin, xMax);
    if (yMin > yMax) std::swap(yMin, yMax);
    int nx = xMax - xMin + 1;
    if (nx <= 0) return pts;
    if (n > nx) n = nx;
    // create list of x values
    std::vector<int> xs;
    xs.reserve(nx);
    for (int x = xMin; x <= xMax; ++x) xs.push_back(x);
    std::random_device rd;
    std::mt19937 eng(rd());
    std::shuffle(xs.begin(), xs.end(), eng);
    std::uniform_int_distribution<int> dy(yMin, yMax);
    for (int i = 0; i < n; ++i) {
        int x = xs[i];
        int y = dy(eng);
        pts.push_back({x,y});
    }
    // sort by x ascending so graph will be drawn correctly
    std::sort(pts.begin(), pts.end(), [](const PointI&a,const PointI&b){ return a.x < b.x; });
    return pts;
}

// Generate n points with unique x and unique y (one-to-one). If ranges do not allow, caller should adjust n.
static std::vector<PointI> GeneratePointsBijective(int n, int xMin, int xMax, int yMin, int yMax)
{
    std::vector<PointI> pts;
    if (n <= 0) return pts;
    if (xMin > xMax) std::swap(xMin, xMax);
    if (yMin > yMax) std::swap(yMin, yMax);
    int nx = xMax - xMin + 1;
    int ny = yMax - yMin + 1;
    if (nx <= 0 || ny <= 0) return pts;
    if (n > nx) n = nx;
    if (n > ny) n = ny;
    // sample n distinct x
    std::vector<int> xs;
    xs.reserve(nx);
    for (int x = xMin; x <= xMax; ++x) xs.push_back(x);
    std::random_device rd;
    std::mt19937 eng(rd());
    std::shuffle(xs.begin(), xs.end(), eng);
    xs.resize(n);
    // sample n distinct y
    std::vector<int> ys;
    ys.reserve(ny);
    for (int y = yMin; y <= yMax; ++y) ys.push_back(y);
    std::shuffle(ys.begin(), ys.end(), eng);
    ys.resize(n);
    // pair xs[i] with ys[i] and then sort by x
    for (int i = 0; i < n; ++i) pts.push_back({xs[i], ys[i]});
    std::sort(pts.begin(), pts.end(), [](const PointI&a,const PointI&b){ return a.x < b.x; });
    return pts;
}
