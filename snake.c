#include <stdlib.h>
#include <GL/glut.h>

typedef struct {
    float x;
    float y;
    float z;
} Position;

typedef struct  {
    float parameter_x;
    float parameter_y;
    float parameter_z;

} Parameter;

#define TIMER_INTERVAL 15
#define TIMER_ID 0

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

static int window_width, window_height;

static Parameter *parameters;
static int is_animation_going = 0;
static float speed = 0.01;
static float size = 0.1;


static int current_direction = LEFT;
static int previous_direction = LEFT;

static Position *positions_of_snake;
static Position *positions_of_head;

static int indicator_of_position_head = 0;

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_special(int key, int x, int y);

static void drawWall();
static void view();
static void light_scene();
static void draw_snake();

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("zmijica");

    parameters = malloc(50*sizeof(Parameter));
    if (parameters == NULL) {
        return;
    }

    parameters[0].parameter_x = 0;
    parameters[0].parameter_y = 0;
    parameters[0].parameter_z = 0;

    positions_of_snake = malloc(50*sizeof(Position));
    positions_of_head = malloc(50 * sizeof(Position));

    glutReshapeFunc(on_reshape);
    glutSpecialFunc(on_special);
    glutKeyboardFunc(on_keyboard);
    
    glutDisplayFunc(on_display);
    

    glClearColor(0, 0.58, 0, 0);
    glEnable(GL_DEPTH_TEST);
    
    glLineWidth(2);

    glutMainLoop();

	return 0;
}

static void view() {
    glViewport(0, 0, window_width, window_height);

    /*podesava se projekcija */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, window_width/(float)window_height, 1, 25);

    /*podesava se tacka pogleda */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 4, 0, 0, 0, 0, 1, 0, 0);

    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(0,0,0);
        glVertex3f(10,0,0);
        
        glColor3f(0,1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,10,0);
        
        glColor3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(0,0,10);
    glEnd();

}

static void drawWall() {

    /*zid se sastoji od 4 pravougaonika */

   
    /*leva strana zida */
    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(0, 0, -3);
    glScalef(4, 0.2, 0.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*desna strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(0, 0, 3);
    glScalef(4, 0.2, 0.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*gornja strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(2, 0, 0);
    glScalef(0.2, 0.2, 6.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*donja strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(-2, 0, 0);
    glScalef(0.2, 0.2, 6.2);
    
    glutSolidCube(1);

    glPopMatrix();


    
    
}

static void on_reshape(int width, int height) {
    window_width = width;
    window_height = height;
}

static void on_special(int key, int x, int y) {

    int num = sizeof(positions_of_head) / sizeof(Position);

    switch(key) {
        case 101:
            previous_direction = current_direction;
            current_direction = UP;
            positions_of_head[num].x = positions_of_snake[0].x;
            positions_of_head[num].y = positions_of_snake[0].y;
            positions_of_head[num].z = positions_of_snake[0].z;

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        
        case 103:
            previous_direction = current_direction;
            current_direction = DOWN;
            positions_of_head[num].x = positions_of_snake[0].x;
            positions_of_head[num].y = positions_of_snake[0].y;
            positions_of_head[num].z = positions_of_snake[0].z;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 100:
            previous_direction = current_direction;
            current_direction = LEFT;
            positions_of_head[num].x = positions_of_snake[0].x;
            positions_of_head[num].y = positions_of_snake[0].y;
            positions_of_head[num].z = positions_of_snake[0].z;

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 102:
            previous_direction = current_direction;
            current_direction = RIGHT;
            positions_of_head[num].x = positions_of_snake[0].x;
            positions_of_head[num].y = positions_of_snake[0].y;
            positions_of_head[num].z = positions_of_snake[0].z;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;

    }
    glutPostRedisplay();

}

static void on_timer(int value) {
    if (value != 0) {
        return;
    }

    switch (current_direction)
    {
        case UP:
            parameters[0].parameter_x += speed;
            switch (previous_direction) {
                case LEFT:

                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                       
                        
                        parameters[1].parameter_x += speed;
                        positions_of_snake[0].x += speed;
                        positions_of_snake[1].x += speed;
                        indicator_of_position_head++;
                    }
                    else {
                        positions_of_snake[0].x += speed;
                        positions_of_snake[1].z -= speed;
                        parameters[1].parameter_z -= speed;
                    }

                    
                    break;
                case RIGHT:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_x += speed;
                        positions_of_snake[0].x += speed;
                        positions_of_snake[1].x += speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_z += speed;
                        positions_of_snake[0].x += speed;
                        positions_of_snake[1].z += speed;
                    }
                    break;
            }
            break;
            
        case DOWN:
            parameters[0].parameter_x -= speed;
            switch (previous_direction) {
                case LEFT:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_x -= speed;
                        positions_of_snake[0].x -= speed;
                        positions_of_snake[1].x -= speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_z -= speed;
                        positions_of_snake[0].x -= speed;
                        positions_of_snake[1].z -= speed;
                    }
                    break;
                case RIGHT:

                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_x -= speed;
                        positions_of_snake[0].x -= speed;
                        positions_of_snake[1].x -= speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_z += speed;
                        positions_of_snake[0].x -= speed;
                        positions_of_snake[1].z += speed;
                    }

                    
                    break;
            }
            break;
        case LEFT:
            parameters[0].parameter_z -= speed;
            switch (previous_direction) {
                case UP:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_z -= speed;
                        positions_of_snake[0].z -= speed;
                        positions_of_snake[1].z -= speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_x += speed;
                        positions_of_snake[0].z -= speed;
                        positions_of_snake[1].x += speed;

                    }
                    break;
                case DOWN:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_z -= speed;
                        positions_of_snake[0].z -= speed;
                        positions_of_snake[1].z -= speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_x -= speed;
                        positions_of_snake[0].z -= speed;
                        positions_of_snake[0].x -= speed;
                    }
                    break;
            }
            break;
        case RIGHT:
            parameters[0].parameter_z += speed;
            switch (previous_direction) {
                case UP:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_z += speed;
                        positions_of_snake[0].z += speed;
                        positions_of_snake[1].z += speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_x += speed;
                        positions_of_snake[0].z += speed;
                        positions_of_snake[0].x += speed;
                    }
                    break;
                case DOWN:
                    if (positions_of_snake[1].x == positions_of_head[indicator_of_position_head].x &&
                        positions_of_snake[1].y == positions_of_head[indicator_of_position_head].y &&
                        positions_of_snake[1].z == positions_of_head[indicator_of_position_head].z) {
                        parameters[1].parameter_z += speed;
                        positions_of_snake[0].z += speed;
                        positions_of_snake[1].z += speed;
                        indicator_of_position_head++;
                    }
                    else {
                        parameters[1].parameter_x -= speed;
                        positions_of_snake[0].z += speed;
                        positions_of_snake[1].x -= speed;
                    }
                    break;
            }
            break;
    }

    glutPostRedisplay();

    if (is_animation_going) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'a':
            previous_direction = current_direction;
            current_direction = LEFT;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;

        case 'w':
            previous_direction = current_direction;
            current_direction = UP;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 'd':
            previous_direction = current_direction;
            current_direction = RIGHT;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 'x':
            previous_direction = current_direction;
            current_direction = DOWN;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 's':
            is_animation_going = 0;
            break;

    }
}

