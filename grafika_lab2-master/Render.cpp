#include "Render.h"
#include <sstream>
#include <iostream>
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "MyOGL.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "GUItextRectangle.h"

#define PI 3.14159265359

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������

//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����

//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}

GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}






//������
struct Point
{
	double x, y, z;
};

struct Vector
{
	double x, y, z;
};

Vector calculateVector(Point point_1, Point point_2)
{
	Vector vector;
	vector.x = point_2.x - point_1.x;
	vector.y = point_2.y - point_1.y;
	vector.z = point_2.z - point_1.z;
	return vector;
}

Vector calculateNormal(Vector vec_1, Vector vec_2)
{
	Vector normal;
	normal.x = vec_1.y * vec_2.z - vec_1.z * vec_2.y;
	normal.y = vec_1.z * vec_2.x - vec_1.x * vec_2.z;
	normal.z = vec_1.x * vec_2.y - vec_1.y * vec_2.x;

	double length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;

	return normal;
}
//�����


//������
void polygon_1(double height)
{
	double A[] = { 0,0,height };
	double B[] = { -8,-2,height };
	double C[] = { -4,-8,height };
	double H[] = { -1,-7,height };

	glBegin(GL_POLYGON);
	glNormal3d(0, 0, height > 0 ? 1 : -1);
	if (height == 0)
	{
		glColor3d(0.3, 0.2, 0.1);
	}
	else
	{
		glColor3d(0.4, 0.5, 0.9);
	}
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(H);
	glEnd();
}
//�����


//������
void polygon_2_1(double height)
{
	double C[] = { -4,-8,height };
	double D[] = { -7,-13,height };
	double E[] = { -2,-9,height };
	double F[] = { 3,-15,height };
	double G[] = { 5,-9,height };
	double H[] = { -1,-7,height };

	glBegin(GL_QUADS);
	glNormal3d(0, 0, height > 0 ? 1 : -1);
	if (height == 0)
	{
		glColor3d(0.7, 0.1, 0.9);
	}
	else
	{
		glColor3d(0.3, 0.2, 0.4);
	}
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(G);
	glVertex3dv(H);
	glEnd();
}

void polygon_2_2()
{
	double D[] = { -7,-13, 0 };
	double G[] = { 5,-9, 0 };

	double D1[] = { -7,-13, 1 };
	double G1[] = { 5,-9, 1 };

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0.7, 0.6, 0.2);
	glVertex3dv(D);
	glVertex3dv(G);
	glVertex3d(4 + 7.2111025 / 2 * cos(0.744 * PI), -10 + 7.2111025 / 2 * sin(0.744 * PI), 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glColor3d(0.7, 0.6, 0.2);
	glVertex3dv(D1);
	glVertex3dv(G1);
	glVertex3d(4 + 7.2111025 / 2 * cos(0.744 * PI), -10 + 7.2111025 / 2 * sin(0.744 * PI), 1);
	glEnd();
}

void polygon_2_3()
{
	double x0 = 3;
	double y0 = -15;

	for (double i = 0.3; i <= 1.188; i += 0.0001)
	{
		double x = 4 + 7.2111025 / 2 * cos(i * PI);
		double y = -10 + 7.2111025 / 2 * sin(i * PI);


		if (i >= 0.744)
		{
			glBegin(GL_TRIANGLES);
			glNormal3d(0, 0, -1);
			glColor3d(0.5, 0.5, 0.5);
			glVertex3d(-7, -13, 0);
			glVertex3d(x0, y0, 0);
			glVertex3d(x, y, 0);

			glNormal3d(0, 0, 1);
			glColor3d(0.5, 0.5, 0.5);
			glVertex3d(-7, -13, 1);
			glVertex3d(x0, y0, 1);
			glVertex3d(x, y, 1);
			glEnd();
		}
		if (i <= 0.744)
		{
			glBegin(GL_TRIANGLES);
			glNormal3d(0, 0, -1);
			glColor3d(0.5, 0.5, 0.5);
			glVertex3d(5, -9, 0);
			glVertex3d(x0, y0, 0);
			glVertex3d(x, y, 0);

			glNormal3d(0, 0, 1);
			glColor3d(0.5, 0.5, 0.5);
			glVertex3d(5, -9, 1);
			glVertex3d(x0, y0, 1);
			glVertex3d(x, y, 1);
			glEnd();
		}

		x0 = x;
		y0 = y;
	}
}

void concave(double centerX, double centerY, double radius, double startAngle, double endAngle, double height)
{
	int numSegments = 100;
	double angleStep = (endAngle - startAngle) / numSegments;

	glBegin(GL_QUAD_STRIP);
	glColor3d(0.5, 0.9, 0.2);
	for (int i = 0; i <= numSegments; i++)
	{
		double angle = startAngle - i * angleStep;
		double x = centerX + radius * cos(angle);
		double y = centerY + radius * sin(angle);
		glNormal3d(x, y, height);
		glVertex3d(x, y, height);
		glNormal3d(x, y, height + 1);
		glVertex3d(x, y, height + 1);
	}
	glEnd();
}

void polygon_2_concave(double height)
{
	double C[] = { -4,-8,height };
	double D[] = { -7,-13,height };
	double E[] = { -2,-9,height };
	double F[] = { 3,-15,height };
	double G[] = { 5,-9,height };
	double H[] = { -1,-7,height };

	double radius = sqrt(52) / 2.0;
	double center_x = 4;
	double center_y = -10;
	double startAngle = atan2(F[1] - center_y, F[0] - center_x);
	double endAngle = atan2(E[1] - center_y, E[0] - center_x);

	polygon_2_1(height);
	polygon_2_2();
	polygon_2_3();
	concave(center_x, center_y, radius, startAngle, endAngle, 0);
}
//�����


