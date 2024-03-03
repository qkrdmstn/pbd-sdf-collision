#include "Face.h"


Face::Face()
{
}


Face::~Face()
{
}

int Face::getIndex(Vertex* v)
{
    for (int i = 0; i < 3; i++) {
        if (_vertices[i] == v) {
            return i;
        }
    }
    return -1;
}

bool Face::hasEdge(Vertex* v0, Vertex* v1)
{
    for (int i = 0; i < _edges.size(); i++)
    {
        if (v0 == _edges[i]->_vertices[0] && v1 == _edges[i]->_vertices[1])
        {
            return true;
        }
        else if (v0 == _edges[i]->_vertices[1] && v1 == _edges[i]->_vertices[0])
        {
            return true;
        }
    }
    return false;
}

bool Face::hasEdge(Edge* e)
{
    auto v0 = e->v(0);
    auto v1 = e->v(1);
    for (int i = 0; i < _edges.size(); i++)
    {
        if (v0 == _edges[i]->_vertices[0] && v1 == _edges[i]->_vertices[1])
        {
            return true;
        }
        else if (v0 == _edges[i]->_vertices[1] && v1 == _edges[i]->_vertices[0])
        {
            return true;
        }
    }
    return false;
}

bool Face::hasVertex(Vertex* v)
{
    for (int i = 0; i < 3; i++) {
        if (_vertices[i] == v) {
            return true;
        }
    }
    return false;
}