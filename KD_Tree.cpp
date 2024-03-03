#include "KD_Tree.h"
#include <algorithm>
using namespace std;

#define FINDMINMAX( x0, x1, x2, min, max ) \
  min = max = x0; if(x1<min) min=x1; if(x1>max) max=x1; if(x2<min) min=x2; if(x2>max) max=x2;
// X-tests	
#define AXISTEST_X01( a, b, fa, fb )									\
	p0 = a * v0(1) - b * v0(2), p2 = a * v2(1) - b * v2(2);				\
    if (p0 < p2) { min = p0; max = p2;} else { min = p2; max = p0; }	\
	rad = fa * halfSize(1) + fb * halfSize(2);				\
	if (min > rad || max < -rad) return 0;
#define AXISTEST_X2( a, b, fa, fb )										\
	p0 = a * v0(1) - b * v0(2), p1 = a * v1(1) - b * v1(2);				\
    if (p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0;}	\
	rad = fa * halfSize(1) + fb * halfSize(2);				\
	if(min>rad || max<-rad) return 0;
// Y-tests
#define AXISTEST_Y02( a, b, fa, fb )									\
	p0 = -a * v0(0) + b * v0(2), p2 = -a * v2(0) + b * v2(2);			\
    if(p0 < p2) { min = p0; max = p2; } else { min = p2; max = p0; }	\
	rad = fa * halfSize(0) + fb * halfSize(2);				\
	if (min > rad || max < -rad) return 0;
#define AXISTEST_Y1( a, b, fa, fb )										\
	p0 = -a * v0(0) + b * v0(2), p1 = -a * v1(0) + b * v1(2);			\
    if (p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0; }	\
	rad = fa * halfSize(0) + fb * halfSize(2);				\
	if (min > rad || max < -rad) return 0;
// Z-tests
#define AXISTEST_Z12( a, b, fa, fb )									\
	p1 = a * v1(0) - b * v1(1), p2 = a * v2(0) - b * v2(1);				\
    if(p2 < p1) { min = p2; max = p1; } else { min = p1; max = p2; }	\
	rad = fa * halfSize(0) + fb * halfSize(1);				\
	if (min > rad || max < -rad) return 0;
#define AXISTEST_Z0( a, b, fa, fb )										\
	p0 = a * v0(0) - b * v0(1), p1 = a * v1(0) - b * v1(1);				\
    if(p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0; }	\
	rad = fa * halfSize(0) + fb * halfSize(1);				\
	if (min > rad || max < -rad) return 0;

KdTree::KdTree()
{
}

KdTree::KdTree(Mesh* mesh)
{
	_mesh = mesh;
	_level = 10;
}

KdTree::~KdTree()
{
}

void KdTree::clearUp(KdCorner* node)
{
	for (int i = 0; i < 2; i++) {
		if (node->_children[i] != NULL) {
			clearUp(node->_children[i]);
		}
	}
	node->clearAll();
}

void KdTree::clear(void)
{
	if (_root != NULL) {
		clearUp(_root);
	}
	_queue.clear();
	_render.clear();
}

void KdTree::computeTree(vec3 minPos, vec3 maxPos)
{
	_root = new KdCorner();
	_root->_polygons.clear();

	_root->_level = 0;

	double offset = (maxPos - minPos).length() / 80.0;
	minPos -= offset;
	maxPos += offset;

	for (auto f : _mesh->_faces) {
		_root->_polygons.push_back(f);
	}

	vec3 size = maxPos - minPos;
	_root->_boundary._pos = minPos;
	_root->_boundary._size.set(fabs(size.x()), fabs(size.y()), fabs(size.z()));

	_queue.push_back(_root);
	KdCorner* traversalNode = nullptr;
	while (_queue.size() != 0) {
		traversalNode = _queue[0];
		_queue.erase(_queue.begin());
		_render.push_back(traversalNode);
		if (traversalNode->_polygons.size() < 30) {
			continue;
		}
		else if (_level > traversalNode->_level) {
			subdivide(traversalNode);
			for (int i = 0; i < 2; i++) {
				if (traversalNode->_children[i] != NULL) {
					_queue.push_back(traversalNode->_children[i]);
				}
			}
		}
	}
	printf("build KdTree\n");
}

