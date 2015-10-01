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
#include "OpenGLTools/Renderable.h"
using std::cout;
using std::endl;
using std::cerr;


Mat4f MVP;
Mat4f M; // Every model should have it's own Model matrix
Mat4f V;
Mat4f P;

Renderable track, cart;
float thetaXRotate, thetaYRotate = 0;
float mousePreviousY, mousePreviousX  = 0;

// TODO: store in cart?
int currentIndex = 0;
Vec3f currentPosition;


int WIN_WIDTH = 800, WIN_HEIGHT = 600;

// function declarations... just to keep things kinda organized.
void displayFunc();
void resizeFunc();
void idleFunc();
void init();
void setupVAO();
void loadBuffer(Renderable);
void loadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();
void reloadMVPUniform(Renderable);
int main( int, char** );
void mouseButtonFunc( int button, int state, int x, int y);
// function declarations

// TODO: move into cart (and create cart class which inherits from renderable)
void moveCart(float distance)
{
	int nextIndex = (currentIndex + 1) % track.getVerts().size();
	Vec3f previousPoint = track.getVerts().at(currentIndex);
	Vec3f nextPoint = track.getVerts().at(nextIndex);
	Vec3f toGo;
	Vec3f previousToNext = nextPoint - previousPoint;
	float distanceTravelled = (nextPoint - currentPosition).length();

	// We are on the correct line segment
	if( distanceTravelled >= distance)
	{
		toGo = (previousToNext * (1.0f/previousToNext.length())) * distance;

		// Special case: The cart needs to land EXACTLY on the next point
		if(distanceTravelled == distance)
		{
			currentIndex = nextIndex;
		}
	}
	// We're not on the correct line segment; find it!
	else
	{

		// Look through each segment until we've accumulated enough distance
		do
		{
			currentIndex = nextIndex;
			nextIndex = (nextIndex + 1) % track.getVerts().size();
			previousPoint = track.getVerts().at(currentIndex);
			nextPoint = track.getVerts().at(nextIndex);
			previousToNext = nextPoint - previousPoint;

			distanceTravelled +=  previousToNext.length();


		}while(distanceTravelled < distance);

		// This is necessary because we need to know how much distance was traversed
		// on every line segment BUT the one we're on
		distanceTravelled -= previousToNext.length();

		// We've found the right segment!
		// Our new position is some percentage along this segment. To find that percentage,
		// subtract how far we went on every line segment BUT this one from how far we want to go
		// (which gives us how much further we need to go) and divide it by the length of the
		// current segment
		Vec3f newPosition = previousPoint + (previousToNext * (1.0f/previousToNext.length())) * (distance - distanceTravelled);


		toGo = newPosition - currentPosition;
	}

	cart.M = cart.M * TranslateMatrix(toGo.x(), toGo.y(), toGo.z());
	currentPosition = currentPosition + toGo;

	setupModelViewProjectionTransform();

	// send changes to GPU
	reloadMVPUniform(cart);
}

// Fixes a curve so that it's points are all a fixed distance from one another
// NOTE: the length of the curve should be evenly divided by distance
std::vector<Vec3f> arcLengthReparameterize(float distance, std::vector<Vec3f> curve)
{
	// find the length of the curve (TODO: move into track (and create a track class))
	float total = 0;
	for(int i = 0; i < curve.size()-1; i++)
	{
		 Vec3f previous = curve.at(i);
		 Vec3f next = curve.at(i+1);

		 total += (next - previous).length();
	}
	if(fmodf(total, distance) != 0)
	{
		cout << "ERROR: The distance chosen does not divide evenly into the curve!\n";
		return curve;
	}

	std::vector<Vec3f> reparameterizedCurve;
	Vec3f currentPosition = curve.at(0);
	reparameterizedCurve.push_back(currentPosition);

	for(int previousIndex = 0; previousIndex < curve.size() - 1; previousIndex++)
	{

		int nextIndex = previousIndex + 1;
		Vec3f previousPoint = curve.at(previousIndex);

		Vec3f nextPoint = curve.at(nextIndex);
		Vec3f toGo;
		Vec3f previousToNext = nextPoint - previousPoint;
		float distanceTravelled = (nextPoint - currentPosition).length();

		// We are on the correct line segment
		if( distanceTravelled >= distance)
		{
			toGo = (previousToNext * (1.0f/previousToNext.length())) * distance;
			reparameterizedCurve.push_back(currentPosition + toGo);
		}
		// We're not on the correct line segment; find it!
		else
		{

			// Look through each segment until we've accumulated enough distance
			do
			{
				previousIndex = nextIndex;
				nextIndex = nextIndex + 1;
				previousPoint = curve.at(previousIndex);
				nextPoint = curve.at(nextIndex);
				previousToNext = nextPoint - previousPoint;

				distanceTravelled +=  previousToNext.length();


			}while(distanceTravelled < distance);

			// This is necessary because we need to know how much distance was traversed
			// on every line segment BUT the one we're on
			distanceTravelled -= previousToNext.length();

			// We've found the right segment!
			// Our new position is some percentage along this segment. To find that percentage,
			// subtract how far we went on every line segment BUT this one from how far we want to go
			// (which gives us how much further we need to go) and divide it by the length of the
			// current segment
			Vec3f newPosition = previousPoint + (previousToNext * (1.0f/previousToNext.length())) * (distance - distanceTravelled);


			reparameterizedCurve.push_back(newPosition);
		}
	}

	return reparameterizedCurve;
}

