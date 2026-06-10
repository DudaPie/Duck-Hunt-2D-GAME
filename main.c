
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#define TAM_JANELA 100.00

// Variáveis de controle de Janela (Estrutura do Modelo)
int g_posicao_x = 50;
int g_posicao_y = 50;
int g_largura = 800;
int g_altura = 600;
char g_titulo[] = "Duck Hunt";
int g_idle = 0;
int g_timer = 1;           // Ativado para animação
int g_timer_value = 25;    // Taxa de atualização (25ms)

// Variáveis de Projeção e Jogo
float g_angulo_visao = 60.0f; 
float g_z_proximo = 1.0f;
float g_z_distante = 100.0f;

float g_pato_x = 0;
float g_pato_y = 0;
float g_pato_z = -10.0f;
float g_pato_vel_x = 0.12f;
float g_pato_vel_y = 0.08f;
int g_pato_vivo = 1;
int g_score = 0;
int g_foi_baleado = 0;
int g_botao_pressionado = 0;

// Funções Lógicas do Jogo
void ResetaPato() {
    g_pato_vivo = 1;
    g_foi_baleado = 0;
    g_pato_x = -7.0f + (rand() % 15);
    g_pato_y = rand() % 6;
    g_pato_z = -10.0f;
    g_pato_vel_x = 0.08f + (rand() % 8) / 100.0f;
    g_pato_vel_y = 0.05f + (rand() % 6) / 100.0f;
    if (rand() % 2 == 0) g_pato_vel_x = -g_pato_vel_x;
    if (rand() % 2 == 0) g_pato_vel_y = -g_pato_vel_y;
}

void DesenhaPato() {
    glPushMatrix();
    
    float escala_x = 0.006f;
    if (g_pato_vel_x < 0) {
        escala_x = -0.006f; 
    }
    
    if (g_foi_baleado) {
        g_pato_y -= 0.3f;
        if (g_pato_y < -2.0f) g_pato_y = -2.0f;
        
        glTranslatef(g_pato_x, g_pato_y, g_pato_z);
        glScalef(escala_x, 0.006f, 1.0f);
        
        glLineWidth(1.5f);
        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_LINES);
            glVertex2f(-13.0f, -75.0f);
            glVertex2f(-7.0f, -80.0f);
            glVertex2f(-7.0f, -75.0f);
            glVertex2f(-13.0f, -80.0f);
        glEnd();
        glBegin(GL_LINES);
            glVertex2f(13.0f, -75.0f);
            glVertex2f(7.0f, -80.0f);
            glVertex2f(7.0f, -75.0f);
            glVertex2f(13.0f, -80.0f);
        glEnd();
        
        glColor3f(1.1f, 1.1f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(30.5f, 85.0f);
            glVertex2f(20.0f, 65.0f);
            glVertex2f(13.5f, 85.0f);
        glEnd();
        glBegin(GL_TRIANGLES);
            glVertex2f(-30.5f, 85.0f);
            glVertex2f(-21.0f, 65.0f);
            glVertex2f(-11.5f, 85.0f);
        glEnd();
        
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(35.5f, 40.0f);
            glVertex2f(0.0f, 100.0f);
            glVertex2f(-35.5f, 40.0f);
        glEnd();
        glBegin(GL_TRIANGLES);
            glVertex2f(10.5f, -40.0f);
            glVertex2f(35.5f, 0.0f);
            glVertex2f(10.5f, 0.0f);
        glEnd();
        glBegin(GL_TRIANGLES);
            glVertex2f(-10.5f, -40.0f);
            glVertex2f(-35.5f, 0.0f);
            glVertex2f(-10.5f, 0.0f);
        glEnd();

        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_QUADS);
            glVertex2f(-10.5f, -40.0f);  
            glVertex2f(-10.5f, 0.0f); 
            glVertex2f(10.5f, 0.0f);  
            glVertex2f(10.5f, -40.0f);  
        glEnd();
        
        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_QUADS);
            glVertex2f(-35.5f, 40.0f);  
            glVertex2f(-35.5f, 0.0f); 
            glVertex2f(35.5f, 0.0f);  
            glVertex2f(35.5f, 40.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_QUADS);
            glVertex2f(-10.5f, -40.0f);  
            glVertex2f(-10.5f, -50.0f); 
            glVertex2f(10.5f, -50.0f);  
            glVertex2f(10.5f, -40.0f);  
        glEnd();
        
        glColor3f(0.4f, 0.9f, 0.4f); 
        glBegin(GL_QUADS);
            glVertex2f(-15.5f, -50.0f);  
            glVertex2f(-15.5f, -80.0f); 
            glVertex2f(15.5f, -80.0f);  
            glVertex2f(15.5f, -50.0f);  
        glEnd();

        glColor3f(1.0f, 0.5f, 0.0f); 
        glBegin(GL_QUADS);
            glVertex2f(-10.0f, -80.0f);  
            glVertex2f(-10.0f, -100.0f); 
            glVertex2f(10.5f, -100.0f);  
            glVertex2f(10.5f, -80.0f);  
        glEnd();
        
    } else {
        glTranslatef(g_pato_x, g_pato_y, g_pato_z);
        glScalef(escala_x, 0.006f, 1.0f);
        
        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_QUADS);
            glVertex2f(60.0f, 40.0f);  
            glVertex2f(60.0f, 30.0f);  
            glVertex2f(70.5f, 30.0f);  
            glVertex2f(70.5f, 40.0f); 
        glEnd();
        
        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-17.0f, 45.5f);  
            glVertex2f(-46.0f, 30.0f);  
            glVertex2f(-30.5f, 13.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-17.0f, 50.5f);  
            glVertex2f(-46.0f, 30.0f);  
            glVertex2f(-17.5f, 45.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-58.0f, 55.5f);  
            glVertex2f(-133.0f, 0.0f);  
            glVertex2f(-57.5f, 45.0f);  
        glEnd();
        
        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-58.0f, 50.5f);  
            glVertex2f(-133.0f, 0.0f);  
            glVertex2f(-17.5f, -5.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 0.0f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-69.0f, -44.5f);  
            glVertex2f(-45.0f, -60.0f); 
            glVertex2f(-76.5f, -60.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 0.0f); 
        glBegin(GL_QUADS);
            glVertex2f(-65.0f, -34.0f);  
            glVertex2f(-65.0f, -50.0f);  
            glVertex2f(-72.5f, -50.0f);  
            glVertex2f(-72.5f, -34.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.4f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-5.0f, -39.5f);   
            glVertex2f(-137.5f, -30.0f); 
            glVertex2f(17.5f, 0.0f);     
        glEnd();

        glColor3f(0.1f, 0.1f, 0.1f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(-5.0f, 22.5f);    
            glVertex2f(-137.5f, -30.0f); 
            glVertex2f(17.5f, 0.0f);     
        glEnd();

        glColor3f(1.0f, 1.0f, 1.4f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-5.0f, 22.5f); 
            glVertex2f(32.5f, 15.0f);  
            glVertex2f(17.5f, 0.0f);  
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.4f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-5.0f, 22.5f); 
            glVertex2f(32.5f, 15.0f);  
            glVertex2f(7.5f, 40.0f);  
        glEnd();
        
        glColor3f(0.4f, 0.9f, 0.4f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(32.5f, 15.0f); 
            glVertex2f(62.5f, 65.0f); 
            glVertex2f(7.5f, 40.0f);  
        glEnd();
        
        glColor3f(0.4f, 0.9f, 0.4f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(32.5f, 15.0f); 
            glVertex2f(92.5f, 15.0f); 
            glVertex2f(62.5f, 65.0f); 
        glEnd();
        
        glColor3f(1.0f, 0.5f, 0.0f); 
        glBegin(GL_TRIANGLES);
            glVertex2f(137.5f, 40.0f); 
            glVertex2f(62.5f, 15.0f); 
            glVertex2f(62.5f, 40.0f); 
        glEnd();
    }
    
    glPopMatrix();
}

