// CPSC 587 Created By: Andrew Owens
// This is a (very) basic program to
// 1) load shaders from external files, and make a shader program
// 2) make Vertex Array Object and Vertex Buffer Object for the triangle

// take a look at the following sites for further readings:
// opengl-tutorial.org -> The first triangle (New OpenGL, great start)
// antongerdelan.net -> shaders pipeline explained
// ogldev.atspace.co.uk -> good resource 


// NOTE: this dependencies (include/library files) will need to be tweaked if
// you wish to run this on a non lab computer

#include<iostream>
#include<cmath>

#include<GL/glew.h> // include BEFORE glut
#include<GL/glut.h>

#include "MathTools/Mat4f.h"
#include "MathTools/OpenGLMatrixTools.h"
#include "MathTools/Vec3f.h"
#include "Helpers/FileHelper.h"
#include "OpenGLTools/GLCurve.h"
#include "OpenGLTools/ShaderTools.h"
#include "OpenGLTools/GLData.h"
using std::cout;
using std::endl;
using std::cerr;

// TODO: consider moving into GLCurve?
/******************************************************/

Mat4f MVP;
Mat4f M;
Mat4f V;
Mat4f P;
/*****************************************************/

GLData glData;

GLCurve track;
float thetaXRotate, thetaYRotate = 0;
float mousePreviousY, mousePreviousX  = 0;


int WIN_WIDTH = 800, WIN_HEIGHT = 600;

// function declarations... just to keep things kinda organized.
void displayFunc();
void resizeFunc();
void idleFunc();
void init();
void setupVAO();
void loadBuffer(std::vector<Vec3f>, Vec3f);
void loadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();
void reloadMVPUniform();
int main( int, char** );
Vec3f affineCombination(Vec3f, Vec3f, float);
void mouseButtonFunc( int button, int state, int x, int y);
std::vector<Vec3f> subdivide(std::vector<Vec3f>, int);
// function declarations

void mouseButtonFunc( int button, int state, int x, int y)
{
	if(button != GLUT_RIGHT_BUTTON || state != GLUT_DOWN)
	{
		return;
	}

	track.setVerts(subdivide(track.getVerts(), 1));
	loadBuffer(track.getVerts(), track.getColour()); // TODO: may need to find a way to get rid of this
	glutPostRedisplay();
}

// Right now just a function to test if points are subdividing correctly
void mouseMotionFunc(int x, int y)
{

	std::cout << "x:" << x << "\ny:" << y << "\n\n";
	std::cout << "previousX:" << mousePreviousX << "\nPreviousY:" << mousePreviousY << "\n\n";

	//thetaXRotate -= (x - mousePreviousX)/100;
	thetaYRotate = ((x - mousePreviousX) / WIN_WIDTH) * 360; // May not be necessary
	mousePreviousY = y;
	mousePreviousX = x;

	M = M * RotateAboutXMatrix( thetaXRotate ) * RotateAboutYMatrix( thetaYRotate );

	setupModelViewProjectionTransform();

		// send changes to GPU
	reloadMVPUniform();
	glutPostRedisplay();
}

/*
 * Creates a subdivision curve (Chaikin) using the 'Chasing Game' algorithm.
 * NOTE: Does not move the first or last points in the curve.
 *
 * points: the points that create the initial curve to subdivide.
 * depth: how many iterations to subdivide. Higher depth = more points, smoother curve.
 *
 * Returns:
 * A vector containing the new points representing the subdivided curve.
 */
std::vector<Vec3f> subdivide(std::vector<Vec3f> points, int depth)
{
	if(depth == 0)
	{
		return points;
	}

	std::vector<Vec3f> newPoints;

	// we don't want the first point to creep up, so we add it as is
	newPoints.push_back(points.at(0));

	// For each pair of points, we create a new point halfway between them
	for(int i = 0; i < points.size() - 1; i++)
	{
		Vec3f firstPoint = points.at(i);
		Vec3f secondPoint = points.at(i+1);

		Vec3f inBetweenPoint = affineCombination(firstPoint, secondPoint, 0.5);

		// Move each point halfway towards its new neighbour
		firstPoint = affineCombination(firstPoint, inBetweenPoint, 0.5);
		inBetweenPoint = affineCombination(inBetweenPoint, secondPoint, 0.5);

		if(i != 0){
			newPoints.push_back(firstPoint);
		}
		newPoints.push_back(inBetweenPoint);
	}

	// We don't want the last point to creep up either, so we add it as is
	newPoints.push_back(points.at(points.size() - 1));

	return subdivide(newPoints, depth - 1);

}

/*
 * Returns and affine combination of the two points given a proportion.
 * ie. Moves firstPoint a proportion of the way towards secondPoint.
 *
 * TODO: Move into one of the math helper classes?
 */
