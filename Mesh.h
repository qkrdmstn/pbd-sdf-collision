#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Vec3.h"
#include "Face.h"
#include "GL\glut.h"

class Mesh
{
public:
	Vec3<double>	_minBoundary;
	Vec3<double>	_maxBoundary;
	Vec3<double>	_initCenter;
	vector<Vertex*>	_vertices;
	vector<Edge*>	_edges;
	vector<Face*>	_faces;

public:
	Mesh();
	Mesh(char *filename, vec3 center)
	{
		_initCenter = center;
		loadObj(filename);
	}
	~Mesh();
public:
	void	loadObj(char *filename);
	void	moveToCenter(double scale);
	void	computeNormal(void);
	void	buildAdjacency(void);
public:
	void	drawWire(void);
	void	drawSolid(void);
	void	drawPoint(void);
};

#endif
