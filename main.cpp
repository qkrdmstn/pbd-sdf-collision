#include <stdio.h>
#include <stdlib.h>
#include "GL\glut.h"
#include "SceneGraph.h"

float _zoom = 4.0f; // 화면 확대,축소
float _rot_x = 0.0f; // x축 회전
float _rot_y = 0.001f; // y축 회전
float _trans_x = 0.0f; // x축 이동
float _trans_y = 0.0f; // y축 이동
int _last_x = 0; // 이전 마우스 클릭 x위치
int _last_y = 0; // 이전 마우스 클릭 y위치
unsigned char _buttons[3] = { 0 }; // 마우스 상태(왼쪽,오른쪽,휠 버튼)
bool simulation = false;

SceneGraph* _scene;
int _renderMode = 0;

void Init(void)
{
	// 깊이값 사용 여부
	glEnable(GL_DEPTH_TEST);
	// ...
	_scene = new SceneGraph();
}

void Draw(void)
{
	glEnable(GL_LIGHTING); // 조명 활성화
	glEnable(GL_LIGHT0); // 첫번째 조명

	switch (_renderMode)
	{
	case 0:
		_scene->drawSolid();
		break;
	case 1:
		_scene->drawWire();
		break;
	case 2:
		_scene->drawPoint();
		break;
	}

	glDisable(GL_LIGHTING);
}

void Update(void)
{
	static int frame = 0;
	if (simulation) {
		//if (frame == 0 || frame % 4 == 0) {
		//   static int index = 0;
		//   char filename[100];
		//   sprintf(filename, "capture\\capture-%d.bmp", index++);
		//   Capture(filename, width, height);
		//}
		_scene->simulation(0.015f);
	}
	frame++;
	::glutPostRedisplay();
}

void Display(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // 이걸  빼먹음!! (단위행렬)

	glTranslatef(0, 0, -_zoom);
	glTranslatef(_trans_x, _trans_y, 0);
	glRotatef(_rot_x, 1, 0, 0);
	glRotatef(_rot_y, 0, 1, 0);

	Draw(); // 그려지는 물체
	glutSwapBuffers(); // GLUT_DOUBLE 
}

void Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW); // 이걸 빼먹음!!!
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q':
	case 'q':
		exit(0);
	case 's':
	case 'S':
		_renderMode = 0;
		break;
	case 'w':
	case 'W':
		_renderMode = 1;
		break;
	case 'p':
	case 'P':
		_renderMode = 2;
		break;
	case ' ':
		simulation = !simulation;
		break;
	case '1':
		_scene->reset(1);
		break;
	case '2':
		_scene->reset(2);
		break;
	case '3':
		_scene->reset(3);
		break;
	case '4':
		_scene->reset(4);
		break;
	case 'f':
	case 'F':
		_scene->applyWind(Vec3<double>(-0.5, -0.25, -0.25));
		break;
	default:
		break;
	}
	glutPostRedisplay(); // 화면갱신
}

void Mouse(int button, int state, int x, int y)
{
	// 이전 마우스 클릭 위치
	_last_x = x;
	_last_y = y;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		_buttons[0] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_MIDDLE_BUTTON:
		_buttons[1] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_RIGHT_BUTTON:
		_buttons[2] = state == GLUT_DOWN ? 1 : 0;
		break;
	default:
		break;
	}
	glutPostRedisplay(); // 화면갱신
}

void Motion(int x, int y)
{
	int diff_x = x - _last_x;
	int diff_y = y - _last_y;
	_last_x = x;
	_last_y = y;

	if (_buttons[2]) {
		_zoom -= (float)0.02f * diff_x;
	} else if (_buttons[1]) {
		_trans_x += (float)0.02f * diff_x;
		_trans_y -= (float)0.02f * diff_y;
	} else if (_buttons[0]) {
		_rot_x += (float)0.2f * diff_y;
		_rot_y += (float)0.2f * diff_x;
	}
	glutPostRedisplay(); // 화면갱신
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SDFCollision");
	glutDisplayFunc(Display); // 화면에 보여질 부분
	glutReshapeFunc(Reshape); // 화면창 초기화, 창의 크기 변경
	glutIdleFunc(Update);
	glutMouseFunc(Mouse); // 마우스 클릭 이벤트
	glutMotionFunc(Motion); // 마우스 이동 이벤트
	glutKeyboardFunc(Keyboard); // 키보드 클릭 이벤트
	Init(); // 각종 클래스, 변수 초기화
	glutMainLoop();
}