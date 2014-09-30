// cga002.cpp: определяет точку входа для консольного приложения.
//6я лаба: 
//Метод повышения реалистичности: влияние параметров (компонентов модели освещения) источника света
//Анимация: твининг-анимация, линейная
//Использование текстуры: использование текстуры для определения интенсивности поверхности
//Перед каждым вызовом glVertex делаем glNormal и glTex
//  http://www.glfw.org/docs/latest/quick.html
#define _USE_MATH_DEFINES
#include "stdafx.h"
#include "glew.h"
#include <gl\GLU.h>
#include "glfw3.h"
#include "math.h"
#include <random>
#include "stdafx.h"
#include <vector>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
GLfloat PROJ_ANGLE = 45 * M_PI / 180;
GLfloat PI = M_PI;
GLfloat A = SCREEN_WIDTH / 4.0, B = 0.0, C = SCREEN_HEIGHT / 2.0, D = C;
GLfloat orth[16] = { 1, 0, 0, 0, 0, 1, 0, 0, -cos(PROJ_ANGLE), -sin(PROJ_ANGLE), 1, 0, 0, 0, 0, 1 };
GLfloat SPEED = 1, ANGLE = M_PI / 2, t = 0;
GLint LAT = 3, LON = 8, pMode = GL_LINE,move,use_texture=0,light=0;
GLfloat cyan[3] = { 0, 1, 1 };
GLfloat black[3] = { 0, 0, 0 };
GLfloat lcolr0[3] = { 0, 0, 1 },
pos0[4] = { 0, -1, 0, 1 },
a0[4] = { 0.2, 0.2, 0.2, 1 },
d0[4] = { 0.9, 0.9, 0.9, 1 },
s0[4] = { 0.4, 0.4, 0.4, 1 },
sd0[3] = { 0, 0, -1 },
c0 = M_PI / 2;

unsigned int Texture;
void TextureInit();
void SaveScene();
void LoadScene();

