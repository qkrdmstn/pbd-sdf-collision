#ifndef __PHYS_POLYGON_EDGE_H__
#define __PHYS_POLYGON_EDGE_H__

#pragma once
#include "Vertex.h"

class Edge
{
public:
	int						_idx;
	bool					_surface;
	vec3					_normal;
	vector<Vertex*>			_vertices;
	vector<Face*>			_nbFaces;
public:
	Edge(void);
	Edge(Vertex* v0, Vertex* v1, int idx);
	~Edge(void);
public:
	inline Vertex* v(int idx) { return _vertices[idx]; }
	inline double	getLength(void) { return (_vertices[0]->_pos - _vertices[1]->_pos).length(); }
public:
	bool	hasFace(Face* face);
	bool	hasVertex(Vertex* vertex);
	void	removeAdjacency(void);
};

#endif