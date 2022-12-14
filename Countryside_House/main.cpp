//#include<windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "imageloader.h"
#include "imageloader.cpp"
#include <stdlib.h>
#include <string.h>
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

bool aux_rain = false;

#define MAX_PARTICLES 1000
#define WCX		640
#define WCY		480
#define RAIN	0
#define SNOW	1
#define	HAIL	2

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall;

typedef struct {
  // Life
  bool alive;	// is the particle alive?
  float life;	// particle lifespan
  float fade; // decay
  // color
  float red;
  float green;
  float blue;
  // Position/direction
  float xpos;
  float ypos;
  float zpos;
  // Velocity/Direction, only goes down in y dir
  float vel;
  // Gravity
  float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
    par_sys[i].alive = true;
    par_sys[i].life = 1.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;

    par_sys[i].xpos = (float) (rand() % 21) - 10;
    par_sys[i].ypos = 10.0;
    par_sys[i].zpos = (float) (rand() % 21) - 10;

    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;

    par_sys[i].vel = velocity;
    par_sys[i].gravity = -0.8;//-0.8;

}

// PAR�METROS DO LOOKAT
double view[3]= {2,2,12.9};
double look[3]= {2,2,2};
double up[3] = {0.0, 1.0, 0.0};
int flag=-1;

double angle=0,speed=5,maino=-180.0,windo1=-2.5,windo2=-3.5,tro=0,romo=0,mgo=0;
GLuint  _textureSky, _textureWindow, _texturePiso, _textureGrama;

//declarating quadric objects
GLUquadricObj *Cylinder;
GLUquadricObj *Disk;

struct tm *newtime;
time_t ltime;

GLfloat angle1;

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

//initialisation
void myinit(void)
{

    int x, z;

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    // Initialize particles
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        initParticles(loop);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0,1.0,-1*1366/768,1*1366/768,1,200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Image* image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Countryside_House/Img/piso1.bmp");
	_textureSky = loadTexture(image);
	image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Countryside_House/Img/roof.bmp");
	_textureWindow = loadTexture(image);
    image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Countryside_House/Img/bricks.bmp");
	_texturePiso = loadTexture(image);
    image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Countryside_House/Img/grass.bmp");
	_textureGrama = loadTexture(image);

	delete image;

    //defining new quadric object
    Cylinder = gluNewQuadric();
    //to set drawing style
    gluQuadricDrawStyle( Cylinder, GLU_FILL);
    //to set automatic normals
    gluQuadricNormals( Cylinder,GLU_SMOOTH);


    Disk = gluNewQuadric();
    gluQuadricDrawStyle( Disk, GLU_FILL);
    gluQuadricNormals( Disk, GLU_SMOOTH);
    GLfloat gam[]= {0.2,.2,.2,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gam);

}

// For Rain
void drawRain() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.5, 0.5, 1.0);
      glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x, y+0.5, z);
      glEnd();

      // Update values
      //Move
      // Adjust slowdown for speed!
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Hail
void drawHail() {
  float x, y, z;

  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.8, 0.8, 0.9);
      glBegin(GL_QUADS);
        // Front
        glVertex3f(x-hailsize, y-hailsize, z+hailsize); // lower left
        glVertex3f(x-hailsize, y+hailsize, z+hailsize); // upper left
        glVertex3f(x+hailsize, y+hailsize, z+hailsize); // upper right
        glVertex3f(x+hailsize, y-hailsize, z+hailsize); // lower left
        //Left
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        // Back
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        //Right
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
        //Top
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        //Bottom
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
      glEnd();

      // Update values
      //Move
      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].vel = par_sys[loop].vel * -1.0;
      }
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000); // * 1000
      par_sys[loop].vel += par_sys[loop].gravity;

      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Snow
void drawSnow() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(1.0, 1.0, 1.0);
      glPushMatrix();
      glTranslatef(x, y, z);
      glutSolidSphere(0.2, 16, 16);
      glPopMatrix();

      // Update values
      //Move
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (float) w / (float) h, .1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle ( ) {
  glutPostRedisplay();
}

//set material property
void matprop(GLfloat amb[],GLfloat dif[],GLfloat spec[],GLfloat shi[])
{
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,dif);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shi);
}

void setLeftView()
{
    up[0] = 0;
    up[1] = 1;
    up[2] = 0;

    look[0] = 0;

    view[0] = -7;
    view[1] = 3;
    view[2] = 2;
}

