#include <stdio.h>
#include <cstdlib> 
#include <glew.h>
#include <glut.h>
#include <time.h>
#include <ctime>
#include <vector> 
#include <float.h>
#include <format>

#define _USE_MATH_DEFINES
#include <cmath>

#define TIME_STEP (1/60.0)

#define BLOCK_1_SIZE .10
#define BLOCK_2_SIZE .30
const double BLOCK_DISTANCE = ((BLOCK_1_SIZE + BLOCK_2_SIZE) / 2.0);

#define DIGITS 3

#define BLOCK_1_MASS 1.0
const double BLOCK_2_MASS = pow(100, DIGITS-1);

const double COMBINED_MASS = (BLOCK_1_MASS + BLOCK_2_MASS);

#define BLOCK_1_INITIAL_POS 0.0
#define BLOCK_2_INITIAL_POS .50;

#define BLOCK_1_INITIAL_VELOCITY  0.0
#define BLOCK_2_INITIAL_VELOCITY -.250

#define WALL_POSITION -.50
#define FLOOR_POSITION -.30

#define BLOCK_COLOR 1, 1, 1
#define FLOOR_COLOR .8, .8, .8

double block_1_pos = BLOCK_1_INITIAL_POS;
double block_1_velocity = BLOCK_1_INITIAL_VELOCITY;

double block_2_pos = BLOCK_2_INITIAL_POS;
double block_2_velocity = BLOCK_2_INITIAL_VELOCITY;

double block_1_draw_pos;
double block_2_draw_pos;

double sim_time = 0;
double last_tick;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

struct collision
{
    double time;
    double sim_time;
    int type; // 0 is wall, 1 is blocks

    double block_1_pos;
    double block_1_velocity;

    double block_2_pos;
    double block_2_velocity;
};

std::vector<collision> collisions;
int collision_count = 0;


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

void drawRect(float left, float top, float right, float bottom)
{
    glBegin(GL_POLYGON);

    glVertex2d(right, top);
    glVertex2d(right, bottom);
    glVertex2d(left, bottom);
    glVertex2d(left, top);

    glEnd();
}

int last_collision = 0;
void timer(int t) {

    double c = clock();
    sim_time += (double) (c - last_tick) / CLOCKS_PER_SEC;
    //printf("sim_time %f\n", sim_time);
    last_tick = c;
    while (last_collision +1 < collision_count+1 && sim_time >= collisions[last_collision +1].sim_time)
    {
        last_collision++;
    }

    double time_dif = sim_time - collisions[last_collision].sim_time;

    block_1_draw_pos = collisions[last_collision].block_1_pos + collisions[last_collision].block_1_velocity * time_dif;
    block_2_draw_pos = collisions[last_collision].block_2_pos + collisions[last_collision].block_2_velocity * time_dif;

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

#define STRING_TEMPLATE "Collisions %d"
void display() {  // Display function will draw the image.

    glClearColor(0, 0, 0, 1);  // (In fact, this is the default.)
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing Blocks
    glColor3d(BLOCK_COLOR);
    drawSquare(block_1_draw_pos, FLOOR_POSITION + BLOCK_1_SIZE / 2, BLOCK_1_SIZE);
    drawSquare(block_2_draw_pos, FLOOR_POSITION + BLOCK_2_SIZE / 2, BLOCK_2_SIZE);

    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(-.5, .1); //define position on the screen

    int string_size = snprintf(NULL, 0, STRING_TEMPLATE, last_collision)+1;
    char* str = (char*)malloc(sizeof(char) * string_size);
    snprintf(str, string_size, STRING_TEMPLATE, last_collision);
    char* string_ptr = str;

    while (*str) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str++);
    }

    glColor3d(FLOOR_COLOR);
    drawRect(-1, FLOOR_POSITION, 1, -1); // Floor
    drawRect(-1, 1, WALL_POSITION, FLOOR_POSITION);
    

    glutSwapBuffers(); // Required to copy color buffer onto the screen.

}

void block_collision()
{
    const double new_block_1_velocity = (BLOCK_1_MASS - BLOCK_2_MASS) / COMBINED_MASS * block_1_velocity
        + 2 * BLOCK_2_MASS / COMBINED_MASS * block_2_velocity;
    const double new_block_2_velocity = (BLOCK_2_MASS - BLOCK_1_MASS) / COMBINED_MASS * block_2_velocity
        + 2 * BLOCK_1_MASS / COMBINED_MASS * block_1_velocity;

    block_1_velocity = new_block_1_velocity;
    block_2_velocity = new_block_2_velocity;
}

void wall_collision()
{
    block_1_pos = WALL_POSITION + (BLOCK_1_SIZE / 2);
    block_1_velocity = abs(block_1_velocity);
}

const int MAX_COLLISIONS = pow(10, DIGITS);

void fill_collision(struct collision *col)
{
    col->block_1_pos = block_1_pos;
    col->block_2_pos = block_2_pos;
    col->block_1_velocity = block_1_velocity;
    col->block_2_velocity = block_2_velocity;
}

void calculate_collisions()
{
    double _time = 0;
    int i = 0;

    struct collision initial_col;
    initial_col.time = 0;
    initial_col.sim_time = 0;
    initial_col.block_1_pos = BLOCK_1_INITIAL_POS;
    initial_col.block_2_pos = BLOCK_2_INITIAL_POS;
    initial_col.block_1_velocity = BLOCK_1_INITIAL_VELOCITY;
    initial_col.block_2_velocity = BLOCK_2_INITIAL_VELOCITY;
    initial_col.type = -1;
    collisions.push_back(initial_col);

    while (i < MAX_COLLISIONS)
    {
        struct collision block_col;
        block_col.type == 1;

        const double block_distance = block_2_pos - block_1_pos - BLOCK_DISTANCE; // block 2 will always be to the right of block 1

        double collide_time = block_distance / (block_1_velocity - block_2_velocity);

        block_1_pos += collide_time * block_1_velocity;
        block_2_pos += collide_time * block_2_velocity;

        block_collision();
        fill_collision(&block_col);
        block_col.time = collide_time;
        _time += block_col.time;
        block_col.sim_time = _time;

        collisions.push_back(block_col);
        
        collision_count++;

        if (block_1_velocity >= 0)
            break;

        struct collision *wall_col = (struct collision *) malloc(sizeof(collision));
        wall_col->type == 0;
        const double wall_time = (block_1_pos - WALL_POSITION - BLOCK_1_SIZE / 2) / -block_1_velocity;
        wall_collision();
        block_2_pos += wall_time * block_2_velocity;

        fill_collision(wall_col);
        wall_col->time = wall_time;
        _time += wall_col->time;
        wall_col->sim_time = _time;

        collisions.push_back(*wall_col);
        collision_count++;

        if (block_2_velocity > block_1_velocity)
            break;


        //printf("Collision at time %f after %f\n\n", _time, col.time);
        i++;
    }

    printf("There were %d total collisions\n", collision_count);
}

int main(int argc, char** argv) {  // Initialize GLUT and 

    calculate_collisions();

    last_tick = clock();

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
