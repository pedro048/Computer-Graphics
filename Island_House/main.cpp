//#include<windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "imageloader.h"
#include "imageloader.cpp"
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

// PAR�METROS DO LOOKAT
double view[3]= {2,2,12.9};
double look[3]= {2,2,2};
double up[3] = {0.0, 1.0, 0.0};
int flag=-1;

double angle=0,speed=5,maino=-180.0,windo1=-2.5,windo2=-3.5,tro=0,romo=0,mgo=0;
GLuint  _textureHouse, _textureJanela, _textureGrama, _texturePiso;

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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0,1.0,-1*1366/768,1*1366/768,1,200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Image* image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Island_House/Img/basecase2.bmp");
	_textureGrama = loadTexture(image);
	image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Island_House/Img/janela1.bmp");
	_textureJanela = loadTexture(image);
	image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Island_House/Img/sky.bmp");
	_textureHouse = loadTexture(image);
	image = loadBMP("/home/pedro/Documentos/Mestrado/QoE Testes/Códigos - OpenGL/Computer-Graphics/Island_House/Img/piso2.bmp");
	_texturePiso = loadTexture(image);
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
    glBindTexture(GL_TEXTURE_2D, _textureHouse);
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
    glBindTexture(GL_TEXTURE_2D, _textureJanela);
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
    
    GLfloat ambient[]= {1,0,4,0};
    GLfloat specular[]= {0,1,1,1};
    GLfloat diffuse[]= {.2,.9,.5,1};
    GLfloat shininess[]= {50};
    matprop(ambient,diffuse,specular,shininess);

    GLfloat lightIntensity[]= {.7,.7,.7,1};
    GLfloat light_position[]= {2,5,-3,0};
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightIntensity);
    
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslated(0,-.25,0);
    glScaled(500,.5,500);
    glutSolidCube(1.0);
    glPopMatrix();
    glFlush();
}

void ilha(void)
{
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _textureGrama);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTranslated(-5,-0.99,15);
    glScaled(10,1,10);
    glRotated(90.0,0,1,0);
    glRotated(-90.0,1,0,0);
    glBegin(GL_QUADS);
            glTexCoord2f(1,0);  glVertex3f(0.56,0,1); //1
            glTexCoord2f(2,0); glVertex3f(2,0,1); //2
            glTexCoord2f(2,-1); glVertex3f(2,-1.6,1); //3
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
    glEnable(GL_LIGHT1);

    piso();
    ilha();
    right_window();


    // base da casa
	glPushMatrix();
	glTranslated(-.1,-0.02,-.1);
	glScaled(1.532,6,1.033);
	wall(0.08);
	glPopMatrix();

	// base da entrada
	glPushMatrix();
	glTranslated(3.8,0,4);
	glScaled(.56,6,.36);
	wall(0.08);
	glPopMatrix();

	// escada da base de entrada
	glPushMatrix();
	glTranslated(3.8,0,5.4);
	glScaled(.56,3,.05);
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

    glPopMatrix();
    glFlush();

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
    house();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();

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

    case 'b':
        setBackView();
        break;

    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

int main(int argc,char**argv)
{
    printf("**<<Press G for Gate on & off>>**\n");
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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);//smooth shaded
    glEnable(GL_DEPTH_TEST);//to remove hidden surface
    glEnable(GL_NORMALIZE);//to make normal vector to unit vector
    glClearColor(0,.20,.88,0);
    glutMainLoop();
    return 0;
}