typedef struct Point
{
	GLfloat	x;
	GLfloat	y;
	GLfloat	z;
	GLfloat	RGB[3];
	void operator = (const Point b) { x = b.x; y = b.y; z = b.z; }
	Point operator * (double k) { Point a; a.x = x * k; a.y = y * k; a.z = z * k; return a; }
	Point operator + (const Point b) { x += b.x; y += b.y; z += b.z; return *this; }
	Point operator - (const Point b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
} Point;

typedef struct Edge
{
	Point v1, v2;
}Edge;

typedef struct Vector
{
	GLfloat x, y, z;
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
	Cone(GLfloat height, GLfloat radius, Point bottomc, GLint lat, GLint lon);
	~Cone();
	void draw();
	void Normal();
	void PolyConstruct(GLint ilat, GLint ilon);
	void setHeight(GLfloat);
	void setRadius(GLfloat);
	void resize(GLfloat bcx, GLfloat bcy);
	void setBCCoords(GLfloat,GLfloat,GLfloat);
	void setPCoords(GLfloat, GLfloat, GLfloat);
	GLint lat, lon;
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
		*position,
		*amb,
		*diff,
		*spec;

	char e = 0;
	
	Light(GLfloat *color,GLfloat *pos, GLfloat *a, GLfloat *d, GLfloat *s);
	void SetColor(GLfloat *color);
	void SetGeom(GLfloat *pos);
	void SetLightConf(GLfloat *a, GLfloat *d, GLfloat *s);
	void Enable();
	void Disable();
};

Point cC = { A * 2, C, 0.0, { 0, 0, 0 } }, cC2 = { A / 4, C / 2, 0.5, { 0, 0, 0 } };
Cone kenny(C / 2, A / 2, cC, LAT, LON);
Light l0(lcolr0,pos0,a0,d0,s0);

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
	glMultMatrixf((GLfloat *)&orth);

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
			l0.Enable();
		}
		else {
			l0.e = 0;
			l0.Disable();
			
		}
	}
	if ((key == GLFW_KEY_M) && (action == GLFW_PRESS))
	{
		pMode = (pMode == GL_LINE) ? GL_FILL : GL_LINE;
		printf("Polygon mode has changed\n");
	}
	if ((key == GLFW_KEY_T) && (action == GLFW_PRESS))
	{
		if (use_texture) {
			use_texture = 0;
			glDisable(GL_TEXTURE_2D);
		}
		else {
			TextureInit();
			glEnable(GL_TEXTURE_2D);
			use_texture = 1;
		}
	}
	if ((key == GLFW_KEY_Q) && (action == GLFW_PRESS))
	{
		SaveScene();
	}
	if ((key == GLFW_KEY_E) && (action == GLFW_PRESS))
	{
		LoadScene();
	}
	if ((key == GLFW_KEY_1) && (action == GLFW_PRESS))
	{
		float k;
		printf("Ambient:\n");
		scanf_s("%f", &k);
		k = abs(k);
		a0[0] = k, a0[1] = k, a0[2] = k, a0[3] = 1;
		l0.SetLightConf(a0, d0, s0);
		l0.Disable();
		l0.Enable();
	}
	if ((key == GLFW_KEY_2) && (action == GLFW_PRESS))
	{
		float k;
		printf("Diffuse:\n");
		scanf_s("%f", &k);
		k = abs(k);
		d0[0] = k, d0[1] = k, d0[2] = k, d0[3] = 1;
		l0.SetLightConf(a0, d0, s0);
		l0.Disable();
		l0.Enable();
	}
	if ((key == GLFW_KEY_3) && (action == GLFW_PRESS))
	{
		float k;
		printf("Specular:\n");
		scanf_s("%f", &k);
		k = abs(k);
		s0[0] = k, s0[1] = k, s0[2] = k, s0[3] = 1;
		l0.SetLightConf(a0, d0, s0);
		l0.Disable();
		l0.Enable();
	}
	if ((key == GLFW_KEY_4) && (action == GLFW_PRESS))
	{
		float k;
		printf("Position:\n");
		scanf_s("%f", &k);
		k = abs(k);
		pos0[0] = k, pos0[1] = k, pos0[2] = k, pos0[3] = 1;
		l0.SetGeom(pos0);
		l0.Disable();
		l0.Enable();
	}
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void TextureInit()
{
	FILE* img;
	int offset;
	img = fopen("texture2.bmp", "rb");
	fseek(img, 0x0a, SEEK_SET); //смещение поля OffBits, указывающее на начало массива пиксельных данных
	fread(&offset, 4, 1, img);
	fseek(img, (long)offset, SEEK_SET);
	unsigned char *data = new unsigned char[786486];
	fread(data, 786486, 1, img);
	fclose(img);
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	delete data;
}
//пропустить заголовок
//выполнить изменение параметров света
void draw()
{
	GLfloat side = (A<C ? A : C) / 2;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	kenny.resize(A * 2, C);
	if (move)
	{
	/*	if ((t>0) && (t < 200))
		{
			kenny.setHeight((1-t))
		}*/
	}
	glPushMatrix();
	glLoadIdentity();
	glPopMatrix();
	kenny.draw();
	
}

