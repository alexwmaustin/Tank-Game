
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>

float light_pos[2][4] = {
	{-20, 20, 20, 1.0},
	{20, 20, -20, 5.0},
};
float amb[2][4] = {
	{0.1, 0.1, 0.1, 1},
	{1, 1, 1, 1}};
float diff[2][4] = {
	{0.9, 0.9, 0.9, 1},
	{0, 0, 0, 1}};
float spec[2][4] = {
	{0.5, 0.5, 0.5, 1},
	{1, 1, 1, 1}};
GLfloat m_ambient[4] = {0.329412f, 0.223529f, 0.027451f, 1.0f};
GLfloat m_diffuse[4] = {0.5f, 0.5f, 0.0f, 1.0f};
GLfloat m_specular[4] = {0.60f, 0.60f, 0.50f, 1.0f};
GLfloat m_shininess = 27.8974f;

float pos[] = {0, 1, 0};
float rot[] = {0, 0, 0};
float headRot[] = {0, 0, 0};
std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
std::vector<std::vector<GLfloat>> vertices;
std::vector<std::vector<GLfloat>> uvs;
std::vector<std::vector<GLfloat>> normals;
std::vector<char[255]> materials; // Name
float diffuses[3][3];			  // RGB
float speculars[3][3];
GLuint tank;

/*** 
 EYE LOCATION
 ***/
float eye[] = {5.0f, 5.0f, 5.0f};

float angle = 0.0f;

bool loadObj(const char *fname,
			 std::vector<std::vector<GLfloat>> &out_vertices,
			 std::vector<std::vector<GLfloat>> &out_uvs,
			 std::vector<std::vector<GLfloat>> &out_normals)
{
	printf("Loading OBJ file %s...\n", fname);
	std::vector<std::vector<GLfloat>> temp_vertices;
	std::vector<std::vector<GLfloat>> temp_uvs;
	std::vector<std::vector<GLfloat>> temp_normals;
	FILE *fp;
	int read;
	GLfloat x, y, z;
	char ch;
	tank = glGenLists(1);
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file %s\n", fname);
		exit(1);
	}
	// glNewList(tank, GL_COMPILE);
	// {
	while (1)
	{
		char lineHeader[128];
		read = fscanf(fp, "%s", lineHeader);
		if (read == EOF)
		{
			break;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			std::vector<GLfloat> v;
			fscanf(fp, "%f %f %f\n", &x, &y, &z);
			v.push_back(x);
			v.push_back(y);
			v.push_back(z);
			temp_vertices.push_back(v);
			v.clear();
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			std::vector<GLfloat> vt;
			fscanf(fp, "%f %f\n", &x, &y);
			vt.push_back(x);
			vt.push_back(y);
			temp_uvs.push_back(vt);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			std::vector<GLfloat> vn;
			fscanf(fp, "%f %f %f\n", &x, &y, &z);
			vn.push_back(x);
			vn.push_back(y);
			vn.push_back(z);
			temp_normals.push_back(vn);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			// int matches = fscanf(fp, "%d//%d %d//%d %d//%d\n",&vertexIndex[0], &normalIndex[0],&vertexIndex[1], &normalIndex[1],&vertexIndex[2], &normalIndex[2]);
			if (matches != 12)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			else
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				vertexIndices.push_back(vertexIndex[3]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				uvIndices.push_back(uvIndex[3]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
				normalIndices.push_back(normalIndex[3]);
			}
		}
	}
	// glEndList();
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		// unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		std::vector<GLfloat> vertex = temp_vertices[vertexIndex - 1];
		// std::vector<GLfloat> uv = temp_uvs[uvIndex - 1];
		std::vector<GLfloat> normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		// out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	fclose(fp);
	return true;
}

void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	/************************************************************************
     
                        CAMERA CONTROLS
     
     ************************************************************************/
	switch (key)
	{
	case GLUT_KEY_DOWN:
		pos[2] += 0.1f;
		break;
	case GLUT_KEY_LEFT:
		pos[0] -= 0.1f;
		break;
	case GLUT_KEY_UP:
		pos[2] -= 0.1f;
	case GLUT_KEY_RIGHT:
		pos[0] += 0.1f;
	}
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	/************************************************************************
     
                            PERSPECTIVE SET UP!
     
     modelview moves the eye and objects, projection is for camera type
     frustum / ortho left, right, bottom, top, nearVal, farVal
     
     ************************************************************************/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5, 5, -5, 5, -10, 20);
}

