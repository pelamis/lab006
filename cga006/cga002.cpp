// cga002.cpp: определяет точку входа для консольного приложения.
//6я лаба: 
//Метод повышения реалистичности: влияние параметров (компонентов модели освещения) источника света
//Анимация: твининг-анимация, линейная
//Использование текстуры: использование текстуры для определения интенсивности поверхности
//Сделать вычисление нормалей в точках сетки (метод усреднения)
//Перед каждым вызовом glVertex делаем glNormal и glTex
//  http://www.glfw.org/docs/latest/quick.html
#define _USE_MATH_DEFINES
#include "stdafx.h"
#include "glew.h"
#include "glfw3.h"
#include "math.h"
#include <random>
#include "stdafx.h"
#include <vector>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
GLdouble PROJ_ANGLE = 45 * M_PI / 180;
GLdouble PI = M_PI;
GLdouble A = SCREEN_WIDTH / 4.0, B = 0.0, C = SCREEN_HEIGHT / 2.0, D = C;
GLdouble orth[16] = { 1, 0, 0, 0, 0, 1, 0, 0, -cos(PROJ_ANGLE), -sin(PROJ_ANGLE), 1, 0, 0, 0, 0, 1 };
GLdouble SPEED = 1, ANGLE = M_PI / 2;
GLint LAT = 3, LON = 8, pMode = GL_LINE;
GLfloat cyan[3] = { 0, 1, 1 };
GLfloat black[3] = { 0, 0, 0 };
GLfloat ascene[4] = { 0, 0.5, 0.5, 1 };
GLfloat ma[4] = { 0.2, 0.5, 0.5, 1 };
GLfloat md[4] = { 0.2, 0.2, 0.8, 1 };
GLfloat ms[4] = { 1.0,1.0,1.0,1.0 };
GLfloat emis[4] = { 1, 1, 1, 1 };
GLfloat shine[1] = { 63 };
GLfloat lcolr0[3] = { 0, 0, 1 },
n0[3] = { 1, 1, 0 },
pos0[4] = { 10, 10, 10, 0 },
a0[4] = { 0, 0, 0, 1 },
d0[4] = { 1, 1, 1, 1 },
s0[4] = { 1, 1, 1, 1 },
sd0[3] = { 0, 0, -1 },
c0 = M_PI,
e0 = 0, kc = 0.01, kl = 0, kq = 0.0;

typedef struct Point
{
	GLdouble	x;
	GLdouble	y;
	GLdouble	z;
	GLdouble	RGB[3];
} GLdoublePoint;

typedef struct Edge
{
	GLdoublePoint v1, v2;
}Edge;

typedef struct Triangle
{

}Triangle;

typedef std::vector<Point> PointArr;
typedef std::vector<PointArr> PointTable;

class Cone {
public:
	Cone(GLdouble height, GLdouble radius, Point bottomc, GLint lat, GLint lon);
	~Cone();
	void draw();
	void PolyConstruct(GLint ilat, GLint ilon);
	void setHeight(GLdouble);
	void setRadius(GLdouble);
	void resize(GLdouble bcx, GLdouble bcy);
	GLint lat, lon;
private:
	Point BottomCentre;
	GLdouble height, radius;
	PointTable Vertexes;
	Point peak;
	Edge *Edges;
	Point *CapVertexes;
};

class Light {
public:
	GLfloat *RGB,
	*normal,
	*position,
	*amb,
	*diff,
	*spec,
	*spdir,
	exp, cutoff, kc, kl, kq;

	char e = 0;
	
	Light(GLfloat *color, GLfloat *n, GLfloat *pos, GLfloat *a, GLfloat *d, GLfloat *s, GLfloat *sdir,
		GLfloat e, GLfloat coff, GLfloat k_c, GLfloat k_l, GLfloat k_q);
	void SetColor(GLfloat *color);
	void SetGeom(GLfloat *n, GLfloat *pos, GLfloat *sdir, GLfloat e, GLfloat coff);
	void SetLightConf(GLfloat *a, GLfloat *d, GLfloat *s, GLfloat k_c, GLfloat k_l, GLfloat k_q);
	void Enable();
};

