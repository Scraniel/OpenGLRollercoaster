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
#include "MathTools/VectorTools.h"
#include "Helpers/FileHelper.h"
#include "OpenGLTools/ShaderTools.h"
#include "SceneObjects/Renderable.h"
#include "SceneObjects/Cart.h"
#include "SceneObjects/Track.h"
using std::cout;
using std::endl;
using std::cerr;
static const float groundY = -4;

Mat4f MVP;
Mat4f M; // Every model should have it's own Model matrix
Mat4f V;
Mat4f P;

Cart cart;
Track track, arm1, arm2;
Track crossbars, struts;
Renderable ground;
std::vector<Renderable*> toRender = {&cart, &crossbars, &struts, &ground, &arm1, &arm2};
float thetaXRotate, thetaYRotate = 0;
float mousePreviousY, mousePreviousX  = 0;


int WIN_WIDTH = 800, WIN_HEIGHT = 600;

// function declarations... just to keep things kinda organized.
void displayFunc();
void resizeFunc(int, int);
void idleFunc();
void init();
void setupVAO(Renderable);
void loadBuffer(Renderable&);
void loadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();
void reloadMVPUniform(Renderable);
int main( int, char** );
void mouseButtonFunc( int button, int state, int x, int y);
void reloadMVPUniformAllObjects();
// function declarations

void mouseButtonFunc( int button, int state, int x, int y)
{
	if(button != GLUT_RIGHT_BUTTON || state != GLUT_DOWN)
	{

		return;
	}

	glutPostRedisplay();
}

// Right now just a function to test if points are subdividing correctly
void mouseMotionFunc(int x, int y)
{
/*
	std::cout << "x:" << x << "\ny:" << y << "\n\n";
	std::cout << "previousX:" << mousePreviousX << "\nPreviousY:" << mousePreviousY << "\n\n";
*/
	thetaXRotate = (y - mousePreviousY);
	//thetaYRotate = ((x - mousePreviousX) / WIN_WIDTH) * 360; // May not be necessary
	mousePreviousY = y;
	mousePreviousX = x;

	M = M * RotateAboutXMatrix( thetaXRotate ) * RotateAboutYMatrix( thetaYRotate );

	reloadMVPUniformAllObjects();

}

// Reloads the MVP uniform for all objects in the scene, and calls for a redisplay
void reloadMVPUniformAllObjects()
{
	setupModelViewProjectionTransform();
	// For every object, setup VAO's / buffers
	for(unsigned int i = 0; i < toRender.size(); i++)
	{
		Renderable & current = *toRender.at(i);
		reloadMVPUniform(current);
	}
	glutPostRedisplay();
}

void displayFunc()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Render all objects!
	//TODO: move this into renderable as an overridable
	//		function so each child can render differently
	//		(eg. pass in additional buffers)
	for(unsigned int i = 0; i < toRender.size(); i++)
	{
		Renderable & current = *toRender.at(i);

		// Use our shader
		glUseProgram( current.basicProgramID );

		// Use VAO that holds buffer bindings
		// and attribute config of buffers
		glBindVertexArray( current.vaoID );
		// Draw line segments, start at vertex 0, draw all verts in the object
		glDrawArrays( current.getRenderMode(), 0, current.getVerts().size() );
	}

	glutSwapBuffers();
}

void idleFunc()
{
	// every frame refresh, rotate quad around y axis by 1 degree
//	MVP = MVP * RotateAboutYMatrix( 1.0 );
    M = M * RotateAboutYMatrix(cart.getPositionDifference()*8.5);
    setupModelViewProjectionTransform();

	// send changes to GPU
	reloadMVPUniformAllObjects();
	
	// testing cart motion
	cart.move();
	setupModelViewProjectionTransform();

	// send changes to GPU
	reloadMVPUniform(cart);

	glutPostRedisplay();
}

void resizeFunc( int width, int height )
{
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    glViewport( 0, 0, width, height );

    loadProjectionMatrix();
    reloadMVPUniformAllObjects();

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
    M = UniformScaleMatrix( 0.25 );	// scale Scene First

    M = TranslateMatrix( 0, 0, -3 ) * M;	// translate away from (0,0,0)

    // view doesn't change, but if it did you would use this
    V = IdentityMatrix();
}