void SaveScene()
{
	FILE *file;
	file = fopen("save.txt", "w");
	fprintf(file, "%d\t%d\t%d\t%d\n", pMode, use_texture, light, move);
	fprintf(file, "%f\t%f\t%f\t%f\n", a0[0], a0[1], a0[2], a0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", d0[0], d0[1], d0[2], d0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", s0[0], s0[1], s0[2], s0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", pos0[0], pos0[1], pos0[2], pos0[3]);

	fprintf(file, "%f\t%f\t%f\n", kenny.peak.x, kenny.peak.y, kenny.peak.z);
	fprintf(file, "%f\t%f\t%f\n", kenny.BottomCentre.x, kenny.BottomCentre.y, kenny.BottomCentre.z);
	fprintf(file, "%d\t%d\t%d\t%d\n", kenny.lat,kenny.lon,kenny.radius,kenny.height);
	fclose(file);
}
void LoadScene()
{
	FILE *file;
	file = fopen("save.txt", "r");
	fprintf(file, "%d\t%d\t%d\t%d\n", &pMode, &use_texture, &light, &move);
	fprintf(file, "%f\t%f\t%f\t%f\n", &a0[0], &a0[1], &a0[2], &a0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", &d0[0], &d0[1], &d0[2], &d0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", &s0[0], &s0[1], &s0[2], &s0[3]);
	fprintf(file, "%f\t%f\t%f\t%f\n", &pos0[0], &pos0[1], &pos0[2], &pos0[3]);

	fprintf(file, "%f\t%f\t%f\n", &(kenny.peak.x), &(kenny.peak.y), &(kenny.peak.z));
	fprintf(file, "%f\t%f\t%f\n", &(kenny.BottomCentre.x), &(kenny.BottomCentre.y), &(kenny.BottomCentre.z));
	fprintf(file, "%d\t%d\t%d\t%d\n", &(kenny.lat), &(kenny.lon), &(kenny.radius), &(kenny.height));
	fclose(file);
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
Cone::Cone(GLfloat height, GLfloat radius, Point bottomc, GLint ilat, GLint ilon)
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

//lat - кол-во горизонтальных линий, БЕЗ ВЕРШИНЫ!!!
void Cone::Normal()
{
	int i, j;
	Vector v1, v2;
	for (i = 0; i < Polygons.size(); i++)
	{
		v1.x = Polygons[i].p1->x - Polygons[i].p0->x;
		v1.y = Polygons[i].p1->y - Polygons[i].p0->y;
		v1.z = Polygons[i].p1->z - Polygons[i].p0->z;
		v2.x = Polygons[i].p3->x - Polygons[i].p0->x;
		v2.y = Polygons[i].p3->y - Polygons[i].p0->y;
		v2.z = Polygons[i].p3->z - Polygons[i].p0->z;
		Polygons[i].normal.x = v1.y*v2.z - v2.y*v1.z;
		Polygons[i].normal.y = v2.x*v1.z - v1.x*v2.z;
		Polygons[i].normal.z = v1.x*v2.y - v1.y*v2.x;
	}
}

void Cone::PolyConstruct(GLint ilat, GLint ilon)
{
	int i, j, a;
	GLfloat da, cx = BottomCentre.x, cy = BottomCentre.y, cz = BottomCentre.z, rad = radius, rad1;
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
			da = (j*PI * 2) / ((GLfloat)lon);
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

	for (i = 0; i < lon; i++)
	{
		Polygons.push_back({ &(Vertexes[lat - 1][i]), &(peak), NULL, &(Vertexes[lat - 1][(i + 1) % lon]) });
	}
	for (i = 0; i < lat - 1; i++)
	for (j = 0; j < lon; j++)
	{
		Polygons.push_back({ &(Vertexes[i][j]), &(Vertexes[i + 1][j]), &(Vertexes[i + 1][(j + 1) % lon]), &(Vertexes[i][(j + 1) % lon])});
	}
	for (i = 0; i < lat - 1; i++)
	for (j = 0; j < lon; j++)
	{
		Polygons.push_back({ &(CapVertexes[i][j]), &(CapVertexes[i + 1][j]), &(CapVertexes[i + 1][(j + 1) % lon]), &(CapVertexes[i][(j + 1) % lon]) });
	}
	for (i = 0; i < lon; i++)
	{
		Polygons.push_back({ &(Vertexes[0][i]), &(BottomCentre), NULL, &(Vertexes[0][(i + 1) % lon]) });
	}
	Normal();
}

Cone::~Cone() {
	int i;
	printf("They killed Kenny!\n");
	for (i = 0; i < lat - 1; i++)
		Vertexes[i].clear();
}


void Cone::resize(GLfloat cx, GLfloat cy)
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
	GLfloat cx = BottomCentre.x,
		cy = BottomCentre.y,
		cz = BottomCentre.z,
		dbgn1, dbgn2, dbgn3;
	glPolygonMode(GL_FRONT_AND_BACK, pMode);
	//if (l0.e == 1 && Polygons.size()>0) kenny.Normal();
	for (i = 0; i < Polygons.size(); i++)
	{
		if (Polygons[i].p2 != NULL)
		{
			glBegin(GL_QUADS);
			if (l0.e == 1 && Polygons.size()>0)
				dbgn1 = Polygons[i].normal.x, dbgn2 = Polygons[i].normal.y, dbgn3 = Polygons[i].normal.z;
				glNormal3f(-Polygons[i].normal.x, -Polygons[i].normal.y, -Polygons[i].normal.z);
				glColor3fv(Polygons[i].p0->RGB);
				glTexCoord2d(0, 0); glVertex3f(Polygons[i].p0->x, Polygons[i].p0->y, Polygons[i].p0->z);
				glColor3fv(Polygons[i].p1->RGB);
				glTexCoord2d(0, 1); glVertex3f(Polygons[i].p1->x, Polygons[i].p1->y, Polygons[i].p1->z);
				glColor3fv(Polygons[i].p2->RGB);
				glTexCoord2d(1, 1); glVertex3f(Polygons[i].p2->x, Polygons[i].p2->y, Polygons[i].p2->z);
				glColor3fv(Polygons[i].p3->RGB);
				glTexCoord2d(1, 0); glVertex3f(Polygons[i].p3->x, Polygons[i].p3->y, Polygons[i].p3->z);
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
			glNormal3f(-Polygons[i].normal.x, -Polygons[i].normal.y, -Polygons[i].normal.z);
			glColor3fv(Polygons[i].p0->RGB);
			glTexCoord2d(0, 0); glVertex3f(Polygons[i].p0->x, Polygons[i].p0->y, Polygons[i].p0->z);
			glColor3fv(Polygons[i].p1->RGB);
			glTexCoord2d(0, 1); glVertex3f(Polygons[i].p1->x, Polygons[i].p1->y, Polygons[i].p1->z);
			glColor3fv(Polygons[i].p3->RGB);
			glTexCoord2d(1, 1); glVertex3f(Polygons[i].p3->x, Polygons[i].p3->y, Polygons[i].p3->z);
			glEnd();
		}
	}
}

void Cone::setHeight(GLfloat h)
{
	this->height = h;
}

void setBCCoords(GLfloat x, GLfloat y, GLfloat z)
{

}

void setPCoords(GLfloat x, GLfloat y, GLfloat z)
{

}

void Cone::setRadius(GLfloat r)
{
	this->radius = r;
}

//=============================================================
Light::Light(GLfloat *color,GLfloat *pos, GLfloat *a, GLfloat *d, GLfloat *s) {
	RGB = color;
	position = pos;
	amb = a;
	diff = d;
	spec = s;
}

void Light::SetColor(GLfloat *color) {
	RGB = color;
}

void Light::SetGeom(GLfloat *pos) {
	position = pos;
}

void Light::SetLightConf(GLfloat *a, GLfloat *d, GLfloat *s) {
	amb = a;
	diff = d;
	spec = s;
}

void Light::Enable() {
	float light_ambient[] = { 0.2, 0.2, 0.2, 1 };
	float light_diffuse[] = { 0.9, 0.9, 0.9, 1 };
	float light_specular[] = { 0.4, 0.4, 0.4, 1 };
	float light_position[] = { 0, -1, 0, 1 };
//	float ma[] = { 0.3, 0.3, 0.3, 1 };
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	//glLightModelfv(GL_AMBIENT, ascene);
//	glColor3fv(RGB);
	
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, position);


	/*glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);*/
	//glMaterialfv(GL_FRONT, GL_SHININESS, shine);
	//glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
	
}

void Light::Disable()
{
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
}