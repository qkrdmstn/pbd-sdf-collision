#ifndef __KD_TREE_H__
#define __KD_TREE_H__

#pragma once
#include "Mesh.h"	
#include <iostream>

using namespace std;

class KdTreeBoundary
{
public:
	vec3	_size;
	vec3	_pos; //Boundary의 pos는 왼쪽 아래 지점
public:
	KdTreeBoundary(void)
	{
	}
	KdTreeBoundary(vec3& pos, vec3& size) : _pos(pos), _size(size)
	{
	}
	~KdTreeBoundary(void)
	{
	}
public:
	inline double getX(void) { return _pos.x(); }
	inline double getY(void) { return _pos.y(); }
	inline double getZ(void) { return _pos.z(); }
	inline double getWidth(void) { return _size.x(); }
	inline double getHeight(void) { return _size.y(); }
	inline double getDepth(void) { return _size.z(); }
	bool isIntersect(KdTreeBoundary& box) //현재 노드의 바운더리와 전달받은 바운더리가 겹치는지 확인
	{
		vec3 v1 = box._pos, v2 = box._pos + box._size;
		vec3 v3 = _pos, v4 = _pos + _size;
		return ((v4.x() >= v1.x()) && (v3.x() <= v2.x()) &&	// x-axis overlap
			(v4.y() >= v1.y()) && (v3.y() <= v2.y()) &&		// y-axis overlap
			(v4.z() >= v1.z()) && (v3.z() <= v2.z()));		// z-axis overlap
	}
	bool isContains(vec3& v) //전달받은 점이 이 바운더리에 포함되는지 확인
	{
		vec3 v1 = _pos, v2 = _pos + _size;
		return ((v.x() >= v1.x()) && (v.x() <= v2.x()) &&
			(v.y() >= v1.y()) && (v.y() <= v2.y()) &&
			(v.z() >= v1.z()) && (v.z() <= v2.z()));
	}
};

class KdCorner //노드
{
public:
	int				_level; // 트리 노드 level
	int				_splitAxis; //x,y,z 축인듯
	double			_splitPos; //position 아마 중간값
	vector<Face*>	_polygons; //면 넣기
	KdCorner** _children; //자식
	KdTreeBoundary	_boundary; //이 노드가 해당하는 바운더리
public:
	KdCorner(void) //생성자, 초기화
	{
		_level = -1;
		_splitAxis = -1;
		_splitPos = 0.0;
		_children = new KdCorner * [2];
		for (int i = 0; i < 2; i++) {
			_children[i] = nullptr;
		}
	}
	~KdCorner(void)
	{
	}
public:
	void clearAll(void) //지우기
	{
		for (int i = 0; i < 2; i++) {
			if (_children[i] != NULL) {
				delete _children[i];
			}
		}
		delete[] _children;
		_polygons.clear();
	}
	void draw(void) //바운더리 선으로 그리기 (모든 경우의 수)
	{
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		vec3 min = _boundary._pos;
		vec3 max = _boundary._pos + _boundary._size;
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)max.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)max.y(), (GLfloat)max.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)min.z());
		glVertex3f((GLfloat)min.x(), (GLfloat)min.y(), (GLfloat)max.z());
		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
};

class KdTree
{
public:
	int					_level; //트리의 총 _level
	double* _sortBuffer; //정렬할 수 있는 버퍼인듯 축 별로
	Mesh* _mesh; //메시
	KdCorner* _root; //루트노드
	vector<KdCorner*>	_queue; //큐?
	vector<KdCorner*>	_render; //얜 머지..
	vector<double>		_minPrimitve; //primitive는 멀까
	vector<double>		_maxPrimitve;
public:
	KdTree(void);
	KdTree(Mesh* mesh);
	~KdTree(void);
public:
	void				clearUp(KdCorner* node);
	void				clear(void);
	bool				intersect(Face* face, KdTreeBoundary& box);
	bool				isOverlapPlaneBox(vec3& normal, vec3& pos, vec3& maxBox);
	bool				isIntersectNode(KdCorner* node, vec3& pos, double& squaredRange);
public:
	void				computeTree(void);
	void				computeTree(vec3 minPos, vec3 maxPos);
	void				subdivide(KdCorner* traversalNode);
	double				getDistFromTriangle(vec3& node);
	double				getDistFromTriangle_MT(vec3& node);
	double				getDistFromTriangle(vec3& node, Face* face);
public:
	void				draw(void);
};

#endif