void KdTree::computeTree(void)
{
	_root = new KdCorner();
	_root->_polygons.clear();

	_root->_level = 0;
	vec3 minBoundary(0.0, 0.0, 0.0);
	vec3 maxBoundary(1.0, 1.0, 1.0);

	double offset = (maxBoundary - minBoundary).length() / 100.0;
	minBoundary -= offset;
	maxBoundary += offset;

	vec3 size = maxBoundary - minBoundary;//root boundary의 size와 pos 초기화
	_root->_boundary._pos = minBoundary;
	_root->_boundary._size.set(fabs(size.x()), fabs(size.y()), fabs(size.z()));

	_queue.push_back(_root); //큐에다가 root를 넣음
	KdCorner* traversalNode = nullptr;
	while (_queue.size() != 0) {
		traversalNode = _queue[0];//큐의 첫번째 원소를 trapNode에 넣고 큐에서는 지운 뒤, _render에 넣음 //다시 큐에 있는 첫번째부터 시작.
		_queue.erase(_queue.begin());
		_render.push_back(traversalNode);
		if (traversalNode->_polygons.size() < 30) { //해당 노드의 면 개수가 30개 이하면 continue;
			continue;
		}
		else if (_level > traversalNode->_level) { //tree에 사용자 설정 _level보다 아직 traversalNode의 level이 작을 경우에
			subdivide(traversalNode); //해당 노드를 subdivide 한다.
			for (int i = 0; i < 2; i++) { //나눠진 두 개의 children을 큐에 넣기.
				if (traversalNode->_children[i] != NULL) {
					_queue.push_back(traversalNode->_children[i]);
				}
			}
		}
	}
	printf("build KdTree\n");
}

