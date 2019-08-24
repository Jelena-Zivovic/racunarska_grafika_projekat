#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdio.h>

GLubyte rasters[5 * 24] = {
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000011, 0b00001100,
    0b11000111, 0b10001100,
    0b11001100, 0b11001100,
    0b11011000, 0b01101100,
    0b11110000, 0b00111100,
    0b11100000, 0b00011100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,

    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11000000, 0b00001100,
    0b11100000, 0b00011100,
    0b01111111, 0b11111000,
    0b00111111, 0b11110000,

    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b00000011, 0b00000000,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,

    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,

    0b11110000, 0b00000000,
    0b11110000, 0b00000000,
    0b11110000, 0b00000000,
    0b11110000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000
};

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
static float parameter_food_x;
static float parameter_food_z;

static int current_direction = LEFT;
static int previous_direction = LEFT;

static Position *positions_of_snake;

static Position position_of_food;
static int number_of_foods_eaten = 0;


static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_special(int key, int x, int y);

static void drawWall();
static void view();
static void draw_snake();
static void draw_food();
static void game_over();
static void check_is_food_eaten();


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

    
    srand(time(NULL));

    /*generisemo random pozicije hrane */

  
    position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
    position_of_food.z = 0;
    position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);


    glShadeModel(GL_SMOOTH);
    glCullFace(GL_FRONT_AND_BACK);
  

    glutReshapeFunc(on_reshape);
    glutSpecialFunc(on_special);
    glutKeyboardFunc(on_keyboard);
    
    glutDisplayFunc(on_display);
    

    glClearColor(0, 0.58, 0, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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


}

static void drawWall() {

    /*zid se sastoji od 4 pravougaonika */

    
   
    /*leva strana zida */
    glPushMatrix();

     /*pozicija svetla*/
    GLfloat position[] = {0, 5, 0, 0};

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

    GLfloat shininess = 20;

    /*podesavaju se parametri svetla */

    glEnable(GL_LIGHTING);
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

    /*glColor3f(0.7, 0, 0); */
    
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

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    
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

    check_is_food_eaten();

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
        case 'p':
            is_animation_going = 0;
            break;
        

    }
}


static void draw_snake() {


    glPushMatrix();

   

     /*pozicija svetla*/
    GLfloat position[] = {0, 5, 0, 0};

    /*ambijentalna boja svetla */
    GLfloat ambient[] = {0, 0.2, 0.7, 1};

    /*difuzna boja svetla */
    GLfloat diffuse[] = {0, 0, 0.8, 1};

    /*spekularna boja svetla */
    GLfloat specular[] = {0.1, 0.1, 0.1, 1};

    /*ambijentalna refleksija materijala */
    GLfloat a[] = {0.1, 0.1, 0.1, 0.9, 1};

    /*difuzna refleksija materijala */
    GLfloat d[] = {0.0, 0.2, 0.8, 1};

    /*spekularna refleksija materijala */
    GLfloat s[] = {1, 1, 1, 1};

    /*odsjaj */

    GLfloat shininess = 30;

    /*podesavaju se parametri svetla */

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

    /*podesavaju se parametri materijala */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   

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

    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);


}

static void draw_food() {
    

    GLfloat position[] = {0, 5, 0, 0};

    /*ambijentalna boja svetla */
    GLfloat ambient[] = {0, 0, 0, 1};

    /*difuzna boja svetla */
    GLfloat diffuse[] = {0.7, 0.7, 0.7, 1};

    /*spekularna boja svetla */
    GLfloat specular[] = {0.9, 0.9, 0.9, 1};

    /*ambijentalna refleksija materijala */
    GLfloat a[] = {1, 0.1, 0.1,  1};

    /*difuzna refleksija materijala */
    GLfloat d[] = {0, 0, 0.8, 1};

    /*spekularna refleksija materijala */
    GLfloat s[] = {1, 1, 1, 1};

    /*odsjaj */

    GLfloat shininess = 30;

    /*podesavaju se parametri svetla */

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);
    
    glLightfv(GL_LIGHT2, GL_POSITION, position);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

    /*podesavaju se parametri materijala */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glPushMatrix();

    glTranslatef(
        position_of_food.x,
        position_of_food.y,
        position_of_food.z
    );

    

    glutSolidSphere(0.07, 20, 20);

    

    glPopMatrix();

    /*preuzeto iz materijala na sajtu asistenta Bozidara Antica */

    glRasterPos3i(position_of_food.x, 0, position_of_food.z);
    int i;
    for (i = 0; i < 4; i++) {
        glBitmap(
                16, 12,
                0.0, 0.0,
                16.0, 0.0,
                rasters + 24 * i
                );
    }

    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHTING);
    
}

static void game_over() {
    /*treba da se ispise da je igra gotova i poeni */

    

    /*brzina se postavlja na nulu, sto znaci je sada onemoguceno kretanje zmije  */
    speed = 0;

    printf("%d\n", number_of_foods_eaten);


}

static void check_is_food_eaten(){

    /*provera za prvi kvadrant, gde su x i z pozitivni, y je 0 */
    float first = 10;
    float second = 10;

    if (positions_of_snake[0].x >= 0 && position_of_food.x >= 0) {
        if (positions_of_snake[0].y >= 0 && position_of_food.z >= 0) {
            first = positions_of_snake[0].x - position_of_food.x;
            second = positions_of_snake[0].z - position_of_food.z;
        }
    
    }

    /*provera za drugi */

    if (positions_of_snake[0].x >= 0 && position_of_food.x >= 0) {
        if (positions_of_snake[0].y <= 0 && position_of_food.z <= 0) {
            first = positions_of_snake[0].x - position_of_food.x;
            second = positions_of_snake[0].z - position_of_food.z;
        }
    }

    /*treci */

    if (positions_of_snake[0].x <= 0 && position_of_food.x <= 0) {
        if (positions_of_snake[0].y <= 0 && position_of_food.z <= 0) {
            first = positions_of_snake[0].x - position_of_food.x;
            second = positions_of_snake[0].z - position_of_food.z;
        }
    }

    /*cetvrti */
    if (positions_of_snake[0].x <= 0 && position_of_food.x <= 0) {
        if (positions_of_snake[0].y >= 0 && position_of_food.z >= 0) {
            first = positions_of_snake[0].x - position_of_food.x;
            second = positions_of_snake[0].z - position_of_food.z;
        }
    }

    if (first == 10 || second == 10) {
        return;
    }

    int cond1 = first >= -0.03 && first <= 0.03;
    int cond2 = second >= -0.03 && second <= 0.03;

    if (cond1 && cond2) {
      
        parameter_food_x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        parameter_food_z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        position_of_food.x = parameter_food_x;
        position_of_food.z = parameter_food_z;
        number_of_foods_eaten++;
        speed += 0.003;
       
    }


}

static void on_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    view();
 
   drawWall();
    

    draw_food();
  
    draw_snake();

    glutSwapBuffers();

}