Vec3f affineCombination(Vec3f firstPoint, Vec3f secondPoint, float proportion)
{
	return firstPoint + ((secondPoint - firstPoint) * proportion);
}

void displayFunc()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Use our shader
	glUseProgram( glData.basicProgramID );

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray( glData.vaoID );
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)
	glDrawArrays( GL_LINE_STRIP, 0, track.getVerts().size() );

	glutSwapBuffers();
}

void idleFunc()
{
	// every frame refresh, rotate quad around y axis by 1 degree
//	MVP = MVP * RotateAboutYMatrix( 1.0 );
   // M = M * RotateAboutYMatrix( 1.0 );
    //setupModelViewProjectionTransform();

	// send changes to GPU
	//reloadMVPUniform();
	
	glutPostRedisplay();
}

void resizeFunc( int width, int height )
{
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    glViewport( 0, 0, width, height );

    loadProjectionMatrix();
    reloadMVPUniform();

    glutPostRedisplay();
}


void loadProjectionMatrix()
{
    // Perspective Only
    
	// field of view angle 60 degrees
	// window aspect ratio
	// near Z plane > 0
	// far Z plane

    P = PerspectiveProjection(  60, // FOV
                                static_cast<float>(WIN_WIDTH)/WIN_HEIGHT, // Aspect
                                0.01,   // near plane
                                5 ); // far plane depth
}

void loadModelViewMatrix()
{
    M = UniformScaleMatrix( 0.25 );	// scale Quad First

    M = TranslateMatrix( 0, 0, -1.0 ) * M;	// translate away from (0,0,0)

    // view doesn't change, but if it did you would use this
    V = IdentityMatrix();
}

void setupModelViewProjectionTransform()
{
	MVP = P * V * M; // transforms vertices from right to left (odd huh?)
}
 
void reloadMVPUniform()
{
	GLint mvpID = glGetUniformLocation( glData.basicProgramID, "MVP" );
	
	glUseProgram( glData.basicProgramID );
	glUniformMatrix4fv( 	mvpID,		// ID
				1,		// only 1 matrix
				GL_TRUE,	// transpose matrix, Mat4f is row major
				MVP.data()	// pointer to data in Mat4f
			);
}

void setupVAO()
{
	glBindVertexArray( glData.vaoID );

	glEnableVertexAttribArray( 0 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, glData.vertBufferID );
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
	);

	glEnableVertexAttribArray( 1 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, glData.colorBufferID );
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
	);

	glBindVertexArray( 0 ); // reset to default		
}

// Right now just does a solid colour
void loadBuffer(std::vector<Vec3f> verts, Vec3f colour)
{
	
	glBindBuffer( GL_ARRAY_BUFFER, glData.vertBufferID );
	glBufferData(	GL_ARRAY_BUFFER,	
			sizeof(Vec3f)*verts.size(),	// byte size of Vec3f, 4 of them
			verts.data(),		// pointer (Vec3f*) to contents of verts
			GL_STATIC_DRAW );	// Usage pattern of GPU buffer

	std::vector<float> colours;
	for(int i = 0; i < verts.size(); i++){
		colours.push_back(colour.x());
		colours.push_back(colour.y());
		colours.push_back(colour.z());
	}

	glBindBuffer( GL_ARRAY_BUFFER, glData.colorBufferID );
	glBufferData(	GL_ARRAY_BUFFER,
			sizeof(float)*colours.size(),
			colours.data(),
			GL_STATIC_DRAW );
}

void init()
{
	glEnable( GL_DEPTH_TEST );

	// SETUP SHADERS, BUFFERS, VAOs

	FileHelper::loadCurveFromFile("startup.vert", track);

	glData.generateIDs();


	setupVAO();
	loadBuffer(track.getVerts(), track.getColour());

    loadModelViewMatrix();
    loadProjectionMatrix();
	setupModelViewProjectionTransform();
	reloadMVPUniform();
}

int main( int argc, char** argv )
{
    glutInit( &argc, argv );
	// Setup FB configuration
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	
	glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
	glutInitWindowPosition( 0, 0 );

	glutCreateWindow( "Tut08" );

	glewExperimental=true; // Needed in Core Profile
	// Comment out if you want to us glBeign etc...
	if( glewInit() != GLEW_OK )
	{
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	cout << "GL Version: :" << glGetString(GL_VERSION) << endl;

    glutDisplayFunc( displayFunc );
	glutReshapeFunc( resizeFunc );
    glutIdleFunc( idleFunc );
    glutMouseFunc( mouseButtonFunc );
    glutMotionFunc( mouseMotionFunc);

	init(); // our own initialize stuff func

	glutMainLoop();

	// clean up after loop
	glData.deleteIDs();

	return 0;
}
