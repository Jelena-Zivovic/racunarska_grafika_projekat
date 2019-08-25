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

/*makroi koji se koriste za definisanje ishoda igre da bi se u skladu s tim ispisala odgovarajuca poruka */

#define WIN 1
#define LOSE 2

/*makroi koji oznacavaju pravac i smer kretanja zmije */

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

/*indikator koji se koristi za definisanje ishoda igre */
static int indicator_winning = 0;

/*sirina i visina prozora */
static int window_width, window_height;

/*parametri pomeranja zmije */
static Parameter parameters;

/*indikator animacije */
static int is_animation_going = 0;

/*pocetna brzina zmije i parametar uvecanja brzine kada zmije pojede hranu */
static float speed = 0.006;

static float size = 0.1;

/*trenutni pravac i smer kretanja zmije */
static int current_direction = LEFT;

/*koordinate zmije */
static Position position_of_snake;

/*pozicija hrane */
static Position position_of_food;

/*broj poena, tj. koliko je zmija do sada pojela hrane */
static int number_of_foods_eaten = 0;

/*nazivi predmeta koji se ispisuju pored hrane */
static char subjects[][32] =
{"P1", "UOAR1", "DS1", "LAAG", "P2", "UOAR2", "DS2", "A1",
"AISP", "UVIT", "OS", "G", "A2", "KIAA", "OOP", "A3", "ALG1",
"RBP", "PPJ", "RG", "V", "UNM", "VI", "IP1", "PP", "PBP", "S",
"RM", "RS", "ProjBP", "MSNR", "KK"};

/*broj naziva predmeta u nizu predmeta */
int num_subjects = 32;

/*trenutna pozicija u nizu predmeta */
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

   
    /*parametri pravca i smera kretanja zmije */
    /*na pocetku je sve nula jer se zmija ne krece */

    parameters.parameter_x = 0;
    parameters.parameter_y = 0;
    parameters.parameter_z = 0;

    srand(time(NULL));

    /*generisemo nasumicne pozicije hrane, ali da budu van prepreka */

    position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
    position_of_food.y = 0;
    position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);

    /*uslovi koji oznacavaju da je hrana van prve prepreke(donji desni ugao) */
    int condition1_x = position_of_food.x <= -0.58 && position_of_food.x >= -0.82;
    int condition1_z = position_of_food.z <= 1.62 && position_of_food.z >= -0.58;

    /*ako se slucajno izgenerise pozicija koja je na prostoru gde je prepreka, nove koordinate se generisu sve dok se ne dobije pozicija koja je van prostora prepreke */
    while (condition1_x && condition1_z) {
        /*slucajan broj izmedju -1.8 i 1.8, da bi hrana bila u okviru zidova, u ovom slucaju, tacnije,
        izmedju gornjeg i donjeg zida*/
        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        condition1_x = position_of_food.x <= -0.58 && position_of_food.x >= -0.82;
        /*slucajan broj izmedju -2.8 i 2.8, u ovom slucaju, izmedju levog i desnog */
        position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        condition1_z = position_of_food.z <= -0.58 && position_of_food.z >= -0.82;
      
    }

    /*uslovi koji oznacavaju da je hrana van druge prepreke(gornji levi ugao) */
    int condition2_x = position_of_food.x <= 1.35 && position_of_food.x >= -0.35;
    int condition2_z = position_of_food.z <= -1.95 && position_of_food.z >= -2.05;

    while (condition2_x && condition2_z) {
        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        condition2_x = position_of_food.x <= -0.6 && position_of_food.x >= -0.8;
        position_of_food.z = -1.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        condition2_z = position_of_food.z <= -0.6 && position_of_food.z >= -0.8;
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

    
    /*ukljucuje se osvetljenje */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    /*podesavaju se parametri svetla */
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    /*podesavaju se parametri materijala */
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /*prvo transliramo, pa skaliramo pravougaonik */    
    glTranslatef(0, 0, -3);
    glScalef(4, 0.3, 0.2);
    
    /*crta se pravougaonik */
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

   /*iskljucuje se osvetljenje */ 
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
 
}

static void on_reshape(int width, int height) {
    window_width = width;
    window_height = height;
}

