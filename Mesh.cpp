#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::loadObj(char *filename)
{
	printf("------------------------------------------- Object -------------------------------------------\n");
	FILE *fp;
	char header[256] = { 0 };
	double pos[3];
	int v_index[3]; 
	int index = 0;

	_minBoundary.set(100000.0);
	_maxBoundary.set(-100000.0);

	fopen_s(&fp, filename, "r");
	while (fscanf(fp, "%s %lf %lf %lf", header, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (header[0] == 'v' && header[1] == NULL) {
			if (_minBoundary[0] > pos[0])	_minBoundary[0] = pos[0];
			if (_minBoundary[1] > pos[1])	_minBoundary[1] = pos[1];
			if (_minBoundary[2] > pos[2])	_minBoundary[2] = pos[2];
			if (_maxBoundary[0] < pos[0])	_maxBoundary[0] = pos[0];
			if (_maxBoundary[1] < pos[1])	_maxBoundary[1] = pos[1];
			if (_maxBoundary[2] < pos[2])	_maxBoundary[2] = pos[2];
			_vertices.push_back(new Vertex(index++, Vec3<double>(pos[0], pos[1], pos[2])));
		}
	}
	printf("num. vertices : %d\n", _vertices.size());
	printf("min boundary : %f, %f, %f\n", _minBoundary.x(), _minBoundary.y(), _minBoundary.z());
	printf("max boundary : %f, %f, %f\n", _maxBoundary.x(), _maxBoundary.y(), _maxBoundary.z());
		
	index = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d %d %d", header, &v_index[0], &v_index[1], &v_index[2]) != EOF) {
		if (header[0] == 'f' && header[1] == NULL) {
			auto v0 = _vertices[v_index[0] - 1];
			auto v1 = _vertices[v_index[1] - 1];
			auto v2 = _vertices[v_index[2] - 1];
			_faces.push_back(new Face(index++, v0, v1, v2));
		}
	}

	printf("num. faces : %d\n", _faces.size());
	moveToCenter(0.6);
	buildAdjacency();
	computeNormal();
	fclose(fp);
	printf("----------------------------------------------------------------------------------------------\n");
}

void Mesh::buildAdjacency(void)
{
	for (auto v : _vertices) {
		v->_nbFaces.clear();
		v->_nbVertices.clear();
	}

	// v-f
	for (auto f : _faces) {
		for (int j = 0; j < 3; j++) {
			f->_vertices[j]->_nbFaces.push_back(f);
		}
	}

	// v-v
	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			auto pivot = nf->getIndex(v); // 0 : 1,2, 1 : 2,0, 2: 0,1
			int other_id0 = (pivot + 1) % 3;
			int other_id1 = (pivot + 2) % 3;
			if (!v->hasNbVertex(nf->_vertices[other_id0])) {
				v->_nbVertices.push_back(nf->_vertices[other_id0]);
			}
			if (!v->hasNbVertex(nf->_vertices[other_id1])) {
				v->_nbVertices.push_back(nf->_vertices[other_id1]);
			}
		}
	}

	//edge build
	int index = 0;
	int number_vertex = (int)_vertices.size();

	for (auto v : _vertices) {
		for (auto nv : v->_nbVertices) {
			if (!nv->_flag) {
				_edges.push_back(new Edge(v, nv, index++));
			}
		}
		v->_flag = true;
	}

	for (auto v : _vertices) {
		v->_flag = false;
	}

	for (auto e : _edges)
	{
		for (auto nf : e->v(0)->_nbFaces) {
			if (nf->hasVertex(e->v(0)) && nf->hasVertex(e->v(1))) {
				if (!nf->hasEdge(e)) {
					nf->_edges.push_back(e);
				}
				if (!e->hasFace(nf)) {
					e->_nbFaces.push_back(nf);
				}
			}
		}
	}
	printf("num. edges : %d\n", _edges.size());
	printf("build adjacency list\n");
}

void Mesh::computeNormal(void)
{
	// f-normal
	for (auto f : _faces) {
		auto a = f->_vertices[0]->_pos;
		auto b = f->_vertices[1]->_pos;
		auto c = f->_vertices[2]->_pos;
		auto normal = (a - b).cross(a - c);
		normal.normalize();
		f->_normal = normal;
	}

	//e-normal
	for (auto e : _edges)
	{
		e->_normal.clear();
		for (auto nf : e->_nbFaces)
		{
			e->_normal += nf->_normal;
		}
		e->_normal /= e->_nbFaces.size();
		e->_normal.normalize();
	}

	// v-normal
	for (auto v : _vertices) {
		v->_normal.clear();
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= v->_nbFaces.size();
		v->_normal.normalize();
	}
	printf("compute normal\n");
}

void Mesh::moveToCenter(double scale)
{
	double max_length = fmax(fmax(_maxBoundary[0] - _minBoundary[0], _maxBoundary[1] - _minBoundary[1]), _maxBoundary[2] - _minBoundary[2]);
	auto center = (_maxBoundary + _minBoundary) / 2.0;

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto grad = pos - center;
		grad /= max_length;
		grad *= scale;
		pos = _initCenter;
		pos += grad;
		v->_pos = pos;
	}
	printf("move to center\n");
}

void Mesh::drawPoint(void)
{
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPointSize(1.0f);
	glEnable(GL_LIGHTING);
}

void Mesh::drawSolid(void)
{
	//glColor3f(0, 0, 1); //100
	glEnable(GL_LIGHTING);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		for (int j = 0; j < 3; j++) {
			glVertex3f(f->_vertices[j]->x(), f->_vertices[j]->y(), f->_vertices[j]->z());
		}
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

void Mesh::drawWire(void)
{
	glDisable(GL_LIGHTING);

	//glColor3f(0, 0, 0); //edge È®ÀÎ¿ë
	//glLineWidth(1);
	//for (auto f : _faces) {
	//	glBegin(GL_LINES);
	//	for (int j = 0; j < 3; j++) {
	//		int s = j % 3; // 0,1,2
	//		int e = (j + 1) % 3; // 1,2,0
	//		glVertex3f(f->_vertices[s]->x(), f->_vertices[s]->y(), f->_vertices[s]->z());
	//		glVertex3f(f->_vertices[e]->x(), f->_vertices[e]->y(), f->_vertices[e]->z());
	//	}
	//	glEnd();
	//}

	glColor3f(1, 1, 1); //100
	glLineWidth(1); //5
	for (auto e : _edges) {
		glBegin(GL_LINES);
		glVertex3f(e->_vertices[0]->x(), e->_vertices[0]->y(), e->_vertices[0]->z());
		glVertex3f(e->_vertices[1]->x(), e->_vertices[1]->y(), e->_vertices[1]->z());
		glEnd();
	}
	glEnable(GL_LIGHTING);
}