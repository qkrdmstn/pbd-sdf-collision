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
	vec3	_pos; //Boundary�� pos�� ���� �Ʒ� ����
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
	bool isIntersect(KdTreeBoundary& box) //���� ����� �ٿ������ ���޹��� �ٿ������ ��ġ���� Ȯ��
	{
		vec3 v1 = box._pos, v2 = box._pos + box._size;
		vec3 v3 = _pos, v4 = _pos + _size;
		return ((v4.x() >= v1.x()) && (v3.x() <= v2.x()) &&	// x-axis overlap
			(v4.y() >= v1.y()) && (v3.y() <= v2.y()) &&		// y-axis overlap
			(v4.z() >= v1.z()) && (v3.z() <= v2.z()));		// z-axis overlap
	}
	bool isContains(vec3& v) //���޹��� ���� �� �ٿ������ ���ԵǴ��� Ȯ��
	{
		vec3 v1 = _pos, v2 = _pos + _size;
		return ((v.x() >= v1.x()) && (v.x() <= v2.x()) &&
			(v.y() >= v1.y()) && (v.y() <= v2.y()) &&
			(v.z() >= v1.z()) && (v.z() <= v2.z()));
	}
};

class KdCorner //���
{
public:
	int				_level; // Ʈ�� ��� level
	int				_splitAxis; //x,y,z ���ε�
	double			_splitPos; //position �Ƹ� �߰���
	vector<Face*>	_polygons; //�� �ֱ�
	KdCorner** _children; //�ڽ�
	KdTreeBoundary	_boundary; //�� ��尡 �ش��ϴ� �ٿ����
public:
	KdCorner(void) //������, �ʱ�ȭ
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
	void clearAll(void) //�����
	{
		for (int i = 0; i < 2; i++) {
			if (_children[i] != NULL) {
				delete _children[i];
			}
		}
		delete[] _children;
		_polygons.clear();
	}
	void draw(void) //�ٿ���� ������ �׸��� (��� ����� ��)
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
	int					_level; //Ʈ���� �� _level
	double* _sortBuffer; //������ �� �ִ� �����ε� �� ����
	Mesh* _mesh; //�޽�
	KdCorner* _root; //��Ʈ���
	vector<KdCorner*>	_queue; //ť?
	vector<KdCorner*>	_render; //�� ����..
	vector<double>		_minPrimitve; //primitive�� �ֱ�
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