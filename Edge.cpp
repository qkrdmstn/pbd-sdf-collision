#include "Edge.h"

Edge::Edge(void)
{
}

Edge::Edge(Vertex* v0, Vertex* v1, int idx)
{
	_surface = false;
	_idx = idx;
	_vertices.push_back(v0);
	_vertices.push_back(v1);
}

Edge::~Edge(void)
{
}

bool Edge::hasVertex(Vertex* vertex)
{
	for (auto v : _vertices) {
		if (v == vertex) {
			return 1;
		}
	}
	return 0;
}

bool Edge::hasFace(Face* face)
{
	for (auto f : _nbFaces) {
		if (f == face) {
			return 1;
		}
	}
	return 0;
}

void Edge::removeAdjacency(void)
{
	_nbFaces.resize(0);
}