void DesenhaCenario() {
    glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_QUADS);
        glVertex3f(-20.0f, -2.0f, -20.0f);
        glVertex3f(-20.0f, -2.0f, 10.0f);
        glVertex3f(20.0f, -2.0f, 10.0f);
        glVertex3f(20.0f, -2.0f, -20.0f);
    glEnd();
    
    glPushMatrix();
        glTranslatef(-8.0f, -2.0f, -15.0f);
        glColor3f(0.5f, 0.3f, 0.1f);
        glRectf(-0.3f, 0.0f, 0.3f, 3.0f);
        glColor3f(0.0f, 0.5f, 0.0f);
        glTranslatef(0.0f, 3.0f, 0.0f);
        glutSolidSphere(1.2, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, -2.0f, -12.0f);
        glColor3f(0.5f, 0.3f, 0.1f);
        glRectf(-0.25f, 0.0f, 0.25f, 2.5f);
        glColor3f(0.0f, 0.5f, 0.0f);
        glTranslatef(0.0f, 2.5f, 0.0f);
        glutSolidSphere(1.0, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(-5.0f, 7.0f, -18.0f);
        glutSolidSphere(1.2, 8, 8);
        glTranslatef(1.0f, 0.3f, 0.0f);
        glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
}

void DesenhaUI() {
    int i;
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, g_largura, 0, g_altura, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(15, g_altura - 30);
    char buffer[50];
    sprintf(buffer, "SCORE: %d", g_score);
    i = 0;
    while (buffer[i]) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
        i++;
    }

    if (g_foi_baleado) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2i(g_largura/2 - 60, g_altura/2 + 20);
        char msg[] = "ACERTOU! +100";
        i = 0;
        while (msg[i]) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);
            i++;
        }
    }
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(15, 30);
    char instr[] = "Clique no pato!!";
    i = 0;
    while (instr[i]) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, instr[i]);
        i++;
    }

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
}

