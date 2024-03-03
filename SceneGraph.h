#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#pragma once
#include "PBD_ClothObject.h"
#include "Mesh.h"
#include "KD_Tree.h"

class SceneGraph
{
public:
	PBD_ClothObject* _pbdObj;
	Mesh* _mesh;
	KdTree* _tree;

public:
	int            _res;
	double            _gridLength;
	vector<double> _gridDistance;
	vector<vec3>   _gridPosition;
	Vec3<double>	mesh_center;
	Vec3<double>	pbd_center;

public:
	SceneGraph();
	~SceneGraph();

public:
	void simulation(double dt);
	void reset(int n);
	void applyWind(vec3 wind);
	void drawWire(void);
	void drawSolid(void);
	void drawPoint(void);

public:
	void buildRegularSDF(void);
	void drawSDFPoint(void);

public:

};
#endif