Point cC = { A * 2, C, 0.0, { 0, 0, 0 } }, cC2 = { A / 4, C / 2, 0.5, { 0, 0, 0 } };
Cone kenny(C / 2, A / 2, cC, LAT, LON);
Light l0(lcolr0,n0,pos0,a0,d0,s0,sd0,0,M_PI,1,1,1);

static void turnf(GLfloat tr_x, GLfloat tr_y, GLfloat tr_z, GLfloat angle, GLfloat rot_x, GLfloat rot_y, GLfloat rot_z)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(tr_x, tr_y, tr_z);
	glRotatef(angle, rot_x, rot_y, rot_z);
	glTranslatef(-tr_x, -tr_y, -tr_z);
	printf("Turn occured\n");
}

static void movef(GLfloat mov_x, GLfloat mov_y, GLfloat mov_z)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(mov_x, mov_y, mov_z);
	printf("You moved\n");
}

static void cursor_callback(GLFWwindow* window, double x, double y)
{

}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if (action == GLFW_RELEASE) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
	int pcside;
	pcside = (width>height ? width : height) * 2;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -pcside, pcside);
	glMultMatrixd((GLdouble *)&orth);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	A = width / 4.0;
	B = 0.0;
	C = D = height / 2.0;
	printf("Reshape occured\n");
}

static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT)
	{
		turnf(A * 2, C, 0.5, -M_PI / 4, 0, SPEED, 0);
	}
	if (key == GLFW_KEY_RIGHT)
	{
		turnf(A * 2, C, 0.5, M_PI / 4, 0, SPEED, 0);
	}
	if (key == GLFW_KEY_UP)
	{
		turnf(A * 2, C, 0.5, M_PI / 4, 0, 0, SPEED);
		//movef(0,0,SPEED);
	}
	if (key == GLFW_KEY_DOWN)
	{
		turnf(A * 2, C, 0.5, -M_PI / 4, 0, 0, SPEED);
		//movef(0,0,-SPEED);
	}
	if ((key == GLFW_KEY_KP_8) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat + 1, kenny.lon);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_KP_2) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat - 1, kenny.lon);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_KP_6) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat, kenny.lon + 1);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_KP_4) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat, kenny.lon - 1);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_KP_5) && (action == GLFW_PRESS))
	{
		if (!l0.e)
		{
			l0.e = 1;
			glEnable(GL_LIGHTING);
			glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			glEnable(GL_LIGHT0);
		}
		else {
			l0.e = 0;
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHTING);
		}
	}
	if ((key == GLFW_KEY_M) && (action == GLFW_PRESS))
	{
		pMode = (pMode == GL_LINE) ? GL_FILL : GL_LINE;
		printf("Polygon mode has changed\n");
	}
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void draw()
{
	GLdouble side = (A<C ? A : C) / 2;
	l0.Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	kenny.resize(A * 2, C);
	glPushMatrix();
	glLoadIdentity();
	glPopMatrix();
	kenny.draw();

}

PointTable initVertexes(int lat, int lon)
{
	PointTable table(lat);
	int i;
	for (i = 0; i < lat; i++)
	{
		s
	}
}

int main(int argc, _TCHAR* argv[])
{
	// initialise GLFW
	if (!glfwInit())
	{
		printf("glfwInit failed\n");
		return -1;
	}

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window;
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test app", NULL, NULL);
	if (window == NULL)
	{
		printf("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?\n");
		glfwTerminate();
		return -2;
	}

	int attrib;
	attrib = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	attrib = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	attrib = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetFramebufferSizeCallback(window, resize_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_callback);

	resize_callback(window, SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(window))
	{
		draw();

		glfwSwapBuffers(window);

		glfwPollEvents();
		//glfwWaitEvents();
	}

	glfwDestroyWindow(window);

	// clean up and exit
	glfwTerminate();

	return 0;
}
//===========================CONE==============================
Cone::Cone(GLdouble height, GLdouble radius, Point bottomc, GLint ilat, GLint ilon)
{
	this->height = height;
	this->radius = radius;
	this->BottomCentre = bottomc;
	if (ilat >= 3) lat = ilat;
	else lat = 3;
	if (ilon >= 3) lon = ilon;
	else lon = 3;
	this->Vertexes = NULL;
	PolyConstruct(lat, lon);
}