void drawTank(float *pos, float *rot)
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	for (unsigned int i = 0; i < 2; i++)
	{
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light_pos[i]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diff[i]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, amb[i]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, spec[i]);
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);
	glRotatef(-90, 1, 0, 0);
	glPushMatrix();
	for (int i = 0; i < vertexIndices.size(); i = i + 4)
	{

		glBegin(GL_QUADS);
		glNormal3f(normals[normalIndices[i] - 1][0], normals[normalIndices[i] - 1][1], normals[normalIndices[i] - 1][2]);
		glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
		glNormal3f(normals[normalIndices[i + 1] - 1][0], normals[normalIndices[i + 1] - 1][1], normals[normalIndices[i + 1] - 1][2]);
		glVertex3f(vertices[i + 1][0], vertices[i + 1][1], vertices[i + 1][2]);
		glNormal3f(normals[normalIndices[i + 2] - 1][0], normals[normalIndices[i + 2] - 1][1], normals[normalIndices[i + 2] - 1][2]);
		glVertex3f(vertices[i + 2][0], vertices[i + 2][1], vertices[i + 2][2]);
		glNormal3f(normals[normalIndices[i + 3] - 1][0], normals[normalIndices[i + 3] - 1][1], normals[normalIndices[i + 3] - 1][2]);
		glVertex3f(vertices[i + 3][0], vertices[i + 3][1], vertices[i + 3][2]);
		glEnd();
		// glBegin(GL_QUADS);
		// glNormal3f(normals[normalIndices[i] - 1][0], normals[normalIndices[i] - 1][1], normals[normalIndices[i] - 1][2]);
		// glVertex3f(vertices[vertexIndices[i] - 1][0], vertices[vertexIndices[i] - 1][1], vertices[vertexIndices[i] - 1][2]);
		// glNormal3f(normals[normalIndices[i + 1] - 1][0], normals[normalIndices[i + 1] - 1][1], normals[normalIndices[i + 1] - 1][2]);
		// glVertex3f(vertices[vertexIndices[i + 1] - 1][0], vertices[vertexIndices[i + 1] - 1][1], vertices[vertexIndices[i + 1] - 1][2]);
		// glNormal3f(normals[normalIndices[i + 2] - 1][0], normals[normalIndices[i + 2] - 1][1], normals[normalIndices[i + 2] - 1][2]);
		// glVertex3f(vertices[vertexIndices[i + 2] - 1][0], vertices[vertexIndices[i + 2] - 1][1], vertices[vertexIndices[i + 2] - 1][2]);
		// glNormal3f(normals[normalIndices[i + 3] - 1][0], normals[normalIndices[i + 3] - 1][1], normals[normalIndices[i + 3] - 1][2]);
		// glVertex3f(vertices[vertexIndices[i + 3] - 1][0], vertices[vertexIndices[i + 3] - 1][1], vertices[vertexIndices[i + 3] - 1][2]);
		// glEnd();
	}

	// glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(std::vector<std::vector<GLfloat>>), &vertices[0], GL_STATIC_DRAW);
	glPopMatrix();
	glPopMatrix();
}
/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/************************************************************************
     
                                    CAMERA SET UP
     
     ************************************************************************/
	gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0);

	// drawBox(origin, 10, 1, 10); //draws floor
	// DrawSnowman(pos, rot);
	drawTank(pos, rot);
	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char **argv)
{
	glutInit(&argc, argv); //starts up GLUT

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Tank Example"); //creates the window

	glutDisplayFunc(display); //registers "display" as the display callback function
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	init();
	loadObj("14079_WWII_Tank_UK_Cromwell_v1_L2.obj", vertices, uvs, normals);
	glutMainLoop(); //starts the event loop

	return (0); //return may not be necessary on all compilers
}