void setRightView()
{
    up[0] = 0;
    up[1] = 1;
    up[2] = 0;

    look[1] = 0;

    view[0] = 12;
    view[1] = 3;
    view[2] = 2.5;
}

void setTopView()
{
    view[0]=6;
    view[1]=12;
    view[2]=10;
    look[0]=2;
    look[1]=4;
    look[2]=2;
}

void setFrontView()
{
    view[0]=2;
    view[1]=2;
    view[2]=12.9;
    look[0]=2;
    look[1]=2;
    look[2]=3;
}

void setBackView()
{
    view[0]=1;
    view[1]=6;
    view[2]=-7;
    look[0]=2;
    look[1]=4;
    look[2]=2;
}

void front_Triangulo(void)
{
    
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _textureSky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTranslated(4,9.6,4.01);
    glScaled(.11,0.28,1);
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0,1.0); glVertex2f( -9, -10.8 );
        glTexCoord2f(1.0,1.0); glVertex2f( -37,-20);
        glTexCoord2f(0.5,0.0); glVertex2f( 18, -20);
    glEnd();
    glPopMatrix();
}

void back_Triangulo(void)
{
    glPushMatrix();
    glTranslated(4,9.6,-0.03);
    glScaled(.11,0.28,1);
    glBegin(GL_TRIANGLES);
    glVertex2f( -9, -10.8 );
    glVertex2f( -37,-20);
    glVertex2f( 18, -20);
    glEnd();
    glPopMatrix();
}

void var1_triangulo(void)
{
    glPushMatrix();
    glTranslated(3.8,2.3,2.95);
    glScaled(.11,0.28,1);
    glRotated(-90,0,1,0);
    glBegin(GL_TRIANGLES);
    glVertex2f(1.0,6);
    glVertex2f(2.5, 1);
    glVertex2f(1, 1);
    glEnd();
    glPopMatrix();
}

void var2_triangulo(void)
{
    glPushMatrix();
    glTranslated(5.8,2.3,2.95);
    glScaled(.11,0.28,1);
    glRotated(-90,0,1,0);
    glBegin(GL_TRIANGLES);
    glVertex2f(1.0,6);
    glVertex2f(2.5, 1);
    glVertex2f(1, 1);
    glEnd();
    glPopMatrix();
}

void right_window(void)
{
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _textureWindow);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTranslated(6.13,3,0.68);
    glScaled(.16,1,1);
    glRotated(-90.0,0,1,0);
    glBegin(GL_QUADS);
            glTexCoord2f(1,0);  glVertex3f(0.56,0,1); //1
            glTexCoord2f(2.0,0); glVertex3f(2,0,1); //2
            glTexCoord2f(2.0,-1); glVertex3f(2,-1.6,1); //3
            glTexCoord2f(1,-1);  glVertex3f(0.56,-1.6,1); //4
    glEnd();
    glPopMatrix();
}

//to create wall
void wall(double thickness)
{
    glPushMatrix();
    glTranslated(2,.5*thickness,2);
    glScaled(4.0,thickness,4.0);
    glutSolidCube(1.0);
    glPopMatrix();
}
//to create compound wall
void wall2(double thickness)
{
    glPushMatrix();
    glTranslated(.8,.5*thickness*4,3.5);
    glScaled(1.6,thickness*4,7.0);
    glutSolidCube(1.0);
    glPopMatrix();
}

//to create earth
void earth(void)
{
    GLfloat ambient[]= {1,0,0,1};
    GLfloat specular[]= {0,1,1,1};
    GLfloat diffuse[]= {.2,.9,.5,1};
    GLfloat shininess[]= {50};
    matprop(ambient,diffuse,specular,shininess);

    GLfloat lightIntensity[]= {.7,.7,.7,1};
    GLfloat light_position[]= {2,5,-3,0};
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightIntensity);

    glPushMatrix();
    glTranslated(0,-.25,0);
    glScaled(5000,.5,5000);
    glutSolidCube(1.0);
    glPopMatrix();
    glFlush();
}

void gramado(void)
{
   glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _textureGrama);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTranslated(10.2,1.6,-13);
    glScaled(.16,3,10.4);
    glRotated(-90.0,0,1,0);
    glBegin(GL_QUADS);
            glTexCoord2f(1,0);  glVertex3f(0.56,0,1); //1
            glTexCoord2f(2.0,0); glVertex3f(2,0,1); //2
            glTexCoord2f(2.0,-1); glVertex3f(2,-1.6,1); //3
            glTexCoord2f(1,-1);  glVertex3f(0.56,-1.6,1); //4
    glEnd();
    glPopMatrix();
}

