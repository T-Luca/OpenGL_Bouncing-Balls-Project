#include <GL\freeglut.h>
#include <cmath>
#include <stdlib.h>

//Colors
GLfloat WHITE[] = { 1, 1, 1 };
GLfloat BLACK[] = { 0, 0, 0 };
GLfloat BLUE[] = { 0, 0, 1 };
GLfloat RED[] = { 1, 0, 0 };
GLfloat YELLOW[] = { 1, 1, 0 };
GLfloat GREEN[] = { 0, 1, 0 };
float x = 0;
float y = 0;
float z = 0;

//A camera
class Camera {
	double theta; //determinate the x and z positions
	double dTheta; // increment the theta for svinging the camera around
	double dy; // increment in y for moving the camera up/down
	double x,y,z; // the curent y position
	
public:
	Camera(): y(10), x(20), z(4), theta(0.42), dTheta(0.04), dy(0.2){}
	double getX() {
		return 20 * cos(theta);;
	}
	double getY() {
		return y;
	}
	double getZ() {
		return 10 * sin(theta);
	}
	//move camera with rotation
	void moveRight() { theta += dTheta; }
	void moveLeft() { theta -= dTheta; }
	void moveUp() { y += dy; }
	void moveDown() { if (y > dy) y -= dy; }
};

//A ball
class Ball {
	double radius;
	GLfloat*color;
	double maximumHeight;
	double x;
	double y;
	double z;
	int direction;

public:
	Ball(double r, GLfloat* c, double h, double x, double z):
		radius(r), color(c), maximumHeight(h), direction(-1), y(h), x(x), z(z) {}

	void update() {
		y += direction * 0.1;
		if (y > maximumHeight) {
			y = maximumHeight;
			direction = -1;
		}
		else if (y < radius) {
			y = radius;
			direction = 1;
		}
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glTranslated(x, y, z);
		glutSolidSphere(radius, 30, 30);
		glPopMatrix();
	}
};

//A checkerboard
class Checkerboard {
	int displayListId;
	int width;
	int depth;
public:
	Checkerboard(int width, int depth) : width(width), depth(depth) {}
	double centerx() { return width / 2; }
	double centerz() { return depth / 2; }
	void create() {
		displayListId = glGenLists(1);
		glNewList(displayListId, GL_COMPILE);
		GLfloat lightPosition[] = { 4, 3, 7, 1 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glBegin(GL_QUADS);
		glNormal3d(0, 1, 0);
		for (int x = 0; x < width - 1; x++) {
			for (int z = 0; z < depth - 1; z++) {
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
					(x + z) % 2 == 0 ? BLACK : WHITE);
				glVertex3d(x, 0, z);
				glVertex3d(x + 1, 0, z);
				glVertex3d(x + 1, 0, z + 1);
				glVertex3d(x, 0, z + 1);
			}
		}
		glEnd();
		glEndList();
	}
	void draw()
	{
		glCallList(displayListId);
	}
};

// Global variables: a camera, a checkboard and some balls
Checkerboard checkerboard(9, 9);
Camera camera;
Ball balls[] = {
	Ball(0.3, GREEN, 7, 7, 1),
	Ball(0.5, BLUE, 5, 2, 6),
	Ball(0.5, BLUE, 6, 2, 2),
	Ball(0.5, BLUE, 6, 6, 6),
	Ball(0.3, WHITE, 6, 1 , 1),
	Ball(0.6, RED, 5, 4 , 4),
	Ball(0.3, GREEN, 6, 1, 7),
	Ball(0.5, BLUE, 5, 6, 2),
	Ball(0.3, WHITE, 7, 7, 7)
};

//Application-specific initialization: Set up global lighting parameters and create display lists.
void init() {
	glClearColor( 0, 0, 1, 0);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	checkerboard.create();
}

//Draws one frame, the checkerboard then the balls, from the current camera position
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef( x, y, z ); //for move camera without rotation
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(), checkerboard.centerx(), 0.0, checkerboard.centerz(), 0.0, 1.0, 0.0);
	checkerboard.draw();
	for (int i = 0; i < sizeof balls / sizeof(Ball); i++) {
		balls[i].update();
	}
	glFlush();
	glutSwapBuffers();
}

//On reshape, constructs a camera that perfectly lists the window
void reshape(GLint M, GLint b) {
	glViewport(0, 0, M, b);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, GLfloat(M) / GLfloat(b), 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

//Requests to draw the next frame
void timer(int v)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, v);
}

//Moves the camera acording to the key pressed without rotation
void special1( int key, int, int )
{
    const float step = 0.4;
    if( GLUT_KEY_LEFT == key )
        x -= step;
    if( GLUT_KEY_RIGHT == key )
        x += step;
    if( GLUT_KEY_UP == key )
        y += step;
    if( GLUT_KEY_DOWN == key )
        y -= step;
    glutPostRedisplay();
}

//Moves the camera acording to the key pressed with rotation
void special2(int key, int, int){
	switch (key)
	{
	case GLUT_KEY_LEFT: camera.moveLeft(); break;
	case GLUT_KEY_RIGHT: camera.moveRight(); break;
	case GLUT_KEY_UP: camera.moveUp(); break;
	case GLUT_KEY_DOWN: camera.moveDown(); break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) { 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Bouncing Balls");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special1); //camera with rotation
	//glutSpecialFunc(special2); //camera without rotation
	glutTimerFunc(100, timer, 0);
	init();
	glutMainLoop();
	return 0;
}
