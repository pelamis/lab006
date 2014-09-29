// cga002.cpp: ���������� ����� ����� ��� ����������� ����������.
//6� ����: 
//����� ��������� ��������������: ������� ���������� (����������� ������ ���������) ��������� �����
//��������: �������-��������, ��������
//������������� ��������: ������������� �������� ��� ����������� ������������� �����������
//������� ���������� �������� � ������ ����� (����� ����������)
//����� ������ ������� glVertex ������ glNormal � glTex
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
e0 = 0, kc = 0.5, kl = 0.5, kq = 0.5;

typedef struct Point
{
	GLdouble	x;
	GLdouble	y;
	GLdouble	z;
	GLdouble	RGB[3];
	void operator = (const Point b) { x = b.x; y = b.y; z = b.z; }
	Point operator * (double k) { Point a; a.x = x * k; a.y = y * k; a.z = z * k; return a; }
	Point operator + (const Point b) { x += b.x; y += b.y; z += b.z; return *this; }
	Point operator - (const Point b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
} GLdoublePoint;

typedef struct Edge
{
	GLdoublePoint v1, v2;
}Edge;

typedef struct Vector
{
	GLdouble x, y, z;
}Vector;

typedef struct Poly
{
	Point *p0, *p1, *p2, *p3;
	Vector normal;
}Poly;

typedef std::vector<Point> PointArr;
typedef std::vector<PointArr> PointTable;
typedef std::vector<Poly> PolyArr;

class Cone {
public:
	Cone(GLdouble height, GLdouble radius, Point bottomc, GLint lat, GLint lon);
	~Cone();
	void draw();
	void Normal();
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
	PolyArr Polygons;
	PointTable CapVertexes;
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
	if ((key == GLFW_KEY_W) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat + 1, kenny.lon);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_S) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat - 1, kenny.lon);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_D) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat, kenny.lon + 1);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_A) && (action == GLFW_PRESS))
	{
		kenny.PolyConstruct(kenny.lat, kenny.lon - 1);
		printf("lat:\t%d\nlon:\t%d\n", kenny.lat, kenny.lon);
	}
	if ((key == GLFW_KEY_L) && (action == GLFW_PRESS))
	{
		if (!l0.e)
		{
			l0.e = 1;
			//kenny.Normal();
			l0.Enable();
		}
		else {
			l0.e = 0;
			glDisable(GL_NORMALIZE);
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//l0.Enable();
	kenny.resize(A * 2, C);
	glPushMatrix();
	glLoadIdentity();
	glPopMatrix();
	kenny.draw();

}

PointTable initVertexes(int lat, int lon)
{
	PointTable table;
	PointArr dummy;
	Point dummypoint = { 0, 0, 0, { 0, 0, 0 } };
	int i,j;
	for (i = 0; i < lon; i++) dummy.push_back(dummypoint);
	for (i = 0; i < lat; i++) table.push_back(dummy);
	return table;
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
	PolyConstruct(lat, lon);
}

//lat - ���-�� �������������� �����, ��� �������!!!
void Cone::Normal()
{
	int i, j;
	Vector v1, v2;
	for (i = 0; i < Polygons.size(); i++)
	{
		v1 = { (*(Polygons[i].p1) - *(Polygons[i].p0)).x, (*(Polygons[i].p1) - *(Polygons[i].p0)).y, (*(Polygons[i].p1) - *(Polygons[i].p0)).z };
		v2 = { (*(Polygons[i].p3) - *(Polygons[i].p0)).x, (*(Polygons[i].p3) - *(Polygons[i].p0)).y, (*(Polygons[i].p3) - *(Polygons[i].p0)).z };
		Polygons[i].normal = {v1.y*v2.z - v2.y*v1.z, v2.x*v1.z - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
	}
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
	int dbg3 = Vertexes.size();
	if (Vertexes.size()>0)
	{
		for (i = 0; i < Vertexes.size(); i++) Vertexes[i].clear();
	}
	if (CapVertexes.size()>0)
	{
		for (i = 0; i < CapVertexes.size(); i++)
		{
			CapVertexes[i].clear();
		}
	}
	if (Polygons.size()>0)
	{
		Polygons.clear();
	}
	Vertexes=initVertexes(lat,lon);
	CapVertexes = initVertexes(lat, lon);
	int dbg1 = Vertexes.size();
	int dbg2 = Vertexes[0].size();
	for (i = 0; i < lat; i++)
	{
		rad = radius - (i*radius) / lat;
		for (j = 0; j < lon; j++)
		{
			da = (j*PI * 2) / ((GLdouble)lon);
			Vertexes[i][j].x = cx + rad*cos(da);
			Vertexes[i][j].y = cy + i*height / lat;
			Vertexes[i][j].z = cz + rad*sin(da);
			Vertexes[i][j].RGB[0] = 0, Vertexes[i][j].RGB[1] = 1, Vertexes[i][j].RGB[2] = 1 ;
			//bottom
			CapVertexes[i][j].x = Vertexes[i][j].x;
			CapVertexes[i][j].z = Vertexes[i][j].z;
			CapVertexes[i][j].y = BottomCentre.y;
			CapVertexes[i][j].RGB[0] = 0, CapVertexes[i][j].RGB[1] = 1, CapVertexes[i][j].RGB[2] = 1;
		}				
	}
	//cap
	peak.x = cx;
	peak.y = cy + height;
	peak.z = cz;
	peak.RGB[0] = 0, peak.RGB[1] = 1, peak.RGB[2] = 1;

	for (i = 0; i < lat - 1; i++)
	for (j = 0; j < lon; j++)
	{
		Polygons.push_back({ &(Vertexes[i][j]), &(Vertexes[i + 1][j]), &(Vertexes[i + 1][(j + 1) % lon]), &(Vertexes[i][(j + 1) % lon])});
		Polygons.push_back({ &(CapVertexes[i][j]), &(CapVertexes[i + 1][j]), &(CapVertexes[i + 1][(j + 1) % lon]), &(CapVertexes[i][(j + 1) % lon]) });
	}
	for (i = 0; i < lon; i++)
	{
		Point dbg1 = Vertexes[lat - 1][i],
			dbg2 = peak,
			dbg3 = Vertexes[lat - 1][(i + 1) % lon];
		Polygons.push_back({ &(Vertexes[lat - 1][i]), &(peak), NULL, &(Vertexes[lat - 1][(i + 1) % lon]) });
		Polygons.push_back({ &(Vertexes[0][i]), &(BottomCentre), NULL, &(Vertexes[0][(i + 1) % lon]) });
	}
	//Normal();
}

Cone::~Cone() {
	int i;
	printf("They killed Kenny!\n");
	for (i = 0; i < lat - 1; i++)
		Vertexes[i].clear();
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
	if (l0.e == 1 && Polygons.size()>0) kenny.Normal();
	for (i = 0; i < Polygons.size(); i++)
	{
		if (Polygons[i].p2 != NULL)
		{
			glBegin(GL_QUADS);
			if (l0.e == 1 && Polygons.size()>0)
				glNormal3d(-Polygons[i].normal.x, -Polygons[i].normal.y, -Polygons[i].normal.z);
				glColor3dv(Polygons[i].p0->RGB);
				glVertex3d(Polygons[i].p0->x, Polygons[i].p0->y, Polygons[i].p0->z);
				glColor3dv(Polygons[i].p1->RGB);
				glVertex3d(Polygons[i].p1->x, Polygons[i].p1->y, Polygons[i].p1->z);
				glColor3dv(Polygons[i].p2->RGB);
				glVertex3d(Polygons[i].p2->x, Polygons[i].p2->y, Polygons[i].p2->z);
				glColor3dv(Polygons[i].p3->RGB);
				glVertex3d(Polygons[i].p3->x, Polygons[i].p3->y, Polygons[i].p3->z);
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
			glColor3dv(Polygons[i].p0->RGB);
			glVertex3d(Polygons[i].p0->x, Polygons[i].p0->y, Polygons[i].p0->z);
			glColor3dv(Polygons[i].p1->RGB);
			glVertex3d(Polygons[i].p1->x, Polygons[i].p1->y, Polygons[i].p1->z);
			glColor3dv(Polygons[i].p3->RGB);
			glVertex3d(Polygons[i].p3->x, Polygons[i].p3->y, Polygons[i].p3->z);
			glEnd();
		}
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
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glLightModelfv(GL_AMBIENT, ascene);
//	glColor3fv(RGB);
	
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
//	glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, &kc);
//	glLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, &kl);
//	glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, &kq);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ma);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, md);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ms);
//	glMaterialfv(GL_FRONT, GL_SHININESS, shine);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
	
}