void piso(void)
{
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _texturePiso);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTranslated(1,-0.539,4.48);
    glRotated(90.0,0,1,0);
    glRotated(-90.0,1,0,0);
    glBegin(GL_QUADS);
            glTexCoord2f(4,0);  glVertex3f(0.56,1,1); //1
            glTexCoord2f(8,0); glVertex3f(4.5,1,1); //2
            glTexCoord2f(8,-4); glVertex3f(4.5,-4.95,1); //3
            glTexCoord2f(4,-4);  glVertex3f(0.56,-4.95,1); //4
    glEnd();
    glPopMatrix();
}

void compound(void)      //carpaser gher
{

	GLfloat ambient[]={1,0,0,1};
	GLfloat specular[]={0,1,1,1};
	GLfloat diffuse[]={.7,.7,.7,1};
	GLfloat shininess[]={50};


	matprop(ambient,diffuse,specular,shininess);
	GLfloat lightIntensity[]={.7,.7,.8,1};
	GLfloat light_position[]={2,6,1.5,0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lightIntensity);

	//left wall of compound
	glPushMatrix();
	glPushMatrix();
	glTranslated(-6,0,-2.5-.04);
	glRotated(90.0,0,0,1);
	
    glPushMatrix();
    glTranslated(.8,.5*(0.08)*4,3.5);
    glScaled(1.6,0.08*4,14.0);
    glutSolidCube(1.0);
    glPopMatrix();

	glPopMatrix();
	//right wall of compound
	glPushMatrix();
	glTranslated(10,0,-3-.02);
	glRotated(90.0,0,0,1);
	
    glPushMatrix();
    glTranslated(.8,.5*(0.08)*4,3.5);
    glScaled(1.6,0.08*4,14.0);
    glutSolidCube(1.0);
    glPopMatrix();

	glPopMatrix();
	//back wall of compound
	glPushMatrix();
	glTranslated(2,.8,-6);
	glRotated(-90,1,0,0);
	glScaled(16,.02*4,1.6);
	glutSolidCube(1.0);
	glPopMatrix();

	//front left wall of compound
	glPushMatrix();
	glTranslated(-4,.8,8-.08);
	glRotated(-90,1,0,0);
	glScaled(4,.02*4,1.6);
	glutSolidCube(1.0);
	glPopMatrix();
	//front right wall of compound
	glPushMatrix();
	glTranslated(5,.8,8-.08);
	glRotated(-90,1,0,0);
	glScaled(10,.02*4,1.6);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();

	GLfloat ambient2[]={0,1,0,1};
	GLfloat specular2[]={1,1,1,1};
	GLfloat diffuse2[]={.2,.6,0.1,1};
	GLfloat shininess2[]={50};
		matprop(ambient2,diffuse2,specular2,shininess2);

	//floor
	glPushMatrix();
	glTranslated(-4,-0.05,-1);
	glScaled(3,3,1.7);
	wall(0.08);
	glPopMatrix();

    glFlush();

}

void fanwing(float winglen)// fan er pakha
{
    glPushMatrix();

    glRotated(90,1,0,0);
    glRotated(90,0,1,0);
    glScaled(1,15,1);
    gluCylinder(Cylinder,.01,.01,1,4,1);
    glPopMatrix();
}

void fanbottom()
{
    glPushMatrix();

    glTranslated(1,3.2,1);
    glPushMatrix();
    glRotated(90,1,0,0);
    gluCylinder(Cylinder, .2, .2,.05, 128, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0,0.00025,0);
    glRotated(90,1,0,0);
    gluDisk(Disk,0,.2,32,16);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0,-.05,0);
    glRotated(90,1,0,0);
    gluCylinder(Cylinder,.2,.15,.1,128,16);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0,-.1,0);
    glRotated(90,1,0,0);
    gluDisk(Disk,0,.15,32,16);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.1,0.0,0);
    fanwing(.6);
    glPopMatrix();
    glPushMatrix();
    glRotated(120,0,1,0);
    glTranslated(.1,0,0);
    fanwing(.6);
    glPopMatrix();
    glPushMatrix();
    glRotated(240,0,1,0);
    glTranslated(0.1,0.0,0);
    fanwing(.6);
    glPopMatrix();
    glPopMatrix();
}