// Implementações das Callbacks do Modelo Base
void gMeusDesenhos() {
    // Configuração de perspectiva 3D necessária para o jogo funcionar
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(g_angulo_visao, (float)g_largura/g_altura, g_z_proximo, g_z_distante);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,  0, 2, -10,  0, 1, 0);
    
    DesenhaCenario();
    DesenhaPato();
    DesenhaUI();
}

void gDesenha (void) {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glMatrixMode (GL_MODELVIEW); 
    
    gMeusDesenhos();
    glutSwapBuffers ();
}

void gRedimensiona (GLsizei largura, GLsizei altura) {
    GLsizei a_l, l_a;
   
    if (altura == 0) altura = 1; 
    g_largura = largura; // Atualiza dimensões internas para cálculo do clique do mouse
    g_altura = altura;
    
    a_l = altura / largura;
    l_a = largura / altura;
   
    glViewport (0, 0, largura, altura); 
    glMatrixMode (GL_PROJECTION); 
    glLoadIdentity(); 
   
    if (largura <= altura) gluOrtho2D (-TAM_JANELA, TAM_JANELA, (-TAM_JANELA * a_l), (TAM_JANELA * a_l));
    else gluOrtho2D ((-TAM_JANELA * l_a), (TAM_JANELA * l_a), -TAM_JANELA, TAM_JANELA);
}

void gTeclado (unsigned char tecla, int x, int y) {
    if(tecla == 27) exit(0);
}

void gEspeciais (int tecla, int x, int y) {
    glutPostRedisplay();
}

void gMouse (int botao, int estado, int x, int y) {
    char msg[200];
    if (botao == GLUT_LEFT_BUTTON) {
        if (estado == GLUT_DOWN) {
            g_botao_pressionado = 1;
            
            if (g_pato_vivo && !g_foi_baleado) {
                int patoTelaX = (int)((g_pato_x + 9.0f) * (g_largura / 18.0f));
                int patoTelaY = (int)((g_pato_y + 1.0f) * (g_altura / 8.0f));
                
                float dx = (float)x - patoTelaX;
                float dy = (float)(g_altura - y) - patoTelaY;
                float dist = sqrt(dx*dx + dy*dy);
                
                if (dist < 80.0f) {
                    g_foi_baleado = 1;
                    g_pato_vivo = 0;
                    g_score += 100;
                } else {
                    g_score -= 50;
                }
            }
        } else if (estado == GLUT_UP) {
            g_botao_pressionado = 0;
        }
    }
}

void gTempoExecucao (int valor) {
    if (!g_foi_baleado) {
        g_pato_x += g_pato_vel_x;
        g_pato_y += g_pato_vel_y;
        
        if (g_pato_x > 9 || g_pato_x < -9) g_pato_vel_x = -g_pato_vel_x;
        if (g_pato_y > 7 || g_pato_y < -1) g_pato_vel_y = -g_pato_vel_y;
    } else {
        static int tempo_morte = 0;
        tempo_morte++;
        if (tempo_morte > 30) {
            ResetaPato();
            tempo_morte = 0;
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(g_timer_value, gTempoExecucao, 1);
}

// Preservadas do Modelo
void gMousePressionado (int x, int y) {;}
void gMouseLiberado (int x, int y) {;}
void gMouseScroll (int botao, int direcao, int x, int y) {;}
void gSistemaOcioso (void) {;}
void gMenuTeclado (void) {;}
void gMenuJanela (void) {;}

void gInicializa (void) {
    glEnable(GL_DEPTH_TEST);
    glClearColor (0.53f, 0.81f, 0.92f, 1.0f); // Cor do céu adaptada
    srand((unsigned int)time(NULL));
    ResetaPato();
    
    glMatrixMode (GL_PROJECTION);
    gluOrtho2D (-TAM_JANELA, TAM_JANELA, -TAM_JANELA, TAM_JANELA); 
}

int main (int argc, char *argv[]) {
    
    glutInit (&argc, argv); 
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition (g_posicao_x, g_posicao_y);
    glutInitWindowSize (g_largura, g_altura);
    glutCreateWindow (g_titulo);
    
    glutDisplayFunc (gDesenha);
    glutReshapeFunc (gRedimensiona);
    glutKeyboardFunc (gTeclado);
    glutSpecialFunc (gEspeciais);
    glutMouseFunc (gMouse);
    glutMotionFunc (gMousePressionado);
    glutPassiveMotionFunc (gMouseLiberado);
    glutMouseWheelFunc (gMouseScroll);
    
    if (g_idle) glutIdleFunc (gSistemaOcioso); 
    if (g_timer) glutTimerFunc (g_timer_value, gTempoExecucao, 1); 
    
    gMenuTeclado ();
    gMenuJanela ();
    gInicializa ();
    
    glutMainLoop ();
    
    system ("pause");
    return 0;
}

