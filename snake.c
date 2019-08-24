#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>

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

static indicator_draw_food = 0;

static int window_width, window_height;

static Parameter *parameters;
static int is_animation_going = 0;
static float speed = 0.012;
static float size = 0.1;


static int current_direction = LEFT;
static int previous_direction = LEFT;

static Position *positions_of_snake;

static Position *positions_of_food;
static int pos = 0;


static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_special(int key, int x, int y);

static void drawWall();
static void view();
static void light_scene();
static void draw_snake();
static void draw_food(int pos);
static void game_over();
static void check_is_food_eaten(int i);


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

    /*parametri pravca i smera kretanja zmije */

    parameters[0].parameter_x = 0;
    parameters[0].parameter_y = 0;
    parameters[0].parameter_z = 0;

    

    positions_of_snake = malloc(50*sizeof(Position));

    positions_of_food = malloc(15 * sizeof(Position));

    int i;

    srand(time(NULL));

    /*generisemo random pozicije hrane */

    for (i = 0; i < 15; i++) {
        positions_of_food[i].x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        positions_of_food[i].y = 0;
        positions_of_food[i].z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);

    }

    glShadeModel(GL_SMOOTH);
  

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

    

    switch(key) {
        case 101:
            previous_direction = current_direction;
            current_direction = UP;
            

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        
        case 103:
            previous_direction = current_direction;
            current_direction = DOWN;
            
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 100:
            previous_direction = current_direction;
            current_direction = LEFT;
            

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 102:
            previous_direction = current_direction;
            current_direction = RIGHT;
           
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

    if (positions_of_snake[0].x >= 1.8 ||
        positions_of_snake[0].x <= -1.8 ||
        positions_of_snake[0].z >= 2.8 || 
        positions_of_snake[0].z <= -2.8) {
        
        is_animation_going = 0;

        game_over();
    }

    check_is_food_eaten(pos-1);

    switch (current_direction)
    {
        case UP:
            parameters[0].parameter_x += speed;


            break;
            
        case DOWN:
            parameters[0].parameter_x -= speed;
            
            break;
        case LEFT:
            parameters[0].parameter_z -= speed;
            
            break;
        case RIGHT:
            parameters[0].parameter_z += speed;
           
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

static void draw_food(int pos) {
    glColor3f(0.5, 0, 0.5);

    glPushMatrix();

    glTranslatef(
        positions_of_food[pos].x,
        positions_of_food[pos].y,
        positions_of_food[pos].z
    );

    

    glutSolidSphere(0.07, 20, 20);

    

    glPopMatrix();

    
}

static void game_over() {
    /*treba da se ispise da je igra gotova i poeni */



    /*brzina se postavlja na nulu, sto znaci je sada onemoguceno kretanje zmije  */
    speed = 0;


}

static void check_is_food_eaten(int i){

    /*provera za prvi kvadrant, gde su x i z pozitivni, y je 0 */
    float first = 10;
    float second = 10;

    if (positions_of_snake[0].x >= 0 && positions_of_food[i].x >= 0) {
        if (positions_of_snake[0].y >= 0 && positions_of_food[i].z >= 0) {
            first = positions_of_snake[0].x - positions_of_food[i].x;
            second = positions_of_snake[0].z - positions_of_food[i].z;
        }
    
    }

    /*provera za drugi */

    if (positions_of_snake[0].x >= 0 && positions_of_food[i].x >= 0) {
        if (positions_of_snake[0].y <= 0 && positions_of_food[i].z <= 0) {
            first = positions_of_snake[0].x - positions_of_food[i].x;
            second = positions_of_snake[0].z - positions_of_food[i].z;
        }
    }

    /*treci */

    if (positions_of_snake[0].x <= 0 && positions_of_food[i].x <= 0) {
        if (positions_of_snake[0].y <= 0 && positions_of_food[i].z <= 0) {
            first = positions_of_snake[0].x - positions_of_food[i].x;
            second = positions_of_snake[0].z - positions_of_food[i].z;
        }
    }

    /*cetvrti */
    if (positions_of_snake[0].x <= 0 && positions_of_food[i].x <= 0) {
        if (positions_of_snake[0].y >= 0 && positions_of_food[i].z >= 0) {
            first = positions_of_snake[0].x - positions_of_food[i].x;
            second = positions_of_snake[0].z - positions_of_food[i].z;
        }
    }

    if (first == 10 || second == 10) {
        return;
    }

    int cond1 = first >= -0.02 && first <= 0.02;
    int cond2 = second >= -0.02 && second <= 0.02;

    if (cond1 && cond2) {
        printf("hrana\n");
        
    }


}

static void on_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    view();
    
    
    drawWall();

    draw_food(0);
    pos = 1;

   /* if (indicator_draw_food) {
        draw_food(pos);
        pos = pos + 1;
        indicator_draw_food = 0;
    } */


    draw_snake();

    

    
    

    glutSwapBuffers();

}