void mouseButtonFunc( int button, int state, int x, int y)
{
	if(button != GLUT_RIGHT_BUTTON || state != GLUT_DOWN)
	{

		return;
	}


	track.setVerts(VectorTools::subdivide(track.getVerts(), 1));
	loadBuffer(track); // TODO: may need to find a way to get rid of this

	glutPostRedisplay();
}

// Right now just a function to test if points are subdividing correctly
void mouseMotionFunc(int x, int y)
{
/*
	std::cout << "x:" << x << "\ny:" << y << "\n\n";
	std::cout << "previousX:" << mousePreviousX << "\nPreviousY:" << mousePreviousY << "\n\n";
*/
	//thetaXRotate -= (x - mousePreviousX)/100;
	thetaYRotate = ((x - mousePreviousX) / WIN_WIDTH) * 360; // May not be necessary
	mousePreviousY = y;
	mousePreviousX = x;

	M = M * RotateAboutXMatrix( thetaXRotate ) * RotateAboutYMatrix( thetaYRotate );

	setupModelViewProjectionTransform();

		// send changes to GPU
	reloadMVPUniform(track);
	reloadMVPUniform(cart);
	glutPostRedisplay();
}

void displayFunc()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Use our shader
	glUseProgram( track.basicProgramID );

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray( track.vaoID );
	// Draw line segments, start at vertex 0, draw all verts in the track
	glDrawArrays( track.getRenderMode(), 0, track.getVerts().size() );

	// Draw the cart
	glUseProgram( cart.basicProgramID);
	glBindVertexArray(cart.vaoID);
	glDrawArrays( cart.getRenderMode(), 0, cart.getVerts().size());

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
	
	// testing cart motion
	moveCart(0.1);

	glutPostRedisplay();
}

void resizeFunc( int width, int height )
{
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    glViewport( 0, 0, width, height );

    loadProjectionMatrix();
    reloadMVPUniform(cart);
    reloadMVPUniform(track);

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
	track.MVP = P * V * M * track.M; // transforms vertices from right to left (odd huh?)
	cart.MVP = P * V * M * cart.M;
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
void loadBuffer(Renderable toLoad)
{
	
	glBindBuffer( GL_ARRAY_BUFFER, toLoad.vertBufferID );
	glBufferData(	GL_ARRAY_BUFFER,	
			sizeof(Vec3f)*toLoad.getVerts().size(),	// byte size of Vec3f, 4 of them
			toLoad.getVerts().data(),		// pointer (Vec3f*) to contents of verts
			GL_STATIC_DRAW );	// Usage pattern of GPU buffer

	std::vector<float> colours;
	for(int i = 0; i < toLoad.getVerts().size(); i++){
		colours.push_back(toLoad.getColour().x());
		colours.push_back(toLoad.getColour().y());
		colours.push_back(toLoad.getColour().z());
	}

	glBindBuffer( GL_ARRAY_BUFFER, toLoad.colorBufferID );
	glBufferData(	GL_ARRAY_BUFFER,
			sizeof(float)*colours.size(),
			colours.data(),
			GL_STATIC_DRAW );
}

void init()
{
	glEnable( GL_DEPTH_TEST );


	// Read in track
	FileHelper::loadCurveFromFile("startup.vert", track);
	track.setRenderMode(GL_LINE_STRIP);
	track.setFragmentShaderPath("Shaders/basic_fs.glsl");
	track.setVertexShaderPath("Shaders/basic_vs.glsl");

	// After reading in the track, subdivide it so it's nice and smooth
	for(int i = 0; i < 4; i++)
	{
		track.setVerts(VectorTools::subdivide(track.getVerts(), 1));
		loadBuffer(track); // TODO: may need to find a way to get rid of this
	}
	// Reparameterize the arc length!
	track.setVerts(arcLengthReparameterize(0.1, track.getVerts()));


	// Create cart

	Vec3f firstTrackPoint = track.getVerts().at(0);
	cart.setColour(Vec3f(1.0f, 1.0f, 1.0f));
	cart.setVerts(std::vector<Vec3f>(
			{
		firstTrackPoint + Vec3f(-0.25, -0.25, 0.0),
		firstTrackPoint,
		firstTrackPoint + Vec3f(0.25, -0.25, 0.0)
			}));
	cart.setRenderMode(GL_TRIANGLES);
	cart.setFragmentShaderPath("Shaders/basic_fs.glsl");
	cart.setVertexShaderPath("Shaders/basic_vs.glsl");
	currentPosition = firstTrackPoint;

	// SETUP SHADERS, BUFFERS, VAOs
	// For Track:
	track.generateIDs();
	setupVAO(track);
	loadBuffer(track);

	// For Cart:
	cart.generateIDs();
	setupVAO(cart);
	loadBuffer(cart);

    loadModelViewMatrix();
    loadProjectionMatrix();
	setupModelViewProjectionTransform();
	reloadMVPUniform(track);
	reloadMVPUniform(cart);
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
	track.deleteIDs();
	cart.deleteIDs();

	return 0;
}
