#include "Vertex.h"


Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

bool Vertex::hasNbVertex(Vertex* v)
{
    for (auto nv : _nbVertices) {
        if (nv == v) {
            return true;
        }
    }
    return false;
}

bool Vertex::hasNbFace(Face* f)
{
    for (auto nf : _nbFaces) {
        if (nf == f) {
            return true;
        }
    }
    return false;
}