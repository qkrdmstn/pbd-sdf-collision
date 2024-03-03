#include <stdio.h>
#include <stdlib.h>
#include "GL\glut.h"
#include "SceneGraph.h"

float _zoom = 4.0f; // ȭ�� Ȯ��,���
float _rot_x = 0.0f; // x�� ȸ��
float _rot_y = 0.001f; // y�� ȸ��
float _trans_x = 0.0f; // x�� �̵�
float _trans_y = 0.0f; // y�� �̵�
int _last_x = 0; // ���� ���콺 Ŭ�� x��ġ
int _last_y = 0; // ���� ���콺 Ŭ�� y��ġ
unsigned char _buttons[3] = { 0 }; // ���콺 ����(����,������,�� ��ư)
bool simulation = false;

SceneGraph* _scene;
int _renderMode = 0;

void Init(void)
{
	// ���̰� ��� ����
	glEnable(GL_DEPTH_TEST);
	// ...
	_scene = new SceneGraph();
}

void Draw(void)
{
	glEnable(GL_LIGHTING); // ���� Ȱ��ȭ
	glEnable(GL_LIGHT0); // ù��° ����

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
	glLoadIdentity(); // �̰�  ������!! (�������)

	glTranslatef(0, 0, -_zoom);
	glTranslatef(_trans_x, _trans_y, 0);
	glRotatef(_rot_x, 1, 0, 0);
	glRotatef(_rot_y, 0, 1, 0);

	Draw(); // �׷����� ��ü
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
	glMatrixMode(GL_MODELVIEW); // �̰� ������!!!
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
	glutPostRedisplay(); // ȭ�鰻��
}

void Mouse(int button, int state, int x, int y)
{
	// ���� ���콺 Ŭ�� ��ġ
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
	glutPostRedisplay(); // ȭ�鰻��
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
	glutPostRedisplay(); // ȭ�鰻��
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SDFCollision");
	glutDisplayFunc(Display); // ȭ�鿡 ������ �κ�
	glutReshapeFunc(Reshape); // ȭ��â �ʱ�ȭ, â�� ũ�� ����
	glutIdleFunc(Update);
	glutMouseFunc(Mouse); // ���콺 Ŭ�� �̺�Ʈ
	glutMotionFunc(Motion); // ���콺 �̵� �̺�Ʈ
	glutKeyboardFunc(Keyboard); // Ű���� Ŭ�� �̺�Ʈ
	Init(); // ���� Ŭ����, ���� �ʱ�ȭ
	glutMainLoop();
}