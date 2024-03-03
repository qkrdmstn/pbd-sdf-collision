#include "SceneGraph.h"

#define SDF 1
SceneGraph::SceneGraph()
{
	pbd_center = vec3(0, 1.1, 0);

#if SDF
	_res = 64;
	_gridLength = 1;
	int size = pow(_res, 3);
	_gridDistance.resize(size);

	mesh_center = vec3(0, 0, 0);

	_mesh = new Mesh("OBJ\\geoSphere.obj", mesh_center);
	_tree = new KdTree(_mesh);
	_tree->computeTree(mesh_center + vec3(-2, -2, -2), mesh_center + vec3(2, 2, 2));
	buildRegularSDF();

	_pbdObj = new PBD_ClothObject("OBJ\\lowPlane2.obj", pbd_center, _mesh->_initCenter, _gridDistance, _gridPosition, _res, _gridLength);
#else
	_pbdObj = new PBD_ClothObject("OBJ\\lowPlane2.obj", pbd_center);
#endif
}

SceneGraph::~SceneGraph()
{

}

void SceneGraph::simulation(double dt)
{
	if (_pbdObj != NULL)
		_pbdObj->simulation(dt);
}

void SceneGraph::reset(int n)
{
	if (_pbdObj != NULL)
	{
		_pbdObj->~PBD_ClothObject();
#if SDF
		if (n == 1)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\cube.obj", pbd_center, _mesh->_initCenter, _gridDistance, _gridPosition, _res, _gridLength);
		}
		else if (n == 2)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\geoSphere.obj", pbd_center, _mesh->_initCenter, _gridDistance, _gridPosition, _res, _gridLength);
		}
		else if (n == 3)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\lowPlane2.obj", pbd_center, _mesh->_initCenter, _gridDistance, _gridPosition, _res, _gridLength);
		}
#else
		if (n == 1)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\cube.obj", pbd_center);
		}
		else if (n == 2)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\geoSphere.obj", pbd_center);
		}
		else if (n == 3)
		{
			_pbdObj = new PBD_ClothObject("OBJ\\lowPlane2.obj", pbd_center);
		}
#endif
	}
}

void SceneGraph::applyWind(vec3 wind)
{
	if (_pbdObj != NULL)
		_pbdObj->applyWind(wind);
}

void SceneGraph::drawWire(void)
{
	if (_pbdObj != NULL)
	{
		_pbdObj->drawWire();
		//_pbdObj->drawCollisionPoint();
	}
	if (_mesh != NULL)
		_mesh->drawWire();
#if !SDF
	////구 그리기
	//glTranslatef(0,0,0);
	//glColor3f(1, 0, 0);
	//glutWireSphere(2, 20, 20);
#endif
	//_tree->draw();
}

void SceneGraph::drawSolid(void)
{
	if (_pbdObj != NULL)
	{
		_pbdObj->drawSolid();
		//_pbdObj->drawCollisionPoint();

	}

	if (_mesh != NULL)
		_mesh->drawSolid();
#if !SDF
	//구 그리기
	//glTranslatef(0, 0, 0);
	//glColor3f(1, 0, 0);
	//glutSolidSphere(2, 20, 20);
#endif
	//_tree->draw();
}

void SceneGraph::drawPoint(void)
{
	if (_pbdObj != NULL)
		_pbdObj->drawPoint();
	if (_mesh != NULL)
		_mesh->drawPoint();

	//_tree->draw();
	drawSDFPoint();
}

bool flag[5] = { true, true, true, true, true };
void progress(double temp)
{
	if (temp < 20 && temp > 19.99 && flag[0])
	{
		printf("%f\n", temp);
		flag[0] = false;
	}
	else if (temp < 40 && temp > 39.99 && flag[1])
	{
		printf("%f\n", temp);
		flag[1] = false;
	}
	else if (temp < 60 && temp > 59.99 && flag[2])
	{
		printf("%f\n", temp);
		flag[2] = false;
	}
	else if (temp < 80 && temp > 79.99 && flag[3])
	{
		printf("%f\n", temp);
		flag[3] = false;
	}
	else if (temp < 100 && temp > 99.99 && flag[4])
	{
		printf("%f\n", temp);
		flag[4] = false;
	}
}

void SceneGraph::buildRegularSDF(void)
{
	printf("SDF Building\n");
	for (int i = 0; i < _res; i++) { //모든 격자(cell) 반복 3차원
		for (int j = 0; j < _res; j++) {
			for (int k = 0; k < _res; k++)
			{
				auto px = _mesh->_initCenter.x() - ((double)_gridLength / 2) + (_gridLength * ((double)i / (double)_res));
				auto py = _mesh->_initCenter.y() - ((double)_gridLength / 2) + (_gridLength * ((double)j / (double)_res));
				auto pz = _mesh->_initCenter.z() - ((double)_gridLength / 2) + (_gridLength * ((double)k / (double)_res));

				int index = (i * _res + j) * _res + k;
				double dmax = 100000;
				double dmin = -100000;
				vec3 node(px, py, pz);
				double d = _tree->getDistFromTriangle(node);

				_gridDistance[index] = max(min(d, dmax), dmin);
				
				double temp = index / pow(_res, 3) * 100;
				//progress(temp);
				printf("%f\n", temp);
			}
		}
	}
	printf("SDF Building End\n");
}

void SceneGraph::drawSDFPoint(void)
{
	for (int i = 0; i < _res; i++) { //모든 격자(cell) 반복 3차원
		for (int j = 0; j < _res; j++) {
			for (int k = 0; k < _res; k++)
			{
				auto px = _mesh->_initCenter.x() - ((double)_gridLength / 2) + (_gridLength * ((double)i / (double)_res));
				auto py = _mesh->_initCenter.y() - ((double)_gridLength / 2) + (_gridLength * ((double)j / (double)_res));
				auto pz = _mesh->_initCenter.z() - ((double)_gridLength / 2) + (_gridLength * ((double)k / (double)_res));

				int index = (i * _res + j) * _res + k;
				double d = _gridDistance[index];

				if (d < 0.00001)
					glColor3f(0.0f, 0.0f, 0.0f);
				else
					continue;

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
}