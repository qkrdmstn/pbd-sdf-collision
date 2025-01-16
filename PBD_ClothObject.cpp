#define _CRT_SECURE_NO_WARNINGS
#define FRANK_WOLFE 0
#define PROJECTED_GRADIENT_DESCENT 1

//sdf calculate
#define SPHERE 
//#define PLANE
//#define BOX

#define SDFCalculate SDFCalculate1 //sdf
//#define SDFCalculate SDFCalculate2

#include "PBD_ClothObject.h"
#include "GL\glut.h"
#include <string>

PBD_ClothObject::~PBD_ClothObject()
{
	for (auto v : _vertices)
	{
		v->~Vertex();
	}
	for (auto f : _faces)
	{
		f->~Face();
	}
	if (_vertices.size() != 0)
		_vertices.clear();
	if (_faces.size() != 0)
		_faces.clear();
}

void PBD_ClothObject::reset(void)
{

}

void PBD_ClothObject::loadObj(char* filename)
{
	printf("----------------------------------------- PBD Object -----------------------------------------\n");
	FILE* fp;
	char header[256] = { 0 };
	double pos[3];
	int v_index[3];
	int index = 0;

	_minBoundary.set(100000.0);
	_maxBoundary.set(-100000.0);

	fopen_s(&fp, filename, "r");
	while (fscanf(fp, "%s %lf %lf %lf", header, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (header[0] == 'v' && header[1] == NULL) {
			if (_minBoundary[0] > pos[0])   _minBoundary[0] = pos[0];
			if (_minBoundary[1] > pos[1])   _minBoundary[1] = pos[1];
			if (_minBoundary[2] > pos[2])   _minBoundary[2] = pos[2];
			if (_maxBoundary[0] < pos[0])   _maxBoundary[0] = pos[0];
			if (_maxBoundary[1] < pos[1])   _maxBoundary[1] = pos[1];
			if (_maxBoundary[2] < pos[2])   _maxBoundary[2] = pos[2];
			_vertices.push_back(new Vertex(index++, Vec3<double>(pos[0], pos[1], pos[2]), Vec3<double>(0.0, 0.0, 0.0), 1.0));
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
	moveToCenter(1);
	buildAdjacency();
	computeRestLength();
	computeNormal();

	buildRegularSDF();
	fclose(fp);
	printf("----------------------------------------------------------------------------------------------\n\n");
}

void PBD_ClothObject::buildAdjacency(void)
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

void PBD_ClothObject::computeNormal(void)
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

	for (auto e : _edges)
	{
		e->_normal.clear();
		for (auto nf : e->_nbFaces)
		{
			e->_normal += nf->_normal;
		}
		e->_normal /= 2;
	}

	// v-normal
	for (auto v : _vertices) {
		v->_normal.clear();
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= v->_nbFaces.size();
	}
	printf("compute normal\n");
}

void PBD_ClothObject::moveToCenter(double scale)
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

void PBD_ClothObject::computeRestLength(void)
{
	//for (auto f : _faces) {
	//	//init structural Length
	//	for (int i = 0; i < 3; i++) {
	//		f->_structuralLength.push_back((f->_vertices[i]->_pos - f->_vertices[(i + 1) % 3]->_pos).length());
	//	}

	//	//init bend Length
	//	for (int i = 0; i < 3; i++) {
	//		int id0 = (i + 1) % 3;
	//		int id1 = (i + 2) % 3;
	//		for (auto nv : f->_vertices[id0]->_nbVertices) {
	//			if (f->_vertices[id1]->hasNbVertex(nv) && nv != f->_vertices[i]) {
	//				f->_bendLength.push_back((f->_vertices[i]->_pos - nv->_pos).length());
	//				break;
	//			}
	//		}
	//	}
	//}

	for (auto e : _edges){
		//init structural Length
		_structuralLength.push_back((e->v(0)->_pos - e->v(1)->_pos).length());

		//init bend Length
		if (e->_nbFaces.size() == 2){
			Vertex* v[2];
			int index = 0;
			for (auto nf : e->_nbFaces){
				for (int i = 0; i < 3; i++){
					if (!e->hasVertex(nf->_vertices[i])){
						v[index] = nf->_vertices[i];
						index++;
						break;
					}
				}
			}
			_bendLength.push_back((v[0]->_pos - v[1]->_pos).length());
		}
	}
}

void PBD_ClothObject::solveDistanceConstraint(int index0, int index1, double restlength)
{
	double c_p1p2 = (_vertices[index0]->_pos1 - _vertices[index1]->_pos1).length() - restlength;
	vec3 dp1 = (_vertices[index0]->_pos1 - _vertices[index1]->_pos1);
	vec3 dp2 = (_vertices[index0]->_pos1 - _vertices[index1]->_pos1);
	dp1.normalize();
	dp2.normalize();
	dp1 *= -_vertices[index0]->_invMass / (_vertices[index0]->_invMass + _vertices[index1]->_invMass) * c_p1p2;
	dp2 *= _vertices[index1]->_invMass / (_vertices[index0]->_invMass + _vertices[index1]->_invMass) * c_p1p2;

	_vertices[index0]->_pos1 += dp1 * _springK;
	_vertices[index1]->_pos1 += dp2 * _springK;
}

void PBD_ClothObject::applyExtForces(double dt)
{
	vec3 gravity(0.0, -9.8, 0.0);
	double damping = 0.9;

	for (auto v : _vertices) {

		v->_vel += gravity * dt;
		v->_vel *= damping;
		v->_pos1 = v->_pos + (v->_vel * dt);
	}
}

void PBD_ClothObject::updateStructuralSprings(void)
{
	//for (auto f : _faces) {
	//	for (int i = 0; i < 3; i++) {
	//		int id0 = (i + 1) % 3;
	//		solveDistanceConstraint(f->_vertices[i]->_index, f->_vertices[id0]->_index, f->_structuralLength[i]);
	//	}
	//}

	for (auto e : _edges)
	{
		//init structural length
		solveDistanceConstraint(e->v(0)->_index, e->v(1)->_index, _structuralLength[e->_idx]);
	}
}

void PBD_ClothObject::updateBendSprings(void)
{
	//for (auto f : _faces) {
	//	for (int i = 0; i < 3; i++) {
	//		int id0 = (i + 1) % 3;
	//		int id1 = (i + 2) % 3;
	//		for (auto nv : f->_vertices[id0]->_nbVertices) {
	//			if (f->_vertices[id1]->hasNbVertex(nv) && nv != f->_vertices[i]) {
	//				solveDistanceConstraint(f->_vertices[i]->_index, nv->_index, f->_bendLength[i]);
	//				break;
	//			}
	//		}
	//	}
	//}

	int idx = 0;
	//init bend Length
	for (auto e : _edges)
	{
		if (e->_nbFaces.size() == 2)
		{
			Vertex* v[2];
			int index = 0;
			for (auto nf : e->_nbFaces)
			{
				for (int i = 0; i < 3; i++)
				{
					if (!e->hasVertex(nf->_vertices[i]))
					{
						v[index] = nf->_vertices[i];
						index++;
						break;
					}
				}
			}
			solveDistanceConstraint(v[0]->_index, v[1]->_index, _bendLength[idx++]);
		}
	}
}

void PBD_ClothObject::integrate(double dt)
{
	for (int i = 0; i < _vertices.size(); i++) {

		if (_vertices[i]->_pos.y() >= 0.9)
			continue;
		_vertices[i]->_vel = (_vertices[i]->_pos1 - _vertices[i]->_pos) / dt;

		_vertices[i]->_pos = _vertices[i]->_pos1;
	}
}

void PBD_ClothObject::simulation(double dt)
{
	//////Iterator
	//applyExtForces(dt);

	//for (int k = 0; k < 10; k++) {
	//	updateStructuralSprings();
	//	updateBendSprings();

	//}
	//integrate(dt);
	////SdfCollision(dt);
	//LevelSetCollision();

	////Sub-Steps
	int subTime = 10;
	double subDt = dt / subTime;
	for (int i = 0; i < subTime; i++)
	{
		applyExtForces(subDt);
		updateStructuralSprings();
		updateBendSprings();
		//printf("mag: %f \n", (_vertices[25]->_pos - _vertices[25]->_pos1).getNorm());

		//double magSum = 0;
		//for (int i = 0; i < _vertices.size(); i++)
		//{
		//	magSum += (_vertices[i]->_pos - _vertices[i]->_pos1).getNorm();
		//}
		//printf("%f\n", magSum / _vertices.size());

		integrate(subDt);
		//SdfCollision(subDt);
		//_vertices[25]->_pos.print();
		//printf("%f\n", _vertices[25]->_pos[1]);

	}


}

void PBD_ClothObject::computeWindForTriangle(vec3 wind, Face* f)
{
	auto p0 = f->_vertices[0]->_pos1;
	auto p1 = f->_vertices[1]->_pos1;
	auto p2 = f->_vertices[2]->_pos1;
	auto normal = (p1 - p0).cross(p2 - p0);
	normal.normalize();
	auto force = normal * (normal.dot(wind));
	f->_vertices[0]->_vel += force;
	f->_vertices[1]->_vel += force;
	f->_vertices[2]->_vel += force;
}

void PBD_ClothObject::applyWind(vec3 wind)
{
	for (auto f : _faces) {
		computeWindForTriangle(wind, f);
	}
}

void PBD_ClothObject::drawPoint(void)
{
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (auto v : _vertices) {
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPointSize(1.0f);
	glEnable(GL_LIGHTING);

	drawSDFPoint();
}

void PBD_ClothObject::drawSolid(void)
{
	int i = 0;
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		if(f->_index % 2)
			glColor3f(1.0f, 1.0f, 1.0f);
		else
			glColor3f(0.0f, 0.0f, 0.0f);

		glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		for (int j = 0; j < 3; j++) {
			glVertex3f(f->_vertices[j]->x(), f->_vertices[j]->y(), f->_vertices[j]->z());
		}
		glEnd();
		i++;
	}
	glEnable(GL_LIGHTING);
}

void PBD_ClothObject::drawWire(void)
{
	glDisable(GL_LIGHTING);

	//glColor3f(0, 0, 0); //edge 확인용
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

	glColor3f(1,1,1); //100
	glLineWidth(1); //5
	for (auto e : _edges) {
		glBegin(GL_LINES);
		glVertex3f(e->_vertices[0]->x(), e->_vertices[0]->y(), e->_vertices[0]->z());
		glVertex3f(e->_vertices[1]->x(), e->_vertices[1]->y(), e->_vertices[1]->z());
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

double PBD_ClothObject::lerp(double a, double b, double t)
{
	return a * fabs((1 - t)) + b * fabs(t);
}

double  PBD_ClothObject::interpolationD(vec3 p0, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 p5, vec3 p6, vec3 p7, vec3 p,
	double v0, double v1, double v2, double v3, double v4, double v5, double v6, double v7)
{
	double tX = (p.x() - p0.x()) / (p1.x() - p0.x());
	double tY = (p.y() - p0.y()) / (p4.y() - p0.y());
	double tZ = (p.z() - p0.z()) / (p2.z() - p0.z());

	double d0 = lerp(v0, v1, tX);
	double d1 = lerp(v2, v3, tX);
	double d2 = lerp(v4, v5, tX);
	double d3 = lerp(v6, v7, tX);

	double d4 = lerp(d0, d1, tY);
	double d5 = lerp(d2, d3, tY);

	double D = lerp(d4, d5, tZ);

	return D;
}

double PBD_ClothObject::SDFCalculate1(vec3 p)
{
	return SDFCalculate1(p.x(), p.y(), p.z());
}

double PBD_ClothObject::SDFCalculate1(double x, double y, double z)
{
	int xi0 = floor((x - _objectCenter.x() + ((double)_gridLength / 2)) * ((double)_res / (double)_gridLength));
	int yi0 = floor((y - _objectCenter.y() + ((double)_gridLength / 2)) * ((double)_res / (double)_gridLength));
	int zi0 = floor((z - _objectCenter.z() + ((double)_gridLength / 2)) * ((double)_res / (double)_gridLength));

	if (xi0 < 0 || xi0 + 1 > _res - 1) //그리드 범위 밖
		return 2;
	if (yi0 < 0 || yi0 + 1 > _res - 1)
		return 2;
	if (zi0 < 0 || zi0 + 1 > _res - 1)
		return 2;

	int id[8];
	vec3 pos[8];
	int i = 0;
	for (int y = 0; y <= 1; y++)
	{
		for (int z = 0; z <= 1; z++)
		{
			for (int x = 0; x <= 1; x++)
			{
				id[i] = ((xi0 + x) * _res + (yi0 + y)) * _res + (zi0 + z);
				
				auto px = _objectCenter.x() - ((double)_gridLength / 2) + (_gridLength * ((double)(xi0 + x) / (double)_res));
				auto py = _objectCenter.y() - ((double)_gridLength / 2) + (_gridLength * ((double)(yi0 + y) / (double)_res));
				auto pz = _objectCenter.z() - ((double)_gridLength / 2) + (_gridLength * ((double)(zi0 + z) / (double)_res));
				pos[i] = vec3(px, py, pz);
				i++;
			}
		}
	}

	double SDF_value = interpolationD(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], vec3(x, y, z),
		_gridDistance[id[0]], _gridDistance[id[1]], _gridDistance[id[2]], _gridDistance[id[3]], _gridDistance[id[4]],
		_gridDistance[id[5]], _gridDistance[id[6]], _gridDistance[id[7]]);

	//printf("%f\n", SDF_value);
	return SDF_value;
}

double PBD_ClothObject::SDFCalculate2(vec3 p)
{
	return SDFCalculate2(p.x(), p.y(), p.z());
}

double PBD_ClothObject::SDFCalculate2(double x, double y, double z)
{
#if defined(SPHERE)
	double x0 = 0, y0 = 0, z0 = 0; //sphere 중심 좌표
	double r = 0.7f;
	return sqrt(pow(x - x0, 2) + pow(y - y0, 2) + pow(z - z0, 2)) - r; //구 방정식
#elif defined(PLANE)
	//ax + by + cz + d = 0 평면 방정식
	double a = 0.0;
	double b = 1;
	double c = 0.0;
	double d = 0;

	return (a * x + b * y + c * z + d) / sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2));
#elif defined(BOX)
	vec3 b = vec3(1.0, 1.0, 1.0);
	double x0 =0, y0 = 0, z0 = 0; //box 중심 좌표

	vec3 p = vec3(x - x0, y - y0, z - z0);
	vec3 q = p.abs() - b;
	vec3 a = vec3(max(q.x(), 0.0), max(q.y(), 0.0), max(q.z(), 0.0));
	double d = a.length() + min(max(q.x(), max(q.y(), q.z())), 0.0);
	return d;
#endif
}

void PBD_ClothObject::SdfCollision(double dt)
{
	double deltaT = 0.01/5.0f;
	double h = 0.01f;
	double coefficientFriction = 0.1f; //마찰 계수
	for (auto face : _faces)
	{
		Vertex* v0 = face->_vertices[0]; //Triangle 꼭짓점 접근
		Vertex* v1 = face->_vertices[1];
		Vertex* v2 = face->_vertices[2];

		vec3 pos0 = v0->_pos;
		vec3 pos1 = v1->_pos;
		vec3 pos2 = v2->_pos;

		vec3 midPos = (pos0 + pos1 + pos2) / 3.0f; //삼각형의 중간점
		//midPos.print();
		double boundRadius = max(max((pos0 - midPos).length(), (pos1 - midPos).length()), (pos2 - midPos).length()); //Bounding Sphere의 반지름

		if (SDFCalculate(midPos) > boundRadius) //Bounding Sphere의 반지름보다 중심으로부터 충돌체와의 거리가 멀 경우
		{
			continue;
		}

		vec3 p = midPos; //Iterate 시작지점
		double u, v, w;
		double pi;

		for (int iterate = 0; iterate < 20; iterate++)
		{
			vec3 N(SDFCalculate(p.x() + h, p.y(), p.z()) - SDFCalculate(p.x(), p.y(), p.z()),
				SDFCalculate(p.x(), p.y() + h, p.z()) - SDFCalculate(p.x(), p.y(), p.z()),
				SDFCalculate(p.x(), p.y(), p.z() + h) - SDFCalculate(p.x(), p.y(), p.z()));
			N /= h; //법선 벡터 계산 (오일러 방법 이용) = Gradient PI
			N.normalize();
#if PROJECTED_GRADIENT_DESCENT
			p -= N; //Gradient Descent
			p = barycentricCoord(pos0, pos1, pos2, p, w, u, v); //Projection
#endif
#if FRANK_WOLFE
			float da = pos0.dot(N); //각 꼭짓점과 법선 벡터 내적
			float db = pos1.dot(N);
			float dc = pos2.dot(N);

			vec3 s; //내적 값이 가장 작은 꼭짓점을 s로 설정
			if (da < db)
			{
				if (dc < da)
					s = pos2;
				else
					s = pos0;
			}
			else
			{
				if (dc < db)
					s = pos2;
				else
					s = pos1;
			}

			double gamma = 0.3 * 2.0 / (float(iterate) + 2.0); //보간 비율 gamma
			p = p * (1 - gamma) + s * gamma; //현재 위치로부터 s 위치로 이동
#endif
			pi = SDFCalculate(p);
			if (pi < 0.1) //thickness = contact_dist
				break;
		}
		pi = SDFCalculate(p);
		if (pi >= 0.0f)
			continue;

		barycentricCoord(pos0, pos1, pos2, p, w, u, v); //최단점
		cp = p;

		//////sub-step 방식 개선 전/후, solve contraints
		double a0 = w;
		double a1 = u;
		double a2 = v;

		double c = pi;
		double scale = c / (pow(a0, 2) + pow(a1, 2) + pow(a2, 2)); //정점의 무게가 다 같다고 가정

		vec3 n(SDFCalculate(p.x() + h, p.y(), p.z()) - SDFCalculate(p.x(), p.y(), p.z()),
			SDFCalculate(p.x(), p.y() + h, p.z()) - SDFCalculate(p.x(), p.y(), p.z()),
			SDFCalculate(p.x(), p.y(), p.z() + h) - SDFCalculate(p.x(), p.y(), p.z()));
		n /= h; //법선 벡터 계산 (오일러 방법 이용) = Gradient PI
		n.normalize();

		vec3 gP0 = n * a0;
		vec3 gP1 = n * a1;
		vec3 gP2 = n * a2;

		vec3 dP0 = gP0 * scale * (-1); //각 꼭짓점이 이동해야할 거리
		vec3 dP1 = gP1 * scale * (-1);
		vec3 dP2 = gP2 * scale * (-1);
		
		////////////개선 전
		//v0->_vel += dP0 / dt; //최종 속도 업데이트
		//v1->_vel += dP1 / dt; //최종 속도 업데이트
		//v2->_vel += dP2 / dt; //최종 속도 업데이트

		//v0->_pos1 += dP0; //최종 위치 업데이트
		//v1->_pos1 += dP1; //최종 위치 업데이트
		//v2->_pos1 += dP2; //최종 위치 업데이트

		////개선 후
		vec3 vN0 = dP0 / dt;
		vec3 vN1 = dP1 / dt;
		vec3 vN2 = dP2 / dt;

		vec3 vT0 = v0->_vel - n * v0->_vel.dot(n);
		vec3 vT1 = v1->_vel - n * v1->_vel.dot(n);
		vec3 vT2 = v2->_vel - n * v2->_vel.dot(n);

		double damping = 1;
		v0->_vel = vN0 + vT0; //최종 속도 업데이트
		v1->_vel = vN1 + vT1; //최종 속도 업데이트
		v2->_vel = vN2 + vT2; //최종 속도 업데이트
	}
}

void PBD_ClothObject::LevelSetCollision(void)
{
	double deltaT = 0.01f;
	double h = 0.1f;
	double coefficientFriction = 0.1f; //마찰 계수

	for (auto v : _vertices) {
		double x = v->_pos.x();
		double y = v->_pos.y();
		double z = v->_pos.z();

		vec3 N(SDFCalculate(x + h, y, z) - SDFCalculate(x, y, z),
			SDFCalculate(x, y + h, z) - SDFCalculate(x, y, z),
			SDFCalculate(x, y, z + h) - SDFCalculate(x, y, z));
		//N.print();

		N /= h; //법선 벡터 계산 (오일러 방법 이용) = Gradient PI
		N.normalize();

		double pi = SDFCalculate(x, y, z); //PI, newPI 계산
		double newPI = pi + (v->_vel * deltaT).dot(N);

		if (newPI < 0)
		{
			double vpN = v->_vel.dot(N); //원래의 법선 방향 속력
			vec3 vpNN = N * vpN; //원래의 법선 방향 속도
			vec3 vpT = v->_vel - vpNN; //원래의 접선 방향 속도

			double newVpN = vpN - (newPI / deltaT); //새로운 법선 방향 속력
			vec3 newVpNN = N * newVpN; // 새로운 법선 방향 속도

			double friction = (coefficientFriction * (newVpN - vpN) / vpT.getNorm());
			vec3 newVpT = vpT * (1 - friction);

			if (1 - friction < 0)
				newVpT.set(0, 0, 0);

			v->_vel = newVpNN + newVpT; //속도 업데이트
		}
	}
}


void PBD_ClothObject::HistoryBasedCollision(void)
{
	double deltaT = 0.01f;
	double h = 0.001f;
	double coefficientFriction = 0.2f; //마찰 계수

	for (auto v : _vertices) 
	{
		double x = v->_pos.x();
		double y = v->_pos.y();
		double z = v->_pos.z();

		vec3 N(SDFCalculate(x + h, y, z) - SDFCalculate(x, y, z),
			SDFCalculate(x, y + h, z) - SDFCalculate(x, y, z),
			SDFCalculate(x, y, z + h) - SDFCalculate(x, y, z));
		N /= h; //법선 벡터 계산 (오일러 방법 이용) = Gradient PI
		N.normalize();

		double pi = SDFCalculate(x, y, z); //PI, newPI 계산
		vec3 newPos = v->_pos + (v->_vel * deltaT);
		double newPI = SDFCalculate(newPos);

		if (newPI < 0)
		{
			double weight = SDFCalculate(v->_pos) / (SDFCalculate(v->_pos) - SDFCalculate(newPos));

			vec3 beforeV = v->_vel * weight; //현재 위치 ~ 접촉면까지의 속도 (v~n+1/2)
			vec3 xc = v->_pos + beforeV * deltaT;
			v->_vel = beforeV; //업데이트
			v->_pos = xc;

			// V star n+1/2 계산
			double vpN = v->_vel.dot(N); //원래의 법선 방향 속력
			vec3 vpNN = N * vpN; //원래의 법선 방향 속도
			vec3 vpT = v->_vel - vpNN; //원래의 접선 방향 속도

			double newVpN = max(vpN, 0.0); //새로운 법선 방향 속력 (평면 속도 = 0 정지)
			vec3 newVpNN = N * newVpN; // 새로운 법선 방향 속도

			double friction = (coefficientFriction * (newVpN - vpN) / vpT.getNorm()); //마찰 계산
			vec3 newVpT = vpT * (1 - friction);

			if (1 - friction < 0)
				newVpT.set(0, 0, 0);

			vec3 Vnew = newVpNN + newVpT;
			vec3 afterV = Vnew * (1 - weight);

			v->_vel = afterV; //업데이트
		}
	}
}

vec3 PBD_ClothObject::barycentricCoord(vec3 pos0, vec3 pos1, vec3 pos2, vec3 p, double& w, double& u, double& v)
{
	vec3 v0 = pos1 - pos0;
	vec3 v1 = pos2 - pos0;
	vec3 v2 = p - pos0;

	double dot00 = v0.dot(v0);
	double dot01 = v0.dot(v1);
	double dot02 = v0.dot(v2);
	double dot11 = v1.dot(v1);
	double dot12 = v1.dot(v2);

	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);

	double tempU = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double tempV = (dot00 * dot12 - dot01 * dot02) * invDenom;

	if (!(tempU >= 0 && tempV >= 0 && tempU + tempV <= 1))
	{
		tempU = min(tempU, 1.0);
		tempU = max(tempU, 0.0);

		tempV = min(tempV, 1.0);
		tempV = max(tempV, 0.0);
		//continue;
	}

	u = tempU;
	v = tempV;
	w = 1 - u - v;

	vec3 pc = pos0 + v0 * u + v1 * v;
	return pc;
}

