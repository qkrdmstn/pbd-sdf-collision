#ifndef __FACE_H__
#define __FACE_H__

#pragma once
#include "Vertex.h"
#include "Edge.h"
#include <vector>

using namespace std;

class Face
{
public:
    int            _index;
    Vec3<double>   _normal;
    vector<Edge*>    _edges;    // Triangle : num. edge -> 3      0: 1-2, 1:2-0, 2:0-1
    vector<Vertex*>  _vertices; // Triangle : num. vertex -> 3

public:
    Face();
    Face(int index, Vertex* v0, Vertex* v1, Vertex* v2)
    {
        _index = index;
        _vertices.push_back(v0);
        _vertices.push_back(v1);
        _vertices.push_back(v2);

    }
    ~Face();
public:
    int      getIndex(Vertex* v);
    bool     hasEdge(Edge* e);
    bool     hasEdge(Vertex* v0, Vertex* v1);
    bool     hasVertex(Vertex* v);
};

#endif