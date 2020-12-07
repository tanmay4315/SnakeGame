#include <deque>
#include <GL/glut.h>
#include <sstream>
#include<time.h>
#include<fstream>
#include<string.h>
#include<iostream>
// A macro for unused variables (to bypass those pesky G++ warnings)
#define UNUSED(param) (void)(param)

// Snake direction macros
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

const char title[] = "OpenGL Snake";

const float map_half_length = 30.0f;
int level=0;
int direction = DOWN;
bool moved = false;
int move_speed = 200;
std::deque< std::deque<float> > part_coords;

bool food_available = false;
int food_coords[2];
int score = 0;
int high_score;
int growth_stage = 0;
const int growth = 1;
void drawFood(){
    glLoadIdentity();
    glTranslatef(food_coords[0], food_coords[1], -40.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2d( 1.0f,  1.0f);
        glVertex2d( 1.0f, -1.0f);
        glVertex2d(-1.0f, -1.0f);
        glVertex2d(-1.0f,  1.0f);
    glEnd();
}
void filewrite()
{
    FILE *filepointer;
            filepointer=fopen("score.txt","w");
            fprintf(filepointer,"%d",high_score);
            fclose(filepointer);
}
void fileread()
{
    FILE *filepointer;
    int hs;
    filepointer=fopen("score.txt","r");
    if (filepointer !=NULL)
    {
    fscanf(filepointer,"%d",&hs);
    high_score=hs;
    }
    else
    {
            high_score=0;
    }
    fclose(filepointer);
}
void spawnFood(){
    if(food_available){
        return;
    }
    int temp_food_coords[2];
    bool collides;
    bool top_limit;
    do {
        collides = false;
        top_limit= false;

        // Produce a temporary random coordinate
        temp_food_coords[0] = 2 * (rand() % ((int) map_half_length + 1)) - (int) map_half_length;
        temp_food_coords[1] = 2 * (rand() % ((int) map_half_length + 1)) - (int) map_half_length;
         if(temp_food_coords[1]<=24.0f)
         {
             top_limit=true;
         }

        // Does it collide with the snake?
        for(unsigned int a = 0; a < part_coords.size(); a++){
            if(temp_food_coords[0] == part_coords[a][0] &&
               temp_food_coords[1] == part_coords[a][1]){
                collides = true;
            }
        }
    } while(collides && top_limit);

    food_coords[0] = temp_food_coords[0];
    food_coords[1] = temp_food_coords[1];

    food_available = true;
}
void drawBitmapText(char* text, float x, float y, float z) {
    char* c;

    glLoadIdentity();
    glColor3f(0.4f, 0.7f, 0.6f);
    glRasterPos3f(x, y, z);

    for(c = text; *c != '\0'; c++){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}
void drawControls(){
    char* move_message = (char*) "Use WASD or the arrow keys to move";
    char* exit_message = (char*) "Press ESC or Q to exit";

    drawBitmapText(move_message, map_half_length - 32.0f, map_half_length - 2.0f, -39.5f);
    drawBitmapText(exit_message, map_half_length - 21.0f, map_half_length - 4.0f, -39.5f);
}
void drawScore(){
    const char* score_message = (char*) "Score: ";
    const char* length_message = (char*) "Length: ";
    const char* high_message= (char*) "High Score:";
    const char* level_message = (char*) "Level:";

    std::stringstream score_string;
    std::stringstream length_string;
    std::stringstream high_string;
    std::stringstream level_string;

    score_string << score_message << score;
    length_string << length_message << part_coords.size();
    high_string << high_message << high_score;
    level_string<< level_message << level;

    drawBitmapText((char*) score_string.str().c_str(), -map_half_length + 1.0f, map_half_length - 2.0f, -39.5f);
    drawBitmapText((char*) length_string.str().c_str(), -map_half_length + 1.0f, map_half_length - 4.0f, -39.5f);
    drawBitmapText((char*) high_string.str().c_str(), -map_half_length + 1.0f, map_half_length - 6.0f, -39.5f);
    drawBitmapText((char*) level_string.str().c_str(), map_half_length - 21.0f,map_half_length-6.0f,-39.5f);
}
void drawSnake(){
     // Loop over snake size and draw each part at it's respective coordinates
    for(unsigned int a = 0; a < part_coords.size(); a++){
        glLoadIdentity();
        glTranslatef(part_coords[a][0], part_coords[a][1], -40.0f);
        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_POLYGON);
            glVertex2d( 1.0f,  1.0f);
            glVertex2d( 1.0f, -1.0f);
            glVertex2d(-1.0f, -1.0f);
            glVertex2d(-1.0f,  1.0f);
        glEnd();
    }
}
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    drawSnake();

    spawnFood();
    drawFood();

    drawScore();
    drawControls();

    glutSwapBuffers();
}
void moveSnake(int new_direction){
    direction = new_direction;

    int last_part = part_coords.size() - 1;
    std::deque<float> new_head = part_coords[last_part];

    float delta_x = 0.0f;
    float delta_y = 0.0f;

    float map_edge = 0.0f;

    int snake_part_axis = 0;

    switch (direction){
        case UP:{
            delta_y = 2.0f;
            map_edge = map_half_length-6.0f;
            snake_part_axis = 1;

            break;
        }

        case DOWN:{
            delta_y = -2.0f;
            map_edge = -map_half_length;
            snake_part_axis = 1;

            break;
        }

        case RIGHT:{
            delta_x = 2.0f;
            map_edge = map_half_length;
            snake_part_axis = 0;

            break;
        }

        case LEFT:{
            delta_x = -2.0f;
            map_edge = -map_half_length;
            snake_part_axis = 0;

            break;
        }
    }
    for(unsigned int a = 0; a < part_coords.size(); a++){
        if(part_coords[0][0] + delta_x == part_coords[a][0] &&
           part_coords[0][1] + delta_y == part_coords[a][1]){
            exit(0);
        }
    }
    if(part_coords[0][snake_part_axis] == map_edge)
        {
            filewrite();
        exit(0);
    }
    if(part_coords[0][0] + delta_x == food_coords[0] &&
       part_coords[0][1] + delta_y == food_coords[1]){
        score += 10;
        if (high_score<score)
        {
            high_score=score;
        }
        if(score%9==0)
        {
            move_speed=move_speed-10;
            level++;
        }
        growth_stage++;
        food_available = false;
    }

    new_head[0] = part_coords[0][0] + delta_x;
    new_head[1] = part_coords[0][1] + delta_y;

    part_coords.push_front(new_head);

    if(!growth_stage){
        part_coords.pop_back();
    } else if(growth_stage == growth){
        growth_stage = 0;
    } else {
        growth_stage++;
    }

    glutPostRedisplay();
}
void moveSnakeAuto(int value){
    if(!moved){
        UNUSED(value);

        moveSnake(direction);
    } else {
        moved = false;
    }

    glutTimerFunc(move_speed, moveSnakeAuto, 0);
}
void initGL(){
    glMatrixMode(GL_PROJECTION);
    gluPerspective(76.0f, 1.0f, 0.0f, 40.0f);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}
