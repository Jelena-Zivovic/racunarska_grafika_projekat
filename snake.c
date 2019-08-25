#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

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

#define WIN 1
#define LOSE 2

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

static int indicator_winning = 0;

static int window_width, window_height;

static Parameter *parameters;
static int is_animation_going = 0;
static float speed = 0.006;
static float size = 0.1;
static float parameter_food_x;
static float parameter_food_z;

static int current_direction = LEFT;

static Position *positions_of_snake;

static Position position_of_food;
static int number_of_foods_eaten = 0;

static char subjects[][32] =
{"P1", "UOAR1", "DS1", "LAAG", "P2", "UOAR2", "DS2", "A1",
"AISP", "UVIT", "OS", "G", "A2", "KIAA", "OOP", "A3", "ALG1",
"RBP", "PPJ", "RG", "V", "UNM", "VI", "IP1", "PP", "PBP", "S",
"RM", "RS", "ProjBP", "MSNR", "KK"};

int num_subjects = 32;

int pos = 0;

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
static void write_text(char *s);
static void draw_score();
static void write_winner_message();
static void write_loser_message();


int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(1100, 1100);
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

    /*generisemo random pozicije hrane, ali da budu van prepreka */

  
    position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
    position_of_food.z = 0;
    position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);

    int condition1_x = positions_of_snake[0].x <= -0.58 && positions_of_snake[0].x >= -0.82;
    int condition1_z = positions_of_snake[0].z <= 1.62 && positions_of_snake[0].z >= -0.58;

    while (condition1_x) {
        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        condition1_x = positions_of_snake[0].x <= -0.58 && positions_of_snake[0].x >= -0.82;
      
    }

   while (condition1_z) {
        position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        condition1_z = positions_of_snake[0].z <= -0.58 && positions_of_snake[0].z >= -0.82;
      
    }

    int condition2_x = positions_of_snake[0].x <= 1.35 && positions_of_snake[0].x >= -0.35;
    int condition2_z = positions_of_snake[0].z <= -1.95 && positions_of_snake[0].z >= -2.05;

    while (condition2_x) {
        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        condition2_x = positions_of_snake[0].x <= -0.6 && positions_of_snake[0].x >= -0.8;
    }

    while (condition2_z) {
        position_of_food.z = -1.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        condition2_z = positions_of_snake[0].z <= -0.6 && positions_of_snake[0].z >= -0.8;
    }


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
    glScalef(4, 0.3, 0.2);
    
    glutSolidCube(1);
    

    glPopMatrix();

    /*desna strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(0, 0, 3);
    glScalef(4, 0.3, 0.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*gornja strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(2, 0, 0);
    glScalef(0.2, 0.3, 6.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*donja strana zida */

    glPushMatrix();

    glColor3f(0.7, 0, 0);
    
    glTranslatef(-2, 0, 0);
    glScalef(0.2, 0.3, 6.2);
    
    glutSolidCube(1);

    glPopMatrix();

    /*prva prepreka */

    glPushMatrix();

    glTranslatef(-0.7, 0, 0.5);
    glScalef(0.1, 0.3, 2);

    glutSolidCube(1);

    glPopMatrix();

    

    /*druga prepreka */

    glPushMatrix();

    glTranslatef(0.5, 0, -2);
    glScalef(1.5, 0.3, 0.1);
    
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
            current_direction = UP;
            

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        
        case 103:
            current_direction = DOWN;
            
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 100:
            current_direction = LEFT;
            

            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 102:
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
        
       
        indicator_winning = LOSE;
        game_over();
    }

    /*da li je zmija udarila u prepreku */

    int condition1_x = positions_of_snake[0].x <= -0.6 && positions_of_snake[0].x >= -0.8;
    int condition1_z = positions_of_snake[0].z <= 1.6 && positions_of_snake[0].z >= -0.6;

    if (condition1_x && condition1_z) {
        indicator_winning = LOSE;
       
        game_over();
    }

    int condition2_x = positions_of_snake[0].x <= 1.32 && positions_of_snake[0].x >= -0.32;
    int condition2_z = positions_of_snake[0].z <= -1.9 && positions_of_snake[0].z >= -2.1;

    if (condition2_x && condition2_z) {
        indicator_winning = LOSE;
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
            current_direction = LEFT;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;

        case 'w':
            current_direction = UP;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 'd':
            current_direction = RIGHT;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 'x':
            current_direction = DOWN;
            if (!is_animation_going) {
                
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 'p':
            is_animation_going = 0;
            break;
        case 's':
            if (!is_animation_going) {
                parameters[0].parameter_x = 0;
                parameters[0].parameter_z = 0;
                speed = 0.006;
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
                
            }
        

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
    GLfloat specular[] = {0.9, 0.9, 0.5, 1};

    /*ambijentalna refleksija materijala */
    GLfloat a[] = {1, 0.4, 0.1,  1};

    /*difuzna refleksija materijala */
    GLfloat d[] = {0, 0.3, 0.3, 1};

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

    glRasterPos3f(position_of_food.x, 0, position_of_food.z+0.08);
  
    write_text(subjects[pos]);

    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHTING);
    
}

static void game_over() {
    /*treba da se ispise da je igra gotova i poeni */

    

    /*brzina se postavlja na nulu, sto znaci je sada onemoguceno kretanje zmije  */
    is_animation_going = 0;
    speed = 0;

  


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

    int cond1 = first >= -0.04 && first <= 0.04;
    int cond2 = second >= -0.04 && second <= 0.04;

    if (cond1 && cond2) {
      
        parameter_food_x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        parameter_food_z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        position_of_food.x = parameter_food_x;
        position_of_food.z = parameter_food_z;
        number_of_foods_eaten++;
        speed += 0.003;
        if (pos == num_subjects) {
            indicator_winning = WIN;
            game_over();
        } {
            pos++;
        }
       
    }


}

static void write_text(char *s) {
    int i;
    for (i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s[i]);
 
    }
}

static void draw_score() {
    char *str = "SCORE: ";
    glColor3f(1,1,1);
    glRasterPos3f(2, 0, -4.2);
    int i;
    for (i = 0; i < strlen(str); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
 
    }

    char score[3];
    score[0] = number_of_foods_eaten / 10 + '0';
    score[1] = number_of_foods_eaten % 10 + '0';
    score[2] = 0;

    for (i = 0; i < 3; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score[i]);
 
    }


}

static void write_winner_message() {

    glColor3f(1, 1, 1);
    glRasterPos3f(0, 0, -0.9);
    int i;
    char *message = "Cestitamo, polozili ste sve predmete!";

    for (i = 0; i < strlen(message); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
 
    }
}

static void write_loser_message() {
    glColor3f(1, 1, 1);
    glRasterPos3f(0, 0, -1.2);
    int i;
    char *message = "Vise srece sledeci put! Pritisnite ESC za izlaz.";

    for (i = 0; i < strlen(message); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
 
    }
}

static void on_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    view();
 
   drawWall();
    

    draw_food();
  
    draw_snake();

    draw_score();

    if (indicator_winning == WIN) {
        write_winner_message();
    }
    else if (indicator_winning == LOSE) {
        write_loser_message();
    }

    glutSwapBuffers();

}