void KdTree::draw(void)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	for (auto n : _render) {
		if (n == _root) {
			continue;
		}
		n->draw();
	}
	glLineWidth(1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

double KdTree::getDistFromTriangle_MT(vec3& node)
{
	vector<KdCorner*> queue;
	queue.push_back(_root);
	KdCorner* traversalNode = nullptr;

	while (queue.size()) {
		traversalNode = queue[0];
		queue.erase(queue.begin());
		// no child : leaf node
		if (traversalNode->_children[0] == nullptr) {
			break;
		}
		if (traversalNode->_splitAxis == 0) {
			if (node.x() <= traversalNode->_splitPos) {
				queue.push_back(traversalNode->_children[0]);
			}
			else {
				queue.push_back(traversalNode->_children[1]);
			}
		}
		else if (traversalNode->_splitAxis == 1) {
			if (node.y() <= traversalNode->_splitPos) {
				queue.push_back(traversalNode->_children[0]);
			}
			else {
				queue.push_back(traversalNode->_children[1]);
			}
		}
		else {
			if (node.z() <= traversalNode->_splitPos) {
				queue.push_back(traversalNode->_children[0]);
			}
			else {
				queue.push_back(traversalNode->_children[1]);
			}
		}
	}

	double range = DBL_MAX;
	for (auto f : traversalNode->_polygons) {
		for (int j = 0; j < 3; j++) {
			Vertex* v = f->_vertices[j];
			if (!v->_flag) {
				double dist = (v->_pos - node).length();
				if (dist < range) {
					range = dist;
				}
			}
			v->_flag = true;
		}
	}
	double squaredRange = range * range;

	for (auto f : traversalNode->_polygons) {
		for (int j = 0; j < 3; j++) {
			Vertex* v = f->_vertices[j];
			if (v) {
				v->_flag = false;
			}
		}
	}

	queue.clear();
	queue.push_back(_root);

	double minDist = DBL_MAX;

	while (queue.size()) {
		traversalNode = queue[0];
		queue.erase(queue.begin());
		if (isIntersectNode(traversalNode, node, squaredRange)) {
			for (int i = 0; i < traversalNode->_polygons.size(); i++) {
				Face* pFace = traversalNode->_polygons[i];
				double dist = getDistFromTriangle(node, pFace);
				if (fabs(dist) < fabs(minDist)) {
					minDist = dist;
				}
			}
			for (int i = 0; i < 2; i++) {
				if (traversalNode->_children[i] != nullptr) {
					queue.push_back(traversalNode->_children[i]);
				}
			}
			// insert queue <- child nodes
		}
	}
	return minDist;
}

double KdTree::getDistFromTriangle(vec3& node)
{
	_queue.clear();
	_queue.push_back(_root);
	KdCorner* traversalNode = nullptr;

	while (_queue.size()) { //해당 위치에서 해당하는 리프 노드 찾기
		traversalNode = _queue[0];
		_queue.erase(_queue.begin());
		// no child : leaf node
		if (traversalNode->_children[0] == nullptr) {
			break;
		}
		if (traversalNode->_splitAxis == 0) {
			if (node.x() <= traversalNode->_splitPos) {
				_queue.push_back(traversalNode->_children[0]);
			}
			else {
				_queue.push_back(traversalNode->_children[1]);
			}
		}
		else if (traversalNode->_splitAxis == 1) {
			if (node.y() <= traversalNode->_splitPos) {
				_queue.push_back(traversalNode->_children[0]);
			}
			else {
				_queue.push_back(traversalNode->_children[1]);
			}
		}
		else {
			if (node.z() <= traversalNode->_splitPos) {
				_queue.push_back(traversalNode->_children[0]);
			}
			else {
				_queue.push_back(traversalNode->_children[1]);
			}
		}
	}

	double range = DBL_MAX;
	for (auto f : traversalNode->_polygons) { //리프 노드에 속하는 face 순회
		for (int j = 0; j < 3; j++) { //해당 face의 vertex 순회
			Vertex* v = f->_vertices[j];
			if (!v->_flag) { //여러 면에 같은 정점 중복 계산 방지
				double dist = (v->_pos - node).length();
				if (dist < range) {
					range = dist; //range는 해당 노드의 면에 속한 가장 가까운 정점과의 거리
				}
			}
			v->_flag = true;
		}
	}
	double squaredRange = range * range;

	for (auto f : traversalNode->_polygons) { //다시 false로 바꾸기 (다른 노드에서는 거리가 다르기 때문)
		for (int j = 0; j < 3; j++) {
			Vertex* v = f->_vertices[j];
			v->_flag = false;
		}
	}

	_queue.clear();
	_queue.push_back(_root);

	double minDist = DBL_MAX;

	while (_queue.size()) {
		traversalNode = _queue[0];
		_queue.erase(_queue.begin());
		if (isIntersectNode(traversalNode, node, squaredRange)) {
			for (int i = 0; i < traversalNode->_polygons.size(); i++) {
				Face* f = traversalNode->_polygons[i];
				double dist = getDistFromTriangle(node, f);
				if (fabs(dist) < fabs(minDist)) {
					minDist = dist;
				}
			}
			for (int i = 0; i < 2; i++) {
				if (traversalNode->_children[i] != NULL) {
					_queue.push_back(traversalNode->_children[i]);
				}
			}
			// insert queue <- child nodes
		}
	}
	return minDist;
}

double KdTree::getDistFromTriangle(vec3& node, Face* face) //각각 face, edge, point의 가장 가까운 거리를 전부 구함. 가장 최소 거리를 반환.
{
	double minDist = DBL_MAX;
	vec3 normal;
	double dist = face->_normal.dot((face->_vertices[0]->_pos) - vec3(0.0, 0.0, 0.0));
	double projDist = dist;
	vec3 projPoint = node + (face->_normal * (projDist - node.dot(face->_normal)));

	vec3 edge_a = face->_vertices[1]->_pos - face->_vertices[0]->_pos;	// p0 -> p1
	vec3 edge_b = face->_vertices[2]->_pos - face->_vertices[1]->_pos;	// p1 -> p2
	vec3 edge_c = face->_vertices[0]->_pos - face->_vertices[2]->_pos;	// p2 -> p0
	vec3 edge_d = projPoint - face->_vertices[0]->_pos;			// p0 -> projected_point
	vec3 edge_e = projPoint - face->_vertices[1]->_pos;			// p1 -> projected_point
	vec3 edge_f = projPoint - face->_vertices[2]->_pos;			// p2 -> projected_point

	// face 
	vec3 face_intersect = face->_normal;
	face_intersect *= projDist - node.dot(face->_normal);
	face_intersect += node;

	if (edge_a.cross(edge_d).dot(face->_normal) >= 0.0 && edge_b.cross(edge_e).dot(face->_normal) >= 0.0 && edge_c.cross(edge_f).dot(face->_normal) >= 0.0) {
		//dist = face->_normal.dot(node - face->_center);
		dist = face->_normal.dot(node - face->_vertices[0]->_pos);
		return dist;
	}
	else {
		// edge 0 -> 1
		vec3 edge0 = (face->_vertices[1]->_pos - face->_vertices[0]->_pos);
		double edge0Dist = edge0.length();
		edge0.normalize();
		double rada_0 = (node - face->_vertices[0]->_pos).dot(edge0);
		edge0 *= rada_0;
		vec3 edge0Intersect = face->_vertices[0]->_pos;
		edge0Intersect += edge0;
		if (rada_0 > 0.0 && rada_0 < edge0Dist) { // 주어진 점이, edge 선분 내에 있다면, -> 선분 내에 없다면 꼭짓점과 가장 가까움
			for (int i = 0; i < 3; i++) {
				if (face->_edges[i]->hasVertex(face->_vertices[0]) && face->_edges[i]->hasVertex(face->_vertices[1])) {
					normal = face->_edges[i]->_normal; // 가장자리의 법선 벡터 == 인접한 면의 평균 법선 벡터
				}
			}
			dist = (node - edge0Intersect).length();
			if (fabs(minDist) > fabs(dist)) {
				if (normal.dot(node - edge0Intersect) < 0.0) { //메시 내부에 있다면 dist는 음수
					minDist = dist * -1.0;
				}
				else {
					minDist = dist;
				}
			}
		}

		// edge 1 -> 2
		vec3 edge1 = (face->_vertices[2]->_pos - face->_vertices[1]->_pos);
		double edge1Dist = edge1.length();
		edge1.normalize();
		double rada_1 = (node - face->_vertices[1]->_pos).dot(edge1);
		edge1 *= rada_1;
		vec3 edge1Intersect = face->_vertices[1]->_pos;
		edge1Intersect += edge1;
		if (rada_1 > 0.0 && rada_1 < edge1Dist) {
			for (int i = 0; i < 3; i++) {
				if (face->_edges[i]->hasVertex(face->_vertices[1]) && face->_edges[i]->hasVertex(face->_vertices[2])) {
					normal = face->_edges[i]->_normal;
				}
			}
			dist = (node - edge1Intersect).length();
			if (fabs(minDist) > fabs(dist)) {
				if (normal.dot(node - edge1Intersect) < 0.0) {
					minDist = dist * -1.0;
				}
				else {
					minDist = dist;
				}
			}
		}

		// edge 2 -> 0
		vec3 edge2 = (face->_vertices[0]->_pos - face->_vertices[2]->_pos);
		double edge2Dist = edge2.length();
		edge2.normalize();
		double rada_2 = (node - face->_vertices[2]->_pos).dot(edge2);
		edge2 *= rada_2;
		vec3 edge2Intersect = face->_vertices[2]->_pos;
		edge2Intersect += edge2;
		if (rada_2 > 0.0 && rada_2 < edge2Dist) {
			for (int i = 0; i < 3; i++) {
				if (face->_edges[i]->hasVertex(face->_vertices[2]) && face->_edges[i]->hasVertex(face->_vertices[0])) {
					normal = face->_edges[i]->_normal;
				}
			}
			dist = (node - edge2Intersect).length();
			if (fabs(minDist) > fabs(dist)) {
				if (normal.dot(node - edge2Intersect) < 0.0) {
					minDist = dist * -1.0;
				}
				else {
					minDist = dist;
				}
			}
		}

		// vertex 0, 1, 2
		for (int i = 0; i < 3; i++) { //각 꼭짓점과의 거리를 계산해서 minDist보다 작으면 minDist 업데이트
			dist = (node - (face->_vertices[i]->_pos)).length();
			if (fabs(minDist) > fabs(dist)) {
				if (face->_vertices[i]->_normal.dot(node - face->_vertices[i]->_pos) < 0.0) {
					minDist = dist * -1.0;
				}
				else {
					minDist = dist;
				}
			}
		}
		return minDist;
	}
}

bool KdTree::isIntersectNode(KdCorner* node, vec3& pos, double& squaredRange)
{
	double minDist = 0.0;
	vec3 nodePos = node->_boundary._pos;
	vec3 nodeSize = node->_boundary._size;
	for (int i = 0; i < 3; i++) {
		if (pos(i) < nodePos(i)) {
			minDist = minDist + (pos(i) - nodePos(i)) * (pos(i) - nodePos(i));
		}
		else if (pos(i) > (nodePos(i) + nodeSize(i))) {
			minDist = minDist + (pos(i) - (nodePos(i) + nodeSize(i))) * (pos(i) - (nodePos(i) + nodeSize(i)));
		}
	}
	return (minDist <= squaredRange);
}

void KdTree::subdivide(KdCorner* traversalNode)
{
	double width = fabs(traversalNode->_boundary.getWidth()); //전달 받은 노드의 Boundary 접근하여, boundary의 너비 높이 깊이를 가져옴.
	double height = fabs(traversalNode->_boundary.getHeight());
	double depth = fabs(traversalNode->_boundary.getDepth());

	if (width >= height) { //width = 0, height = 1, depth = 2 가장 사이즈가 큰 쪽을, splitAxis로 설정
		if (width >= depth) {
			traversalNode->_splitAxis = 0;
		}
		else {
			traversalNode->_splitAxis = 2;
		}
	}
	else {
		if (height >= depth) {
			traversalNode->_splitAxis = 1;
		}
		else {
			traversalNode->_splitAxis = 2;
		}
	}

	_minPrimitve.clear(); //뭔진 모르겠지만 Primitve min, max 초기화
	_maxPrimitve.clear();

	int index = 0, size = 0;
	int splitAxis = traversalNode->_splitAxis;
	size = (int)traversalNode->_polygons.size() * 2;

	_sortBuffer = new double[size]; //정렬 버퍼 사이즈를 노드의 면 개수의 2배로 설정
	memset(_sortBuffer, 0, sizeof(_sortBuffer)); //memset(a, b, c) -> a의 주소부터 c 길이만큼, b로 메모리 값 업데이트.

	for (auto f : traversalNode->_polygons) { //노드의 폴리곤들 순회
		vec3 v0 = f->_vertices[0]->_pos;
		double minDist = 100000.0, maxDist = -100000.0;
		for (int j = 0; j < 3; j++) { //면의 정점 중, splitAxis 축으로 좌표값이 가장 min, max한 것들을 저장
			vec3 v1 = f->_vertices[j]->_pos;
			if (v1(splitAxis) < minDist) minDist = v1(splitAxis);
			if (v1(splitAxis) > maxDist) maxDist = v1(splitAxis);
		}
		_minPrimitve.push_back(minDist); //위에서 얻은 최소 최대 값을 push
		_maxPrimitve.push_back(maxDist);
		_sortBuffer[index++] = minDist; //sort 버퍼에도 저장.
		_sortBuffer[index++] = maxDist;
	}

	// Quick sort
	std::sort(_sortBuffer, _sortBuffer + size); //오름차순 정렬
	traversalNode->_splitPos = (_sortBuffer[(size - 1) / 2] + _sortBuffer[((size - 1) / 2) + 1]) / 2.0; //중간값을 splitPos로 설정, (짝수일 경우 중간 값의 평균)
	if (_sortBuffer != NULL) {
		delete[] _sortBuffer;
	}

	double pos1 = traversalNode->_boundary._pos(splitAxis);
	double pos2 = traversalNode->_boundary._pos(splitAxis) + traversalNode->_boundary._size(splitAxis);
	KdTreeBoundary b1 = traversalNode->_boundary, b2 = traversalNode->_boundary;

	b1._size(splitAxis) = traversalNode->_splitPos - pos1; //왼쪽 절반 사이즈
	b2._pos(splitAxis) = traversalNode->_splitPos; //오른쪽 boundary의 position을 가운데로 이동, 왼쪽은 position 그대로 사용
	b2._size(splitAxis) = pos2 - traversalNode->_splitPos; //오른쪽 절반 사이즈

	double b1p1 = b1._pos(splitAxis); //각각의 Boundary에 대한 pos 정리
	double b1p2 = b1._pos(splitAxis) + b1._size(splitAxis);
	double b2p1 = b2._pos(splitAxis);
	double b2p2 = b2._pos(splitAxis) + b2._size(splitAxis);

	for (int i = 0; i < 2; i++) { //child 만들기
		KdCorner* n = new KdCorner();
		n->_level = traversalNode->_level + 1; //자식의 level은 부모 level + 1
		traversalNode->_children[i] = n; //자식으로 설정
	}

	for (int i = 0; i < traversalNode->_polygons.size(); i++) { //부모 노드의 면들을 순회
		Face* f = traversalNode->_polygons[i];
		if ((_minPrimitve[i] <= b1p2) && (_maxPrimitve[i] >= b1p1)) { //해당 바운더리에 걸쳐라도 있으면 intersection 검사.
			if (intersect(f, b1)) {
				traversalNode->_children[0]->_polygons.push_back(f); //해당 면을 자식에 push
			}
		}
		if ((_minPrimitve[i] <= b2p2) && (_maxPrimitve[i] >= b2p1)) {
			if (intersect(f, b2)) {
				traversalNode->_children[1]->_polygons.push_back(f);
			}
		}
	}
	traversalNode->_polygons.clear();
	traversalNode->_children[0]->_boundary = b1; //자식 바운더리 설정
	traversalNode->_children[1]->_boundary = b2;
}

bool KdTree::intersect(Face* face, KdTreeBoundary& box) //해당 면이 바운더리에 intersect 되었는지 검사. false => 충돌 X, true => 충돌 O
{														//AABB와 삼각형 간의 충돌 검사 -> 충돌하지 않을 경우 0을 반환 SAT(Separating Aixs Theorem)
	vec3 boxCenter = box._pos + box._size * 0.5;
	vec3 halfSize = box._size * 0.5;
	vec3 face_v0 = face->_vertices[0]->_pos;
	vec3 face_v1 = face->_vertices[1]->_pos;
	vec3 face_v2 = face->_vertices[2]->_pos;
	vec3 v0, v1, v2, normal, e0, e1, e2;
	double min, max, p0, p1, p2, rad, fex, fey, fez;
	v0 = face_v0 - boxCenter;
	v1 = face_v1 - boxCenter;
	v2 = face_v2 - boxCenter;

	e0 = v1 - v0, e1 = v2 - v1, e2 = v0 - v2;
	fex = fabs(e0(0));
	fey = fabs(e0(1));
	fez = fabs(e0(2));
	AXISTEST_X01(e0(2), e0(1), fez, fey);
	AXISTEST_Y02(e0(2), e0(0), fez, fex);
	AXISTEST_Z12(e0(1), e0(0), fey, fex);
	fex = fabs(e1(0));
	fey = fabs(e1(1));
	fez = fabs(e1(2));
	AXISTEST_X01(e1(2), e1(1), fez, fey);
	AXISTEST_Y02(e1(2), e1(0), fez, fex);
	AXISTEST_Z0(e1(1), e1(0), fey, fex);
	fex = fabs(e2(0));
	fey = fabs(e2(1));
	fez = fabs(e2(2));
	AXISTEST_X2(e2(2), e2(1), fez, fey);
	AXISTEST_Y1(e2(2), e2(0), fez, fex);
	AXISTEST_Z12(e2(1), e2(0), fey, fex);
	FINDMINMAX(v0(0), v1(0), v2(0), min, max);
	if (min > halfSize(0) || max < -halfSize(0)) return false;
	FINDMINMAX(v0(1), v1(1), v2(1), min, max);
	if (min > halfSize(1) || max < -halfSize(1)) return false;
	FINDMINMAX(v0(2), v1(2), v2(2), min, max);
	if (min > halfSize(2) || max < -halfSize(2)) return false;
	normal = e0.cross(e1);
	if (!isOverlapPlaneBox(normal, v0, halfSize)) {
		return false;
	}
	return true;
}


bool KdTree::isOverlapPlaneBox(vec3& normal, vec3& pos, vec3& maxBox)
{
	vec3 minBoundary, maxboundary;
	for (int i = 0; i < 3; i++) {
		double v = pos(i);
		if (normal(i) > 0.0) {
			minBoundary(i) = -maxBox(i) - v;
			maxboundary(i) = maxBox(i) - v;
		}
		else {
			minBoundary(i) = maxBox(i) - v;
			maxboundary(i) = -maxBox(i) - v;
		}
	}
	if (normal.dot(minBoundary) > 0.0) {
		return false;
	}
	if (normal.dot(maxboundary) >= 0.0) {
		return true;
	}
	return false;
}