void keyboard(unsigned char key, int x, int y){
    UNUSED(x);
    UNUSED(y);

    switch(key){
        case 'w':{
            if(direction == LEFT || direction == RIGHT){
                moved = true;

                moveSnake(UP);
            }

            break;
        }

        case 's':{
            if(direction == LEFT || direction == RIGHT){
                moved = true;

                moveSnake(DOWN);
            }

            break;
        }

        case 'a':{
            if(direction == UP || direction == DOWN){
                moved = true;

                moveSnake(LEFT);
            }

            break;
        }

        case 'd':{
            if(direction == UP || direction == DOWN){
                moved = true;

                moveSnake(RIGHT);
            }

            break;
        }

        // Escape key
        case 27:
        case 'q':
            {
                filewrite();
            exit(0);
        }
    }
}
void special(int key, int x, int y){
    UNUSED(x);
    UNUSED(y);

    switch(key){
        case GLUT_KEY_UP:{
            if(direction == LEFT || direction == RIGHT){
                moved = true;

                moveSnake(UP);
            }

            break;
        }

        case GLUT_KEY_DOWN:{
            if(direction == LEFT || direction == RIGHT){
                moved = true;

                moveSnake(DOWN);
            }

            break;
        }

        case GLUT_KEY_LEFT:{
            if(direction == UP || direction == DOWN){
                moved = true;

                moveSnake(LEFT);
            }

            break;
        }

        case GLUT_KEY_RIGHT:{
            if(direction == UP || direction == DOWN){
                moved = true;

                moveSnake(RIGHT);
            }
        }
    }
}
void reshape(GLsizei width, GLsizei height){
    UNUSED(width);
    UNUSED(height);

    // Make the window non-resizable so we don't have to worry about size changes
    glutReshapeWindow(600, 600);
}
int main(int argc, char** argv)
{
    fileread();
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutCreateWindow(title);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutTimerFunc(move_speed, moveSnakeAuto, 0);

    const int initSize = 3;

    // Specify the coordinates to each part of the snake
    for(int a = 1; a <= initSize; a++){
        std::deque<float> row;

        row.push_back(0.0f);
        row.push_back((map_half_length + 2.0f + (initSize * 2)) - (a * 2));

        part_coords.push_front(row);
    }

    srand(time(NULL));

    initGL();
    glutMainLoop();

    return 0;
}
