#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Vertex
{
public:
    int             _index;
    bool            _flag;  //kd-tree ���
    double          _invMass;
    Vec3<double>    _pos; // X,Y,Z
    Vec3<double>    _pos1; // next pos
    Vec3<double>    _normal;
    Vec3<double>    _vel;
    vector<Face*>   _nbFaces; // Neighbor face
    vector<Vertex*> _nbVertices; // Neighbor vertex

public:
    Vertex();
    Vertex(int index, Vec3<double> pos, Vec3<double> vel, double invMass) //PBD ������
    {
        _index = index;
        _pos = pos;
        _vel = vel;
        _invMass = invMass;
        _flag = false;
    }

    Vertex(int index, Vec3<double> pos) //Mesh ������
    {
        _index = index;
        _pos = pos;
        _flag = false;
    }
    ~Vertex();
public:
    inline double x(void) { return _pos.x(); }
    inline double y(void) { return _pos.y(); }
    inline double z(void) { return _pos.z(); }
public:
    bool   hasNbVertex(Vertex* v);
    bool   hasNbFace(Face* f);
};

#endif