void Cone::PolyConstruct(GLint ilat, GLint ilon)
{
	int i, j, a;
	GLdouble da, cx = BottomCentre.x, cy = BottomCentre.y, cz = BottomCentre.z, rad = radius, rad1;
	BottomCentre.RGB[0] = 0, BottomCentre.RGB[1] = BottomCentre.RGB[2] = 1;
	if (ilat >= 3) lat = ilat;
	else lat = 2;
	if (ilon >= 3) lon = ilon;
	else lon = 3;
	if (Vertexes.size()>0)
	{
		for (i = 0; i < lat; i++) Vertexes[i].clear();
	}
	Vertexes=initVertexes(lat,lon);
	for (i = 0; i < lat - 1; i++)
	{
		rad = radius - (i*radius) / lat;
	}

	//if (Edges != NULL) delete[] Edges;
	//if (Vertexes != NULL) delete[] Vertexes;
	//if (CapVertexes != NULL) delete[] CapVertexes;
	//this->Edges = new Edge[4 * lon*(lat - 1)-lon];
	//this->Vertexes = new Point[(lat - 1)*lon + 1];
	//this->CapVertexes = (lat >= 3) ? new Point[(lat - 2)*lon] : NULL;
	////side
	//for (j = 0; j<lat - 1; j++)
	//{
	//	rad = this->radius - (j*this->radius) / lat;
	//	for (i = 0, a = 0; i<lon; i++, a++)
	//	{
	//		da = (i*PI * 2) / ((GLdouble)lon);
	//		Vertexes[j*lon + i].x = cx + rad*cos(da);
	//		Vertexes[j*lon + i].y = cy + j*height / lat;
	//		Vertexes[j*lon + i].z = cz + rad*sin(da);
	//		Vertexes[j*lon + i].RGB[0] = 0, Vertexes[j*lon + i].RGB[1] = 1, Vertexes[j*lon + i].RGB[2] = 1;
	//		//B3W4RE 0F 1D107Z!!!
	//		if ((CapVertexes != NULL) && (j>0) && (j < lat - 1))
	//			CapVertexes[(j - 1)*lon + i].x = Vertexes[j*lon + i].x,
	//			CapVertexes[(j - 1)*lon + i].z = Vertexes[j*lon + i].z,
	//			CapVertexes[(j - 1)*lon + i].y = BottomCentre.y,
	//			CapVertexes[(j - 1)*lon + i].RGB[0] = 0,
	//			CapVertexes[(j - 1)*lon + i].RGB[1] = CapVertexes[(j - 1)*lon + i].RGB[2] = 1;

	//	}

	//}
	////cap
	//Vertexes[(lat - 1)*lon].x = cx;
	//Vertexes[(lat - 1)*lon].y = cy + height;
	//Vertexes[(lat - 1)*lon].z = cz;
	//Vertexes[(lat - 1)*lon].RGB[0] = 0, Vertexes[(lat - 1)*lon].RGB[1] = 1, Vertexes[(lat - 1)*lon].RGB[2] = 1;
	////bottom


}

Cone::~Cone() {
	printf("They killed Kenny!\n");
	delete this->Vertexes;
}


void Cone::resize(GLdouble cx, GLdouble cy)
{
	this->BottomCentre.x = cx;
	this->BottomCentre.y = cy;
	this->radius = A / 2;
	this->height = C / 2;
	this->PolyConstruct(this->lat, this->lon);

}