void PBD_ClothObject::drawSDFPoint(void)
{
	for (int i = 0; i < _res; i++) { //모든 격자(cell) 반복 3차원
		for (int j = 0; j < _res; j++) {
			for (int k = 0; k < _res; k++)
			{
				auto px = _objectCenter.x() - ((double)_gridLength / 2) + (_gridLength * ((double)i / (double)_res));
				auto py = _objectCenter.y() - ((double)_gridLength / 2) + (_gridLength * ((double)j / (double)_res));
				auto pz = _objectCenter.z() - ((double)_gridLength / 2) + (_gridLength * ((double)k / (double)_res));

				int index = (i * _res + j) * _res + k;
				double d = _gridDistance[index];

				glColor3f(1, 1, 1);

				//printf("%f\n", d);


				glDisable(GL_LIGHTING);
				glPointSize(2.0f);
				glBegin(GL_POINTS);
				glVertex3f(px, py, pz);
				glEnd();
				glPointSize(1.0f);
				glEnable(GL_LIGHTING);
			}
		}
	}

	//vec3 p = cp;
	//double d = SDFCalculate(p);
	//printf("%f\n", d);

	//glColor3f(1, 0, 0);
	//glDisable(GL_LIGHTING);
	//glPointSize(5.0f);
	//glBegin(GL_POINTS);
	////glVertex3f(0, 0, 0);
	//glVertex3f(p.x(),p.y(),p.z());
	//glEnd();
	//glPointSize(1.0f);
	//glEnable(GL_LIGHTING);
}