static void light_scene() {

    /*pozicija svetla*/
    GLfloat position[] = {0, 4, 0, 0};

    /*ambijentalna boja svetla */
    GLfloat ambient[] = {1, 0, 0, 1};

    /*difuzna boja svetla */
    GLfloat diffuse[] = {0.7, 0, 0, 1};

    /*spekularna boja svetla */
    GLfloat specular[] = {0.1, 0.1, 0.1, 1};

    /*ambijentalna refleksija materijala */
    GLfloat a[] = {0.6, 0.1, 0.1, 0.1, 1};

    /*difuzna refleksija materijala */
    GLfloat d[] = {0.8, 0, 0.8, 1};

    /*spekularna refleksija materijala */
    GLfloat s[] = {0.7, 0, 0, 1};

    /*odsjaj */

    GLfloat shininess = 10;

    /*podesavaju se parametri svetla */

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    /*podesavaju se parametri materijala */
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);


    glEnable(GL_LIGHT0);
}

static void draw_snake() {

    

    glColor3f(0, 0, 1);

    glPushMatrix();



    glTranslatef(
        parameters[0].parameter_x,
        parameters[0].parameter_y,
        parameters[0].parameter_z
    );
    positions_of_snake[0].x = parameters[0].parameter_x;
    positions_of_snake[0].y = parameters[0].parameter_y;
    positions_of_snake[0].z = parameters[0].parameter_z;

    glutSolidCube(2*size);


    glPopMatrix();
/*
    glPushMatrix();
    glTranslatef(
        parameters[1].parameter_x,
        parameters[1].parameter_y,
        2*size + parameters[1].parameter_z
    );

    positions_of_snake[1].x = parameters[1].parameter_x;
    positions_of_snake[1].y = parameters[1].parameter_y;
    positions_of_snake[1].z = 2*size +  parameters[1].parameter_z;

    glutSolidSphere(size, 20, 20);


    glPopMatrix();

    */

}


static void on_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    view();
    
    
    drawWall();

    draw_snake();
    

    glutSwapBuffers();

}