void Cone::draw()
{
	int i, j;
	GLdouble cx = BottomCentre.x,
		cy = BottomCentre.y,
		cz = BottomCentre.z;
	glPolygonMode(GL_FRONT_AND_BACK, pMode);
	for (j = 0; j<lat - 2; j++)
	{
		//side
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= lon; i++)
		{
			glColor3dv(Vertexes[j*lon + i%lon].RGB);
			glVertex3d(Vertexes[j*lon + i%lon].x, Vertexes[j*lon + i%lon].y, Vertexes[j*lon + i%lon].z);
			glColor3dv(Vertexes[(j + 1)*lon + i%lon].RGB);
			glVertex3d(Vertexes[(j + 1)*lon + i%lon].x, Vertexes[(j + 1)*lon + i%lon].y, Vertexes[(j + 1)*lon + i%lon].z);
		}

		glEnd();
	}
	//cap
	glBegin(GL_TRIANGLE_FAN);
	glColor3dv(Vertexes[(lat - 1)*lon].RGB);
	glVertex3d(Vertexes[(lat - 1)*lon].x, Vertexes[(lat - 1)*lon].y, Vertexes[(lat - 1)*lon].z);
	for (i = (lat - 1)*lon - lon; i<(lat - 1)*lon; i++)
	{
		glColor3dv(Vertexes[i].RGB);
		glVertex3d(Vertexes[i].x, Vertexes[i].y, Vertexes[i].z);
	}
	glColor3dv(Vertexes[(lat - 1)*lon - lon].RGB);
	glVertex3d(Vertexes[(lat - 1)*lon - lon].x, Vertexes[(lat - 1)*lon - lon].y, Vertexes[(lat - 1)*lon - lon].z);
	glEnd();
	//bottom1
	if (CapVertexes != NULL)
	{
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= lon; i++)
		{
			glColor3dv(Vertexes[i%lon].RGB);
			glVertex3d(Vertexes[i%lon].x, Vertexes[i%lon].y, Vertexes[i%lon].z);
			glColor3dv(CapVertexes[i%lon].RGB);
			glVertex3d(CapVertexes[i%lon].x, CapVertexes[i%lon].y, CapVertexes[i%lon].z);
		}

		for (i = 0; i < lat - 3; i++)
		for (j = 0; j <= lon; j++)
		{
			glColor3dv(CapVertexes[i*lon + j%lon].RGB);
			glVertex3d(CapVertexes[i*lon + j%lon].x, CapVertexes[i*lon + j%lon].y, CapVertexes[i*lon + j%lon].z);
			glColor3dv(CapVertexes[(i + 1)*lon + j%lon].RGB);
			glVertex3d(CapVertexes[(i + 1)*lon + j%lon].x, CapVertexes[(i + 1)*lon + j%lon].y, CapVertexes[(i + 1)*lon + j%lon].z);
		}

		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3dv(BottomCentre.RGB);
		glVertex3d(BottomCentre.x, BottomCentre.y, BottomCentre.z);
		for (i = (lat - 3)*lon; i < (lat - 2)*lon; i++)
		{
			glColor3dv(CapVertexes[i].RGB);
			glVertex3d(CapVertexes[i].x, CapVertexes[i].y, CapVertexes[i].z);
		}
		glEnd();
	}
	//bottom2
	else
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0, 1, 1);
		glVertex3d(BottomCentre.x, BottomCentre.y, BottomCentre.z);
		for (i = 0; i < lon; i++)
		{
			glColor3dv(Vertexes[i].RGB);
			glVertex3d(Vertexes[i].x, Vertexes[i].y, Vertexes[i].z);
		}
		glColor3dv(Vertexes[0].RGB);
		glVertex3d(Vertexes[0].x, Vertexes[0].y, Vertexes[0].z);
		glEnd();
	}

}

void Cone::setHeight(GLdouble h)
{
	this->height = h;
}

void Cone::setRadius(GLdouble r)
{
	this->radius = r;
}
//=============================================================
Light::Light(GLfloat *color, GLfloat *n, GLfloat *pos, GLfloat *a, GLfloat *d, GLfloat *s, GLfloat *sdir,
	GLfloat e, GLfloat coff, GLfloat k_c, GLfloat k_l, GLfloat k_q) {
	RGB = color;

	normal = n;
	position = pos;
	spdir = sdir;
	cutoff = coff;
	exp = e;

	amb = a;
	diff = d;
	spec = s;
	kc = k_c;
	kl = k_l;
	kq = k_q;
}

void Light::SetColor(GLfloat *color) {
	RGB = color;
}

void Light::SetGeom(GLfloat *n, GLfloat *pos, GLfloat *sdir, GLfloat e, GLfloat coff) {
	normal = n;
	position = pos;
	spdir = sdir;
	cutoff = coff;
	exp = e;
}

void Light::SetLightConf(GLfloat *a, GLfloat *d, GLfloat *s, GLfloat k_c, GLfloat k_l, GLfloat k_q) {
	amb = a;
	diff = d;
	spec = s;
	kc = k_c;
	kl = k_l;
	kq = k_q;
}

void Light::Enable() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ma);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, md);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ms);
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);
	glColor3fv(RGB);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, &kc);
	glLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, &kl);
	glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, &kq);

	glLightModelfv(GL_AMBIENT, ascene);
}