void PBD_ClothObject::drawCollisionPoint(void)
{
	vec3 p = cp;

	glColor3f(1, 0, 0);
	glDisable(GL_LIGHTING);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	//glVertex3f(0, 0, 0);
	glVertex3f(p.x(), p.y(), p.z());
	glEnd();
	glPointSize(1.0f);
	glEnable(GL_LIGHTING);
}

template <typename T> inline Vec3<T> SCALAR_TO_COLOR(T val)
{
	//T fColorMap[5][3] = { { 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };	//Red->Blue
	T colorMap[4][3] = { { 0.15, 0.35, 1.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.0 }, { 0.0, 0.0, 0.0 } };
	T v = val;
	if (val > 1.0) v = 1.0; if (val < 0.0) v = 0.0; v *= 3.0;
	int low = (int)floor(v), high = (int)ceil(v);
	T t = v - low;
	Vec3<T> color;
	color.x((colorMap[low][0]) * (1 - t) + (colorMap[high][0]) * t);
	color.y((colorMap[low][1]) * (1 - t) + (colorMap[high][1]) * t);
	color.z((colorMap[low][2]) * (1 - t) + (colorMap[high][2]) * t);
	return color;

}

void PBD_ClothObject::buildRegularSDF(void)
{
	int idx = 0;
	for (int k = 0; k < _res; k++) {
		IplImage* img = cvCreateImage(cvSize(_res, _res), IPL_DEPTH_8U, 3);

		for (int i = 0; i < _res; i++) {
			for (int j = 0; j < _res; j++) {
				auto px = _objectCenter.x() - ((double)_gridLength / 2) + (_gridLength * ((double)i / (double)_res));
				auto py = _objectCenter.y() - ((double)_gridLength / 2) + (_gridLength * ((double)j / (double)_res));
				auto pz = _objectCenter.z() - ((double)_gridLength / 2) + (_gridLength * ((double)k / (double)_res));

						auto dist = SDFCalculate(Vec3<double>(px, py, pz));
						CvScalar c;
						c.val[0] = 0;
						c.val[1] = 140;
						c.val[2] = 255;
						if (dist > -0.01 && dist < 0.01)
						{
							c.val[0] = 0;
							c.val[1] = 0;
							c.val[2] = 0;
						}
						else if (dist < 0)
						{
							c.val[0] = 255;
							c.val[1] = 191;
							c.val[2] = 0;
						}
						cvSet2D(img, img->height - 1 - j, i, c);

				//double dist = 1000000.0;
				//auto value = SDFCalculate(px, py, pz);
				//dist = min(dist, value);
				//bool zeroContour = false;
				////if (dist > -0.001 && dist < 0.001) {
				//if (dist > -0.01 && dist < 0.01) {
				//	zeroContour = true;
				//}
				//if (zeroContour) {
				//	// Darw zero contour
				//	cvSet2D(img, img->height - 1 - j, i, CV_RGB(255, 255, 0));
				//}
				//else {
				//	// Darw scalar field
				//	dist *= 6.0;
				//	int integer = (int)dist;
				//	double frac = dist - (double)integer;
				//	if (frac <= 0.25) {
				//		frac = (frac / 0.25) * 1.0;
				//	}
				//	else if (frac <= 0.5) {
				//		frac -= 0.25;
				//		frac = 1.0 - ((frac / 0.25) * 1.0);
				//	}
				//	else if (frac <= 0.75) {
				//		frac -= 0.5;
				//		frac = ((frac / 0.25) * 1.0);
				//	}
				//	else if (frac <= 1.0) {
				//		frac -= 0.75;
				//		frac = 1.0 - ((frac / 0.25) * 1.0);
				//	}
				//	Vec3<double> rainbow = SCALAR_TO_COLOR(frac) * 255.0;
				//	CvScalar c;
				//	c.val[0] = rainbow.z();
				//	c.val[1] = rainbow.y();
				//	c.val[2] = rainbow.x();
				//	cvSet2D(img, img->height - 1 - j, i, c);
				//}
			}
		}
		string path = "image\\regularSDF" + to_string(k) + ".jpg";
		char* strPath = const_cast<char*>((path).c_str());
		cvSaveImage(strPath, img);
		cvReleaseImage(&img);
	}
}