void fan(void)
{
    glPushMatrix();
    glTranslated(2.5,1.9,0);
    glScaled(.5,.5,.5);
    GLfloat mat_ambient[]= {.5,0,0,1};
    GLfloat mat_specular[]= {0,1,1,0};
    GLfloat mat_diffuse[]= {0,.502,0,1};
    GLfloat mat_shininess[]= {50};


    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);


    if(flag==-1)
    {
        glPushMatrix();
        fanbottom();
        glPopMatrix();
    }
    else

    {

        angle+=speed;
        glPushMatrix();
        glTranslated(1,0,1);
        glRotated(angle,0,1,0);
        glTranslated(-1,0,-1);
        fanbottom();
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(1,3.3,1);
    glRotated(-90,1,0,0);
    gluCylinder(Cylinder, .1, 0.005, .25, 16, 16);
    glPopMatrix();
    glPushMatrix();

    glTranslatef(1,4,1);
    glRotated(90,1,0,0);
    gluCylinder(Cylinder, .006, 0.006, .6, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,3.96,1);
    glRotated(90,1,0,0);
    gluCylinder(Cylinder, .1, 0.005, .25, 16, 16);
    glPopMatrix();
    glPopMatrix();
    if(flag==1)
        glutPostRedisplay();
}

void cleg(float cllen,float clwid)
{
    glRotated(90,1,0,0);
    gluCylinder(Cylinder,clwid,clwid,cllen,32,52);

}

void sleg(float len,float thk)
{
    glScaled(thk,len,thk);
    glutSolidCube(1);

}

void solar(void)
{
    GLfloat	ambient1[]= {.1,.1,.1,1};
    GLfloat specular1[]= {1,1,1,1};
    GLfloat diffuse1[]= {1,1,1,1};
    GLfloat mat_shininess[]= {50};

    matprop(ambient1,diffuse1,specular1,mat_shininess);
    GLfloat lightIntensity[]= {.7,.7,.7,1};
    GLfloat light_position[]= {-20,4,60,0};
    glLightfv(GL_LIGHT2,GL_POSITION,light_position);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,lightIntensity);
    glEnable(GL_LIGHT2);
}