//������
void semicircle(double centerX, double centerY, double radius, int numSegments, double height)
{
	glBegin(GL_TRIANGLE_FAN);
	if (height == 0)
	{
		glColor3d(0.7, 0.4, 0.1);
	}
	else
	{
		glColor3d(0.8, 0.3, 0.4);
	}
	glNormal3d(0, 0, height > 0 ? 1 : -1);
	for (int i = 0; i <= numSegments; i++)
	{
		double theta = atan2(centerY, centerX) - i * (PI / numSegments);
		double x = centerX + radius * cos(theta);
		double y = centerY + radius * sin(theta);
		glVertex3d(x, y, height);
	}
	glEnd();
}

void semicircular_wall(double centerX, double centerY, double radius, int numSegments)
{
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= numSegments; i++)
	{
		double theta = atan2(centerY, centerX) - i * (PI / numSegments);
		double x = centerX + radius * cos(theta);
		double y = centerY + radius * sin(theta);
		glColor3d(0.2, 0.5, 0.2);
		glNormal3d(x, y, 1);
		glVertex3d(x, y, 1);
		glNormal3d(x, y, 0);
		glVertex3d(x, y, 0);
	}
	glEnd();
}

void polygon_face_1_convex()
{
	double radius = sqrt((-8 - 0) * (-8 - 0) + (-2 - 0) * (-2 - 0)) / 2.0;
	int numSegments = 100;

	double center_x = (-8 + 0) / 2.0;
	double center_y = (-2 + 0) / 2.0;

	semicircle(center_x, center_y, radius, numSegments, 0);
	semicircle(center_x, center_y, radius, numSegments, 1);
	semicircular_wall(center_x, center_y, radius, numSegments);
}
//�����


//������
void polygon_face_2()
{
	Point A = { -8, -2, 0 };
	Point B = { -4, -8, 0 };
	Point A1 = { -8, -2, 1 };
	Point B1 = { -4, -8, 1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.7, 0.6, 0.2);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void polygon_face_3()
{
	Point A = { -4,-8,0 };
	Point B = { -7,-13,0 };
	Point A1 = {-4,-8,1 };
	Point B1 = { -7,-13,1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.7, 0.9, 0.1);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void polygon_face_4()
{
	Point A = { -7,-13,0 };
	Point B = { -2,-9,0 };
	Point A1 = { -7,-13,1 };
	Point B1 = { -2,-9,1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.6, 0.4, 0.5);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void polygon_face_6()
{
	Point A = { 3,-15,0 };
	Point B = { 5,-9,0 };
	Point A1 = { 3,-15,1 };
	Point B1 = { 5,-9,1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.1, 0.2, 0.8);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void polygon_face_7()
{
	Point A = { 5,-9,0 };
	Point B = { -1,-7,0 };
	Point A1 = { 5,-9,1 };
	Point B1 = { -1,-7,1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.3, 0.8, 0.2);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void polygon_face_8()
{
	Point A = { -1,-7,0 };
	Point B = { 0,0,0 };
	Point A1 = { -1,-7,1 };
	Point B1 = { 0,0,1 };

	Vector AB = calculateVector(A, B);
	Vector AA1 = calculateVector(A, A1);
	Vector normal = calculateNormal(AB, AA1);

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.6);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(A.x, A.y, A.z);
	glVertex3d(B.x, B.y, B.z);
	glVertex3d(B1.x, B1.y, B1.z);
	glVertex3d(A1.x, A1.y, A1.z);
	glEnd();
}
//�����


//������
void texture_circle()
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	for (double i = 0; i <= 2; i += 0.01)
	{
		double x = 9 * cos(i * 3.141593);
		double y = 9 * sin(i * 3.141593);

		double tx = cos(i * 3.141593) * 0.5 + 0.5;
		double ty = sin(i * 3.141593) * 0.5 + 0.5;

		glColor3d(0.5f, 0.5f, 0.5f);
		glNormal3d(0, 0, 1);
		glVertex3d(x, y, -10);
	}
	glEnd();
}
//�����

void polygon_2(double height)
{
	double A[] = { -1,-7,height };
	double B[] = { 5,-9,height };
	double C[] = { 3,-15,height };
	double D[] = { -2,-9,height };
	double E[] = { -7,-13,height };
	double F[] = { -4,-8,height };


	glBegin(GL_POLYGON);
	glNormal3d(0, 0, height > 0 ? 1 : -1);
	if (height == 0)
	{
		glColor3d(0.3, 0.2, 0.1);
	}
	else
	{
		glColor3d(0.4, 0.5, 0.9);
	}
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);
	glEnd();
}

void bok()
{
	double A[] = { 3,-15,0 };
	double B[] = { -2,-9,0 };
	double C[] = { -2,-9,1 };
	double D[] = { 3,-15,1 };

	glBegin(GL_POLYGON);

	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glEnd();
}



void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	glBindTexture(GL_TEXTURE_2D, texId);



	//������ ������
	polygon_1(0);
	polygon_1(1);
	polygon_2(0);
	polygon_2(1);
	bok();
	//polygon_2_concave(0);
	//polygon_2_concave(1);
	polygon_face_1_convex();
	polygon_face_2();
	polygon_face_3();
	polygon_face_4();
	polygon_face_6();
	polygon_face_7();
	polygon_face_8();
	//����� ������




	//��������� ������ ������
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}