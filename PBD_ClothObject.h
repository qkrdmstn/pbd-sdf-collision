#ifndef __PBD_CLOTH_H__
#define __PBD_CLOTH_H__


#pragma once
#include "Face.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <math.h>

using namespace std;

class PBD_ClothObject
{
public:
    Vec3<double>   _minBoundary;
    Vec3<double>   _maxBoundary;
    Vec3<double>   _initCenter;
    vector<Vertex*> _vertices;
    vector<Edge*> _edges;
    vector<Face*> _faces;
    vec3 cp;

public:
    int            _subStep = 5;
    double         _iteration = 5.0;
    double         _springK = 0.6;
    vector<double>   _structuralLength;
    vector<double>   _bendLength;

public:
    Vec3<double>   _objectCenter;
    vector<double> _gridDistance;
    vector<vec3>   _gridPosition;
    int            _res;
    double            _gridLength;

public:
    PBD_ClothObject(char* filename, vec3 center)
    {
        _initCenter = center;
        loadObj(filename);
    }

    PBD_ClothObject(char* filename, vec3 center, vec3 Ocenter, vector<double>& grid, vector<vec3>& gridpos, int res, double length)
    {
        _initCenter = center;
        _objectCenter = Ocenter;
        _gridDistance = grid;
        _gridPosition = gridpos;

        _res = res;
        _gridLength = length;

        loadObj(filename);
    }
    ~PBD_ClothObject();

public:
    void    reset(void);
    void    loadObj(char* filename);
    void    moveToCenter(double scale);
    void    buildAdjacency(void);
    void    integrate(double dt);
    void    simulation(double dt);
    void    computeRestLength(void);
    void    computeNormal(void);
    void    updateBendSprings(void);
    void    updateStructuralSprings(void);
    void    applyWind(vec3 wind);
    void    computeWindForTriangle(vec3 wind, Face* f);
    void    applyExtForces(double dt);
    void    solveDistanceConstraint(int index0, int index1, double restLength);

public:
    double SDFCalculate1(vec3 p);
    double SDFCalculate1(double x, double y, double z);
    double SDFCalculate2(vec3 p);
    double SDFCalculate2(double x, double y, double z);
    double lerp(double a, double b, double t);
    vec3    barycentricCoord(vec3 pos0, vec3 pos1, vec3 pos2, vec3 p, double& w, double& u, double& v);
    void    SdfCollision(double dt);
    void    LevelSetCollision(void);
    void    HistoryBasedCollision(void);
    void    buildRegularSDF(void);

    double  interpolationD(vec3 p0, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 p5, vec3 p6, vec3 p7, vec3 p,
        double v0, double v1, double v2, double v3, double v4, double v5, double v6, double v7);

public:
    void    drawWire(void);
    void    drawSolid(void);
    void    drawPoint(void);
    void    drawSDFPoint(void);
    void    drawCollisionPoint(void);
};


#endif