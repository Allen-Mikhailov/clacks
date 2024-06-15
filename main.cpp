#include <stdio.h>
#include <cstdlib> 
#include <glew.h>
#include <glut.h>
#include <time.h>
#include <ctime>
#include <vector> 
#include <float.h>

#define _USE_MATH_DEFINES
#include <cmath>

#define M_PI 3.14159265358979323846

#define TIME_STEP (1/60.0)

#define BLOCK_1_SIZE 0.1
#define BLOCK_2_SIZE 0.3
#define BLOCK_DISTANCE ((BLOCK_1_SIZE+BLOCK_2_SIZE)/2.0)

#define BLOCK_1_MASS 1.0
#define BLOCK_2_MASS 100.0

#define COMBINED_MASS (BLOCK_1_MASS + BLOCK_2_MASS)

#define BLOCK_1_INITIAL_POS 0
#define BLOCK_2_INITIAL_POS 0.5;

#define BLOCK_1_INITIAL_VELOCITY  0.0
#define BLOCK_2_INITIAL_VELOCITY -0.250

#define WALL_POSITION -.5
#define FLOOR_POSITION -.3

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

    float block_1_pos;
    float block_1_velocity;

    float block_2_pos;
    float block_2_velocity;
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
    printf("sim_time %f\n", sim_time);
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

void display() {  // Display function will draw the image.

    glClearColor(0, 0, 0, 1);  // (In fact, this is the default.)
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing Blocks
    glColor3d(BLOCK_COLOR);
    drawSquare(block_1_draw_pos, FLOOR_POSITION + BLOCK_1_SIZE / 2, BLOCK_1_SIZE);
    drawSquare(block_2_draw_pos, FLOOR_POSITION + BLOCK_2_SIZE / 2, BLOCK_2_SIZE);

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

void get_next_collision(struct collision * col)
{
    double wall_time = DBL_MAX;
    if (block_1_velocity < 0)
        wall_time = (block_1_pos - WALL_POSITION - BLOCK_1_SIZE / 2) / -block_1_velocity;

    const double block_distance = block_2_pos - block_1_pos - BLOCK_DISTANCE; // block 2 will always be to the right of block 1

    double collide_time = block_distance / (block_1_velocity - block_2_velocity);

    const double block_1_sign = sgn(block_1_velocity);
    const double block_2_sign = sgn(block_2_velocity);

    if (collide_time <= 0.00001)
        collide_time = DBL_MAX;
    //if (block_1_velocity >= 0 && )

    const double collision_time = (wall_time > collide_time) ? collide_time : wall_time;

    block_1_pos += collision_time * block_1_velocity;
    block_2_pos += collision_time * block_2_velocity;

    if (wall_time < collide_time) {
        printf("Wall Collision: ");
        col->type = 0;
        wall_collision();
    }
    else {
        printf("Block Collision: ");
        col->type = 1;
        block_collision();
    }

    printf("Block Distance %f, collision_time %f, \nblock_1_pos %f, block_2_pos %f, block_1_vel %f, block_2_vel %f\n", 
        block_distance, collision_time, block_1_pos, block_2_pos, block_1_velocity, block_2_velocity);
    col->block_1_pos = block_1_pos;
    col->block_2_pos = block_2_pos;
    col->block_1_velocity = block_1_velocity;
    col->block_2_velocity = block_2_velocity;
    col->time = collision_time;
}

#define MAX_COLLISIONS 100000

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

    while (!(block_2_velocity >= block_1_velocity && block_1_velocity > 0) && i < MAX_COLLISIONS)
    {
        struct collision col;
        get_next_collision(&col);
        collision_count++;

        _time += col.time;
        col.sim_time = _time;

        collisions.push_back(col);

        printf("Collision at time %f after %f\n\n", _time, col.time);
        i++;
    }

    printf("There were %d total collisions\n", i);
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