void setupModelViewProjectionTransform()
{
	for(unsigned int i = 0; i < toRender.size(); i++)
	{
		Renderable & current = *toRender.at(i);

		current.MVP = P * V * M * current.M;
	}
}
 
void reloadMVPUniform(Renderable toLoad)
{
	GLint mvpID = glGetUniformLocation( toLoad.basicProgramID, "MVP" );
	
	glUseProgram( toLoad.basicProgramID );
	glUniformMatrix4fv( 	mvpID,		// ID
				1,		// only 1 matrix
				GL_TRUE,	// transpose matrix, Mat4f is row major
				toLoad.MVP.data()	// pointer to data in Mat4f
			);
}

void setupVAO(Renderable toRender)
{
	glBindVertexArray( toRender.vaoID );

	glEnableVertexAttribArray( 0 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, toRender.vertBufferID );
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
	);

	glEnableVertexAttribArray( 1 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, toRender.colorBufferID );
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
void loadBuffer(Renderable& toLoad)
{
	
	glBindBuffer( GL_ARRAY_BUFFER, toLoad.vertBufferID );
	glBufferData(	GL_ARRAY_BUFFER,	
			sizeof(Vec3f)*toLoad.getVerts().size(),	// byte size of Vec3f, 4 of them
			toLoad.getVerts().data(),		// pointer (Vec3f*) to contents of verts
			GL_STATIC_DRAW );	// Usage pattern of GPU buffer

	std::vector<float> colours = toLoad.getColours();

	glBindBuffer( GL_ARRAY_BUFFER, toLoad.colorBufferID );
	glBufferData(	GL_ARRAY_BUFFER,
			sizeof(float)*colours.size(),
			colours.data(),
			GL_STATIC_DRAW );
}