static void on_special(int key, int x, int y) {
    /*generisu se akcije prilikom pritiska na strelice
     */
    switch(key) {
        case 101:
            /*ako je pritisnuta strelica na gore */
            current_direction = UP;

            if (!is_animation_going) {
                /*ako animacija nija aktivna, aktivira se */
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 103:
            /*ako je pritisnuta strelica na dole */
            current_direction = DOWN;
            
            if (!is_animation_going) { 
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 100:
            /*ako je pritisnuta strelica na levo */
            current_direction = LEFT;
            
            if (!is_animation_going) {
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;
        case 102:
        /*ako je pritisnuta strelica na desno */
            current_direction = RIGHT;
           
            if (!is_animation_going) {
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                is_animation_going = 1;
            }
            break;

    }
    /*ponovo se iscrtava scena */
    glutPostRedisplay();
}

static void on_timer(int value) {
    if (value != 0) {
        return;
    }
    /*proverava se da li je zmija udarila u zid */
    if (position_of_snake.x >= 1.8 ||
        position_of_snake.x <= -1.8 ||
        position_of_snake.z >= 2.8 || 
        position_of_snake.z <= -2.8) {
        
        /*ako jeste, indikator ishoda igre se postavlja na LOSE, kako bi se ispisala odgovarajuca poruka */
        indicator_winning = LOSE;
        /*oznacava se da je igra gotova */
        game_over();
    }

    /*proverava se da li je zmija udarila u prvu prepreku */

    int condition1_x = position_of_snake.x <= -0.6 && position_of_snake.x >= -0.8;
    int condition1_z = position_of_snake.z <= 1.6 && position_of_snake.z >= -0.6;

    if (condition1_x && condition1_z) {
        /*ako jeste, indikator ishoda igre se postavlja na LOSE, kako bi se ispisala odgovarajuca poruka */
        indicator_winning = LOSE;
       /*oznacava se da je igra gotova */
        game_over();
    }

    /*proverava se da li je zmija udarila u prvu prepreku */

    int condition2_x = position_of_snake.x <= 1.32 && position_of_snake.x >= -0.32;
    int condition2_z = position_of_snake.z <= -1.9 && position_of_snake.z >= -2.1;

    if (condition2_x && condition2_z) {
        indicator_winning = LOSE;
        game_over();
    }

    /*ako zmija nije udarila u prepreku, proverava se da li je pojela hranu */
    check_is_food_eaten();

    /*u skladu sa pritisnutim tasterom se azuriraju parametri pomeranja zmije */
    switch (current_direction)
    {
        case UP:
            parameters.parameter_x += speed;
            break;
        case DOWN:
            parameters.parameter_x -= speed;
            break;
        case LEFT:
            parameters.parameter_z -= speed;
            break;
        case RIGHT:
            parameters.parameter_z += speed;
            break;
    }

    /*ponovo se iscrtava scena */
    glutPostRedisplay();

    if (is_animation_going) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_keyboard(unsigned char key, int x, int y) {
    /*zmija moze i da se pokrece pritiskom na tastere 'a' za levo, 'w' za gore, 'd' za desno i 's' za dole */
    /*pritiskom na ESC se prozor zatvara */
    /*pritiskom na taster 'p', igrica se pauzira */
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
        case 's':
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

    /*crtanje zmije, koja je samo jedna kocka */
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

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    /*podesavaju se parametri svetla */
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

    /*podesavaju se parametri materijala */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   
    /*zmija se prvo translira u skladu sa parametrima pomeranja koji se dobijaju na osnovu pritisnutih tastera */
    glTranslatef(
        parameters.parameter_x,
        parameters.parameter_y,
        parameters.parameter_z
    );

    /*azurira se trenutna pozicija zmije */
    position_of_snake.x = parameters.parameter_x;
    position_of_snake.y = parameters.parameter_y;
    position_of_snake.z = parameters.parameter_z;

    /*crta se kockica koja predstavlja zmiju */
    glutSolidCube(2*size);

    glPopMatrix();

    /*iskljucuje se osvetljenje */
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
}

static void draw_food() {
    
    /*crta se hrana koja predstavlja kruzice */

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

    /*hrana se translira u skladu sa parametrima hrane, a to su u stvari nasumicno odabrane koordinate */
    glTranslatef(
        position_of_food.x,
        position_of_food.y,
        position_of_food.z
    );
    
   /*crta se hrana */
    glutSolidSphere(0.07, 20, 20);
   
    glPopMatrix();

    /*postavlja se rasterska pozicija za ispisivanje naziva predmeta */
    glRasterPos3f(position_of_food.x, 0, position_of_food.z+0.08);
    
    /*ispisuje se tekst koji je na trenutnoj poziciji u nizu predmeta */
    write_text(subjects[pos]);

    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHTING);
    
}

static void game_over() {
    /*ako je igra gotova, zaustavlja se animacija */
    is_animation_going = 0;
}

static void check_is_food_eaten(){

    /*proverava se da li je hrana pojedena */

    /*promenljive first i second predstavljaju razliku trenutnih pozicija zmije i hrane */
    /*postavljaju se na 10 jer je to vrednost koju ne mogu da imaju */
    /*pa ako je na kraju njihova vrednost 10, znaci da zmija nije dosla do hrane */
    float first = 10;
    float second = 10;

     /*provera za prvi kvadrant, gde su x i z pozitivni, y je 0 */
    if (position_of_snake.x >= 0 && position_of_food.x >= 0) {
        if (position_of_snake.y >= 0 && position_of_food.z >= 0) {
            first = position_of_snake.x - position_of_food.x;
            second = position_of_snake.z - position_of_food.z;
        }
    
    }

    /*provera za drugi kvadrant, gde je x pozitivno, a z negativno */
    if (position_of_snake.x >= 0 && position_of_food.x >= 0) {
        if (position_of_snake.y <= 0 && position_of_food.z <= 0) {
            first = position_of_snake.x - position_of_food.x;
            second = position_of_snake.z - position_of_food.z;
        }
    }

    /*provera za treci kvadrant, gde su x i z negativni */
    if (position_of_snake.x <= 0 && position_of_food.x <= 0) {
        if (position_of_snake.y <= 0 && position_of_food.z <= 0) {
            first = position_of_snake.x - position_of_food.x;
            second = position_of_snake.z - position_of_food.z;
        }
    }

    /*provera za cetvri kvadrant, gde je x negativno, a z pozitivno*/
    if (position_of_snake.x <= 0 && position_of_food.x <= 0) {
        if (position_of_snake.y >= 0 && position_of_food.z >= 0) {
            first = position_of_snake.x - position_of_food.x;
            second = position_of_snake.z - position_of_food.z;
        }
    }
    /*da li je zmija dosla do hrane */
    if (first == 10 || second == 10) {
        /*nije dosla do hrane */
        return;
    }
    /*uslovi koji predstavljaju indikatore da li je zmija dosla do hrane, odnosno da li je razlika izmedju
    trenutne pozicije zmije i hrane dovoljno mala */
    int cond1 = first >= -0.04 && first <= 0.04;
    int cond2 = second >= -0.04 && second <= 0.04;

    if (cond1 && cond2) {
        /*ako jeste, generise se nova slucajna pozicija hrane */
        

        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        position_of_food.y = 0;
        position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);

        /*isto kao i na pocetku, potrebno je da se generise pozicija hrane koja je van prostra prepreka */

        /*uslovi koji oznacavaju da je hrana van prve prepreke(donji desni ugao) */
        int condition1_x = position_of_food.x <= -0.58 && position_of_food.x >= -0.82;
        int condition1_z = position_of_food.z <= 1.62 && position_of_food.z >= -0.58;

        /*ako se slucajno izgenerise pozicija koja je na prostoru gde je prepreka, nove koordinate se generisu sve dok se ne dobije pozicija koja je van prostora prepreke */
        while (condition1_x && condition1_z) {
            /*slucajan broj izmedju -1.8 i 1.8, da bi hrana bila u okviru zidova, u ovom slucaju, tacnije,
            izmedju gornjeg i donjeg zida*/
            position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
            condition1_x = position_of_food.x <= -0.58 && position_of_food.x >= -0.82;
            /*slucajan broj izmedju -2.8 i 2.8, u ovom slucaju, izmedju levog i desnog */
            position_of_food.z = -2.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
            condition1_z = position_of_food.z <= -0.58 && position_of_food.z >= -0.82;
        
    }

    /*uslovi koji oznacavaju da je hrana van druge prepreke(gornji levi ugao) */
    int condition2_x = position_of_food.x <= 1.35 && position_of_food.x >= -0.35;
    int condition2_z = position_of_food.z <= -1.95 && position_of_food.z >= -2.05;

    while (condition2_x && condition2_z) {
        position_of_food.x = -1.8 + (rand() / (float)RAND_MAX) * (1.8 + 1.8);
        condition2_x = position_of_food.x <= -0.6 && position_of_food.x >= -0.8;
        position_of_food.z = -1.8 + (rand() / (float)RAND_MAX) * (2.8 + 2.8);
        condition2_z = position_of_food.z <= -0.6 && position_of_food.z >= -0.8;
    }

        /*uvecava se broj poena */
        number_of_foods_eaten++;
        /*povecava se brzina */
        speed += 0.003;
        if (pos == num_subjects) {
            /*ako je trenutna pozicija jednaka broju predmeta, igra je gotova i ishod je pobeda */
            indicator_winning = WIN;
            game_over();
        } {
            /*inace se prelazi na sledecu poziciju u nizu */
            pos++;
        } 
    }
}

/*ispisuje se skracenice predmeta iz niza predmeta */
static void write_text(char *s) {
    int i;
    for (i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s[i]);
    }
}

static void draw_score() {

    /*ispisuje se trenutni rezultat */

    char *str = "SCORE: ";
    glColor3f(1,1,1);
    glRasterPos3f(2, 0, -4.2);
    int i;
    for (i = 0; i < strlen(str); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
 
    }
    /*pretvara se int u char */
    char score[3];
    score[0] = number_of_foods_eaten / 10 + '0';
    score[1] = number_of_foods_eaten % 10 + '0';
    score[2] = 0;

    for (i = 0; i < 3; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score[i]);
 
    }
}

static void write_winner_message() {

    /*ispisuje se pobednicka poruka */

    glColor3f(1, 1, 1);
    glRasterPos3f(0, 0, -0.9);
    int i;
    char *message = "Cestitamo, polozili ste sve predmete!";

    for (i = 0; i < strlen(message); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
 
    }
}

static void write_loser_message() {

    /*ispisuje se gubitnicka poruka */

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
