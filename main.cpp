#include <stdio.h>
#include <cstdlib> 
#include <glew.h>
#include <glut.h>
#include <time.h>
#include <ctime>
#include <vector> 

#define _USE_MATH_DEFINES
#include <cmath>

#define M_PI 3.14159265358979323846

#define TIME_STEP (1/60.0)

#define BLOCK_1_SIZE 0.3
#define BLOCK_2_SIZE 0.3

#define BLOCK_1_MASS 1
#define BLOCK_2_MASS 100

#define BLOCK_1_INITIAL_VELOCITY  0.0
#define BLOCK_2_INITIAL_VELOCITY -1.0

#define WALL_POSITION -.5
#define FLOOR_POSITION -.3

float block_1_pos = 0;
float block_1_velocity = BLOCK_1_INITIAL_VELOCITY;

float block_2_pos = 0.5;
float block_2_velocity = BLOCK_2_INITIAL_VELOCITY;



struct collision
{
    float time;

    float block_1_pos;
    float block_1_velocity;

    float block_2_pos;
    float block_2_velocity;
};

std::vector<collision> collisions;


void drawSquare(float x1, float y1, float sidelength)
{
    float halfside = sidelength / 2;

    glBegin(GL_POLYGON);

    glVertex2d(x1 + halfside, y1 + halfside);
    glVertex2d(x1 + halfside, y1 - halfside);
    glVertex2d(x1 - halfside, y1 - halfside);
    glVertex2d(x1 - halfside, y1 + halfside);

    glEnd();
}

void timer(int t) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void display() {  // Display function will draw the image.

    glClearColor(0, 0, 0, 1);  // (In fact, this is the default.)
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing Block 1
    glColor3d(1, 1, 1);
    drawSquare(block_1_pos, FLOOR_POSITION + BLOCK_1_SIZE / 2, BLOCK_1_SIZE);
    

    glutSwapBuffers(); // Required to copy color buffer onto the screen.

}

void collide()
{

}

void calculate_collisions()
{

}

int main(int argc, char** argv) {  // Initialize GLUT and 

    

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);    // Use single color buffer and no depth buffer.
    glutInitWindowSize(700, 700);         // Size of display area, in pixels.
    glutInitWindowPosition(100, 100);     // Location of window in screen coordinates.
    glutCreateWindow("Clacks"); // Parameter is window title.
    glutDisplayFunc(display);            // Called when the window needs to be redrawn.

    timer(0);

    glutMainLoop(); // Run the event loop!  This function does not return.
    // Program ends when user closes the window.

    
    return 0;

}