void init()
{
	glEnable( GL_DEPTH_TEST );


	// Read in curve for track
	FileHelper::loadCurveFromFile("startup.vert", track);
	track.setRenderMode(GL_LINE_LOOP);
	track.setFragmentShaderPath("Shaders/basic_fs.glsl");
	track.setVertexShaderPath("Shaders/basic_vs.glsl");

	// After reading in the track, subdivide it so it's nice and smooth
	std::vector<Vec3f> subdivided = VectorTools::subdivide(track.getVerts(), 4);
	track.setVerts(subdivided);

	// Reparameterize the arc length!
	//track.setVerts(VectorTools::arcLengthReparameterize(subdivided.size(), subdivided));


	/*
	 * Create 2 ACTUAL tracks
	 */
	/*****************************************************/
	arm1.setRenderMode(GL_LINE_LOOP);
	arm1.setFragmentShaderPath("Shaders/basic_fs.glsl");
	arm1.setVertexShaderPath("Shaders/basic_vs.glsl");
	arm2.setRenderMode(GL_LINE_LOOP);
	arm2.setFragmentShaderPath("Shaders/basic_fs.glsl");
	arm2.setVertexShaderPath("Shaders/basic_vs.glsl");


	/*****************************************************/

	// Create cart

	Vec3f firstTrackPoint = track.getVerts().at(0);
	cart.setCurrentPosition(firstTrackPoint);
	cart.setTrack(track);
	cart.setColour(Vec3f(1.0f, 1.0f, 1.0f));
	cart.setVerts(std::vector<Vec3f>(
			{
		// Front Face
		Vec3f(-0.25, 0.0, 0.25), // Bottom left
		Vec3f(-0.25, 0.5, 0.25), // Top left
		Vec3f(0.25, 0.0, 0.25), // bottom right
		Vec3f(0.25, 0.0, 0.25),
		Vec3f(-0.25, 0.5, 0.25),
		Vec3f(0.25, 0.5, 0.25), // Top right

		// Back Face
		Vec3f(-0.25, 0.0, -0.25), // Bottom left
		Vec3f(-0.25, 0.5, -0.25), // Top left
		Vec3f(0.25, 0.0, -0.25), // bottom right
		Vec3f(0.25, 0.0, -0.25),
		Vec3f(-0.25, 0.5, -0.25),
		Vec3f(0.25, 0.5, -0.25), // Top right

		// Left Face
		Vec3f(-0.25, 0.0, 0.25), // Bottom left
		Vec3f(-0.25, 0.5, 0.25), // Top left
		Vec3f(-0.25, 0.0, -0.25), // bottom right
		Vec3f(-0.25, 0.0, -0.25),
		Vec3f(-0.25, 0.5, 0.25),
		Vec3f(-0.25, 0.5, -0.25), // Top right

		// Right Face
		Vec3f(0.25, 0.0, 0.25), // Bottom left
		Vec3f(0.25, 0.5, 0.25), // Top left
		Vec3f(0.25, 0.0, -0.25), // bottom right
		Vec3f(0.25, 0.0, -0.25),
		Vec3f(0.25, 0.5, 0.25),
		Vec3f(0.25, 0.5, -0.25), // Top right

		// Bottom
		Vec3f(-0.25, 0, 0.25),
		Vec3f(-0.25, 0, -0.25),
		Vec3f(0.25, 0, 0.25),
		Vec3f(0.25, 0, 0.25),
		Vec3f(-0.25, 0, -0.25),
		Vec3f(0.25, 0, -0.25)
			}));
	cart.setRenderMode(GL_TRIANGLES);
	cart.setFragmentShaderPath("Shaders/basic_fs.glsl");
	cart.setVertexShaderPath("Shaders/basic_vs.glsl");

	Vec3f binormal;
	Mat4f frenetFrame;
	std::vector<Vec3f> arm1Verts, arm2Verts, crossbarVerts, strutVerts;
	for(unsigned int i = 0; i < track.getVerts().size(); i++)
	{
		cart.currentIndex = i;
		frenetFrame = Cart::calcModelMatrix(track.getVerts().at(i), track, i, cart.getSpeed());
		binormal = Vec3f(frenetFrame.at(0,2),frenetFrame.at(1,2),frenetFrame.at(2,2));

		Vec3f p1 = track.getVerts().at(i) + (binormal * 0.25);
		Vec3f p2 = track.getVerts().at(i) + (binormal * -0.25);
		arm1Verts.push_back(p1);
		arm2Verts.push_back(p2);

		crossbarVerts.push_back(p1);
		crossbarVerts.push_back(p2);

		strutVerts.push_back(p1);
		strutVerts.push_back(Vec3f(p1.x(), groundY, p1.z()));
		strutVerts.push_back(p2);
		strutVerts.push_back(Vec3f(p2.x(), groundY, p2.z()));

	}
	cart.currentIndex = 0;
	arm1.setVerts(arm1Verts);
	arm1.setColour(Vec3f(0.0, 1.0, 0.0));

	arm2.setVerts(arm2Verts);
	arm2.setColour(Vec3f(0.0, 1.0, 0.0));

	crossbars.setVerts(crossbarVerts);
	crossbars.setColour(Vec3f(0.0,1.0,0.0));
	crossbars.setRenderMode(GL_LINES);
	crossbars.setFragmentShaderPath("Shaders/basic_fs.glsl");
	crossbars.setVertexShaderPath("Shaders/basic_vs.glsl");

	struts.setVerts(strutVerts);
	struts.setColour(Vec3f(0.0,1.0,0.0));
	struts.setRenderMode(GL_LINES);
	struts.setFragmentShaderPath("Shaders/basic_fs.glsl");
	struts.setVertexShaderPath("Shaders/basic_vs.glsl");

	ground.setColour(Vec3f(0.6875,0.7656,0.8672));
	ground.setRenderMode(GL_TRIANGLES);
	ground.setFragmentShaderPath("Shaders/basic_fs.glsl");
	ground.setVertexShaderPath("Shaders/basic_vs.glsl");
	ground.setVerts(std::vector<Vec3f>(
			{
		Vec3f(-10, groundY, 10),
		Vec3f(-10, groundY, -10),
		Vec3f(10, groundY, 10),
		Vec3f(10, groundY, 10),
		Vec3f(-10, groundY, -10),
		Vec3f(10, groundY, -10)
			}));

	// SETUP SHADERS, BUFFERS, VAOs

    loadModelViewMatrix();
    loadProjectionMatrix();
	setupModelViewProjectionTransform();

	// For every object, setup VAO's / buffers
	for(unsigned int i = 0; i < toRender.size(); i++)
	{
		Renderable & current = *toRender.at(i);
		current.generateIDs();
		setupVAO(current);
		loadBuffer(current);
		reloadMVPUniform(current);
	}
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
	for(unsigned int i = 0; i < toRender.size(); i++)
	{
		Renderable & current = *toRender.at(i);

		current.deleteIDs();
	}

	return 0;
}