void house(void)
{

    GLfloat mat_ambient[]= {0.9,0.1,0.1,1};
    GLfloat mat_specular[]= {1,1,1,1};
    GLfloat mat_diffuse[]= {1,1,1,1};
    GLfloat mat_shininess[]= {50};
    matprop(mat_ambient,mat_diffuse,mat_specular,mat_shininess);

    GLfloat lightIntensity4[]= {.7,.7,.7,.7};
    GLfloat light_position4[]= {3,1,.5,1};
    glLightfv(GL_LIGHT6,GL_POSITION,light_position4);
    glLightfv(GL_LIGHT6,GL_DIFFUSE,lightIntensity4);
    glEnable(GL_LIGHT6);

    GLfloat lightIntensity5[]= {.5,.5,.5,.5};
    GLfloat light_position5[]= {8,10,10,0};
    glLightfv(GL_LIGHT7,GL_POSITION,light_position5);
    glLightfv(GL_LIGHT7,GL_DIFFUSE,lightIntensity5);
    glEnable(GL_LIGHT7);

    GLfloat lightIntensity6[]= {.5,.5,.5,.5};
    GLfloat light_position6[]= {0,10,10,1};
    glLightfv(GL_LIGHT1,GL_POSITION,light_position6);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,lightIntensity6);

    piso();
    gramado();
    right_window();

    // base da casa
	glPushMatrix();
	glTranslated(-.1,-0.02,-.1);
	glScaled(1.532,6,1.033);
	wall(0.08);
	glPopMatrix();

	// base da entrada
	glPushMatrix();
	glTranslated(6,0,3);
	glScaled(-.56,6,.6);
	wall(0.08);
	glPopMatrix();

	// coluna esquerda
    glPushMatrix();
    glTranslated(3.81,0.50,5.39);
    glScaled(0.02,.53,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    // coluna direita
    glPushMatrix();
    glTranslated(5.72,0.50,5.39);
    glScaled(0.02,.53,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    // TRIANGULOS DA LATERAL DA VARANDA
    var1_triangulo();
    var2_triangulo();

    // right window
    glPushMatrix();
    glTranslated(windo1,1,4.06);
    glTranslated(5,0,-4);
    glTranslated(-0.596,0,4);
    glScaled(.25,.51,3);
    glRotated(-90.0,1,0,0);
    wall(0.02);
    glPopMatrix();

    // left window
    glPushMatrix();
    glTranslated(windo2,1,4.06);
    glTranslated(5,0,-4);
    glTranslated(-0.6728,0,4);
    glScaled(.27,.51,3);
    glRotated(-90.0,1,0,0);
    wall(0.02);
    glPopMatrix();

	GLfloat ambient1[]= {1,0.9,0.5,1};
    GLfloat specular1[]= {1,1,1,1};
    GLfloat diffuse1[]= {1, 0.5, 0.8,1};
    GLfloat shininess1[]= {50};
    matprop(ambient1,diffuse1,specular1,shininess1);

    // TETO INFERIOR
    glPushMatrix();
    glTranslated(-.075,3.9,-0.07);
    glScaled(1.515,1.4,1.015);
    wall(0.08);
    glPopMatrix();

    front_Triangulo();
    back_Triangulo();


    //left wall
    glPushMatrix();
    glRotated(90.0,0,0,1);
    wall(0.08);
    glPopMatrix();

    //wall below right windows
    glPushMatrix();
    glTranslated(6,0.4,-0.08);
    glScaled(0.9,0.25,1.02);
    glRotated(90.0,0,0,1);
    wall(0.08);
    glPopMatrix();

    //wall above right windows
    glPushMatrix();
    glTranslated(6,3,-0.08);
    glScaled(0.9,0.25,1.02);
    glRotated(90.0,0,0,1);
    wall(0.08);
    glPopMatrix();

    //wall right of right windows
    glPushMatrix();
    glTranslated(6,1.40,-0.08);
    glScaled(0.9,0.4,0.33);
    glRotated(90.0,0,0,1);
    wall(0.08);
    glPopMatrix();

    //wall left of right windows
    glPushMatrix();
    glTranslated(6,1.40,2.68);
    glScaled(0.9,0.4,0.33);
    glRotated(90.0,0,0,1);
    wall(0.08);
    glPopMatrix();

    //back wall
    glPushMatrix();
    glTranslated(-.08,0,0);
    glScaled(1.5+.02,1,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    GLfloat ambient7[]= {1,0.9,0.5,1};
    GLfloat specular7[]= {1,1,1,1};
    GLfloat diffuse7[]= {1, 0.5, 0.8,1};
    GLfloat shininess7[]= {50};
    matprop(ambient7,diffuse7,specular7,shininess7);

    //wall right to the main door
    glPushMatrix();
    glTranslated(0,0,4);
    glScaled(.22,1,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    //wall right to the window
    glPushMatrix();
    glTranslated(2.8,0,4);
    glScaled(.39,1,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    //wall right to the window
    glPushMatrix();
    glTranslated(5.29,0,4);
    glScaled(.16,1,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();



    //wall below the window
    glPushMatrix();
    glTranslated(0.88,0,4);
    glScaled(.7,.25,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    //wall above the window
    glPushMatrix();
    glTranslated(0.88,3.03,4);
    glScaled(.7,.243,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    //wall above the window
    glPushMatrix();
    glTranslated(4.3,3.03,4);
    glScaled(0.4,.243,1);
    glRotated(-90.0,1,0,0);
    wall(0.08);
    glPopMatrix();

    GLfloat ambient3[]= {0.2,0.2,0.2,1};
    GLfloat specular3[]= {1,1,1,1};
    GLfloat diffuse3[]= {1,0,0,1};
    GLfloat shininess3[]= {50};
    matprop(ambient3,diffuse3,specular3,shininess3);

    // roof right
	glPushMatrix();
	glTranslated(6,4,-0.001);
	glRotated(138,0,0,1);
	wall(0.08);
	glPopMatrix();

	// roof left
	glPushMatrix();
	glTranslated(0,4,0);
	glRotated(40.2,0,0,1);
	wall(0.08);
	glPopMatrix();

	 // varanda
	glPushMatrix();
	glTranslated(3.79,3.92,4.00);
	glScaled(0.504,.485,0.5);
	glRotated(44,1,0,0);
	wall(0.08);
	glPopMatrix();

    fan();
    solar();

    GLfloat mat_ambient4[]= {0.1,0.1,0.1,1};
    GLfloat mat_specular4[]= {1,1,1,1};
    GLfloat mat_diffuse4[]= {1,1,1,1};
    GLfloat mat_shininess4[]= {50};
    matprop(mat_ambient4,mat_diffuse4,mat_specular4,mat_shininess4);


     //PORTA PRINCIPAL
    glPushMatrix();
    glTranslated(5.29,0,3.92);
    glRotated(-90.0,1,0,0);
    glRotated(maino,0,0,1);
    glScaled(.235,.95,0.76);
    wall(0.08);
    glPopMatrix();


//    glPushMatrix();
//    glTranslated(4.39,0,3.98);
//    glScaled(.5,1.008,.2);
//    glRotated(-90,1,0,0);
//    gluCylinder(Cylinder, 0.05, 0.05, 3, 16, 16);
//    glPopMatrix();

    glPopMatrix();
    glFlush();

}

void func_particles(void){
 if (fall == RAIN) {
    drawRain();
    glutSwapBuffers();
  }else if (fall == HAIL) {
    drawHail();
    glutSwapBuffers();
  }else if (fall == SNOW) {
    drawSnow();
    glutSwapBuffers();
  }
}

class Camera
{
public:

    void FRENTE(float d)
    {
        view[2] -= d;
    }

    void TRAS(float d)
    {
        view[2] += d;
    }

    void DIREITA(float d)
    {
        look[0] -= d;
    }

    void ESQUERDA(float d)
    {
        look[0] += d;
    }

    void CIMA(float d)
    {
        look[1] -= d;
    }

    void BAIXO(float d)
    {
        look[1] += d;
    }

};

Camera camera;

void display(void)
{

    time(&ltime); // Get time
    newtime = localtime(&ltime); // Convert to local time
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    gluLookAt(view[0], view[1], view[2], look[0], look[1], look[2], up[0], up[1], up[2]);

    earth();
    compound();
    house();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();


    //int i, j;
    //float x, y, z;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();


  glRotatef(pan, 0.0, 1.0, 0.0);
  glRotatef(tilt, 1.0, 0.0, 0.0);


  // Which Particles
  if(aux_rain == true){
    func_particles();
  }
}

void Keyboard(unsigned char key,int x,int y)
{
    float d = 0.5;

    switch (key)
    {
    case 'w':
        camera.FRENTE(d);
        break;
    case 's':
        camera.TRAS(d);
        break;
    case 'a':
        camera.DIREITA(d);
        break;
    case 'd':
        camera.ESQUERDA(d);
        break;
    case '2':
        camera.BAIXO(d);
        break;
    case '8':
        camera.CIMA(d);
        break;

    //to run and stop the fan
    case 'H':
    case 'h':
        flag*=-1;
        glutPostRedisplay();
        break;
    //to open and close the main door
    case 'p':
    case 'P':
        if (maino == -180.0){
            maino = -270.0;
        }else{
            maino = -180.0;
        }
        break;

	case '3':
		view[0]=2.8;
		view[1]=6;
		view[2]=4.4;
		look[0]=2.8;
		look[1]=4;
		look[2]=1;
	break;

    case '4':
    windo1=windo1+0.9;
    if(windo1>-1.6){
    	windo1=-2.5;
    }
        break;
    case '5':
    windo2=windo2-1.03;
    if(windo2<-4.53){
    	windo2=-3.5;
    }
        break;
    case 'l':
        setLeftView();
        break;

    case 'r':
        setRightView();
        break;

    //top view
    case 'T':
    case 't':
        setTopView();
        break;

    case 'f':
        setFrontView();
        break;
    case 'e':
        aux_rain = false;
        break;
    case 'q':
        aux_rain = true;
        break;
    case 'b':
        setBackView();
        break;
    case 'c': // Rain
        fall = RAIN;
        glutPostRedisplay();
        break;
    case 'n': // Hail
        fall = HAIL;
        glutPostRedisplay();
        break;
    case 'm': // Snow
        fall = SNOW;
        glutPostRedisplay();
        break;     
    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

int main(int argc,char**argv)
{

    printf("**<<Press P for Main door & O for insider door on & off>>**\n");
    printf("**<<Press I for inside view & T for top view >>**\n");
    printf("**<<Press B for back view & F for front view>>**\n");
    printf("**<<Press S for Fan on & off>>**\n");
    printf("**<<Press 1 for go inside slow & 2 for back outside>>**\n");
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1366,768);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Tactile Experience");
    glEnable(GL_DEPTH_TEST);
    myinit();

    glutDisplayFunc(display);
    glutKeyboardFunc(Keyboard);

    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);//smooth shaded
    glEnable(GL_DEPTH_TEST);//to remove hidden surface
    glEnable(GL_NORMALIZE);//to make normal vector to unit vector
    glClearColor(0,.20,.88,0);
    glutMainLoop();
    return 0;
}
