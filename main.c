/*
Bruno Alteração:
Mudei a hitbox do pato em gMouse() para 25.
Aumentei a velocidade dos patos em ResetaPato()
Spawn do pato nas laterais e quicando fora da tela
Alterei tirando toda partr 3D pra puramente 2D, eliminando necessidade dimensão Z
Otimização: Implementado Delta Time e V-Sync via Idle Loop para eliminar travamentos e engasgos visuais. Porém n sei se é meu note ou jogo msm.
Adicionado: Cronômetro de 60 segundos com reset automático de jogo ao zerar.
*/

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

// Definição do protótipo para o V-Sync do Windows
typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC) (int interval);

// Variáveis de controle de Janela (Estrutura do Modelo)
int g_posicao_x = 50;
int g_posicao_y = 50;
int g_largura = 800;
int g_altura = 600;
char g_titulo[] = "Duck Hunt 2D";

int g_idle = 1;
int g_timer = 0;           
int g_timer_value = 16;    



// Variáveis de Projeção e Jogo (Adaptadas para 2D)
float g_pato_x = 0;
float g_pato_y = 0;
float g_pato_vel_x = 8.0f;  
float g_pato_vel_y = 3.0f;
int g_pato_vivo = 1;
int g_score = 0;
int g_foi_baleado = 0;
int g_botao_pressionado = 0;

// Variáveis de Controle do Cronômetro e Estado de Fim de Jogo
float g_cronometro = 60.0f;       // Tempo inicial (1 minuto)
int g_jogo_finalizado = 0;       // Trava o jogo quando o tempo zera
float g_tempo_tela_final = 0.0f;  // Acumulador para segurar a tela de "Fim" por 2 segundos

// Controle de Tempo Real (Delta Time)
int g_tempo_anterior = 0;

// Funções Lógicas do Jogo
void ResetaPato() {
    g_pato_vivo = 1;
    g_foi_baleado = 0;
    
    g_pato_y = rand() % 5;
    
    g_pato_vel_x = 8.0f + (rand() % 200) / 20.0f;
    g_pato_vel_y = 3.0f + (rand() % 60) / 20.0f;
    
    if (rand() % 2 == 0) g_pato_vel_y = -g_pato_vel_y;

    if (rand() % 2 == 0) {
        g_pato_x = -13.5f; 
        if (g_pato_vel_x < 0) g_pato_vel_x = -g_pato_vel_x;
    } else {
        g_pato_x = 13.5f;  
        if (g_pato_vel_x > 0) g_pato_vel_x = -g_pato_vel_x;
    }
}

void ResetaJogoCompleto() {
    g_score = 0;
    g_cronometro = 60.0f;
    g_jogo_finalizado = 0;
    g_tempo_tela_final = 0.0f;
    ResetaPato();
}

// Função auxiliar para desenhar as formas circulares da copa
void DesenhaCirculoSolido(float cx, float cy, float r, float red, float green, float blue) {
    int seg = 24;
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; i++) {
            float ang = i * 2.0f * 3.14159265f / seg;
            glVertex2f(cx + cos(ang) * r, cy + sin(ang) * r);
        }
    glEnd();
    // Aqui não pode ter nenhuma chamada de glBegin(GL_LINE_LOOP) ou glColor3f com valores mais escuros!
}

void DesenhaNonagonoSolido(float cx, float cy, float r, float red, float green, float blue) {
    int lados = 9;
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Centro do polígono
        for (int i = 0; i <= lados; i++) {
            // Divide os 360 graus (2 * PI) por 9 lados
            float ang = i * 2.0f * 3.14159265f / lados;
            glVertex2f(cx + cos(ang) * r, cy + sin(ang) * r);
        }
    glEnd();
}

void DesenhaPato() {
    glPushMatrix();
    
    float escala_x = 0.006f;
    if (g_pato_vel_x < 0) {
        escala_x = -0.006f; 
    }
    
    if (g_foi_baleado) {
        glTranslatef(g_pato_x, g_pato_y, 0.0f);
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
        glTranslatef(g_pato_x, g_pato_y, 0.0f);
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
	
	glColor3f(0.2f, 0.7f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(-12.0f, 1.3f);  
        glVertex2f(-12.0f, -2.3f);  
        glVertex2f( 12.0f, -2.3f);  
        glVertex2f( 12.0f, 1.3f);  
    glEnd();

    glColor3f(0.25f, 0.12f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-12.0f, -2.2f);  
        glVertex2f(-12.0f, -2.3f);  
        glVertex2f( 12.0f, -2.3f);  
        glVertex2f( 12.0f, -2.2f);  
    glEnd();

    glColor3f(0.5f, 0.25f, 0.05f);
    glBegin(GL_QUADS);
        glVertex2f(-12.0f, -2.3f);  
        glVertex2f(-12.0f, -5.0f);  
        glVertex2f( 12.0f, -5.0f);  
        glVertex2f( 12.0f, -2.3f);  
    glEnd();
	
	glPushMatrix(); //curculo lago
    // 1. Cor do lago (azul)
    glColor3f(0.2f, 0.5f, 0.8f); 

    // Novos parâmetros: X de -8 a 8 | Y de -2 a 1
    float centro_x = 0.0f;     // Mudou para 0.0f (centralizado)
    float centro_y = -0.5f;
    float raio_x = 8.0f;       // Mudou para 8.0f (esticado até -8 e 8)
    float raio_y = 1.5f; 
    int num_segmentos = 50; 

    // 2. Desenha o preenchimento (Interior do Lago)
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centro_x, centro_y); 
        for (int i = 0; i <= num_segmentos; i++) {
            float angulo = i * 2.0f * 3.14159265f / num_segmentos;
            float x = centro_x + (cos(angulo) * raio_x);
            float y = centro_y + (sin(angulo) * raio_y);
            glVertex2f(x, y);
        }
    glEnd();

    // 3. Desenha a borda (Contorno do Lago)
    glColor3f(0.1f, 0.3f, 0.5f); 
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < num_segmentos; i++) {
            float angulo = i * 2.0f * 3.14159265f / num_segmentos;
            float x = centro_x + (cos(angulo) * raio_x);
            float y = centro_y + (sin(angulo) * raio_y);
            glVertex2f(x, y);
        }
    glEnd();
    glPopMatrix();
	
}
   
   //----------------------------------------------------------------------------------------------------------------------------------------------------
 // 1. FUNÇÃO AUXILIAR
float vector_pos_x_fantasia(int valor) {
    return (float)((valor * 123456789) % 3) - 1.0f;
}

// 2. FUNÇÃO DA GRAMA CORRIGIDA (Base em -2.3f)
void DesenhaGrama() {
    glPushMatrix();

    // 1. BASE DA GRAMA (Retângulo verde plano)
    // Agora começa exatamente em Y = -2.3f e vai até Y = -1.5f
    glColor3f(0.46f, 0.82f, 0.0f); // Verde claro clássico
    glBegin(GL_QUADS);
        glVertex2f(-12.0f, -2.3f); // Começo da grama
        glVertex2f( 12.0f, -2.3f);
        glVertex2f( 12.0f, -1.5f); // Topo liso do bloco
        glVertex2f(-12.0f, -1.5f);
    glEnd();

    // 2. DETALHES ESPETADOS DA GRAMA (Triângulos)
    float x_inicio = -12.0f;
    float x_fim = 12.0f;
    float passo = 0.4f; 
    
    glBegin(GL_TRIANGLES);
    for (float x = x_inicio; x < x_fim; x += passo) {
        // Os espetos agora brotam a partir do topo do bloco (Y = -1.5f)
        float y_base = -1.5f; 
        
        // Pequena variação para dar o efeito serrilhado clássico
        float altura1 = 0.25f + (vector_pos_x_fantasia((int)(x * 10)) * 0.05f); 
        float altura2 = 0.35f - (vector_pos_x_fantasia((int)(x * 5)) * 0.05f);

        // Primeiro espeto
        glVertex2f(x, y_base);
        glVertex2f(x + 0.2f, y_base);
        glVertex2f(x + 0.05f, y_base + altura1);

        // Segundo espeto
        glVertex2f(x + 0.15f, y_base);
        glVertex2f(x + 0.35f, y_base);
        glVertex2f(x + 0.28f, y_base + altura2);
    }
    glEnd();

    // 3. DETALHES DE SOMBRA (Risquinhos verde-escuros)
    // Posicionados dentro da nova faixa (entre -2.3f e -1.5f)
    glColor3f(0.1f, 0.45f, 0.0f); 
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        for (float gx = -11.0f; gx < 12.0f; gx += 2.5f) {
            // Risquinhos ajustados para o novo espaço
            glVertex2f(gx, -2.1f);        glVertex2f(gx, -1.8f);
            glVertex2f(gx + 0.4f, -2.0f);  glVertex2f(gx + 0.4f, -1.7f);
            glVertex2f(gx + 1.2f, -2.2f);  glVertex2f(gx + 1.2f, -1.9f);
        }
    glEnd();

    glPopMatrix();
}

   
//Desenha a Árvore Plana
void DesenhaArvorePlana(float base_x, float base_y) {
    glPushMatrix();
    glTranslatef(base_x, base_y, 0.0f);

    // Cor padrão do preenchimento e contorno do tronco
    float cor_tronco_r = 0.4f, cor_tronco_g = 0.2f, cor_tronco_b = 0.0f;
    float cor_borda_r = 0.15f, cor_borda_g = 0.08f, cor_borda_b = 0.0f;

    // ==========================================
    // 1. TRONCO E GALHOS (Estrutura Geométrica com Contorno)
    // ==========================================
    // Tronco Central
    glColor3f(cor_tronco_r, cor_tronco_g, cor_tronco_b);
    glBegin(GL_QUADS);
        glVertex2f(-0.4f, 0.0f);
        glVertex2f( 0.4f, 0.0f);
        glVertex2f( 0.2f, 3.2f);
        glVertex2f(-0.2f, 3.2f);
    glEnd();

    // Galho Esquerdo (Saindo para a diagonal)
    glBegin(GL_QUADS);
        glVertex2f(-0.2f, 2.4f);
        glVertex2f( 0.0f, 2.1f);
        glVertex2f(-1.1f, 3.8f);
        glVertex2f(-1.3f, 3.6f);
    glEnd();

    // Galho Direito (Saindo para a diagonal)
    glBegin(GL_QUADS);
        glVertex2f( 0.0f, 2.1f);
        glVertex2f( 0.2f, 2.4f);
        glVertex2f( 1.3f, 3.6f);
        glVertex2f( 1.1f, 3.8f);
    glEnd();

    // Linhas de Contorno da Madeira (Apenas para destacar o tronco)
    glColor3f(cor_borda_r, cor_borda_g, cor_borda_b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // Contorno esquerdo do tronco
        glVertex2f(-0.4f, 0.0f); glVertex2f(-0.2f, 3.2f);
        // Contorno direito do tronco
        glVertex2f( 0.4f, 0.0f); glVertex2f( 0.2f, 3.2f);
        // Contornos do galho esquerdo
        glVertex2f(-0.2f, 2.4f); glVertex2f(-1.3f, 3.6f);
        glVertex2f( 0.0f, 2.1f); glVertex2f(-1.1f, 3.8f);
        // Contornos do galho direito
        glVertex2f( 0.2f, 2.4f); glVertex2f( 1.3f, 3.6f);
        glVertex2f( 0.0f, 2.1f); glVertex2f( 1.1f, 3.8f);
    glEnd();

    // ==========================================
    // 2. COPA DA ÁRVORE (Cor Verde Pura e Sem Bordas)
    // ==========================================
    float verde_r = 0.1f;
    float verde_g = 0.55f;
    float verde_b = 0.1f;

    // Criando o volume com as 7 esferas que se fundem perfeitamente agora
    DesenhaCirculoSolido(-1.6f, 3.6f, 1.2f, verde_r, verde_g, verde_b); // Lateral Esquerda Baixa
    DesenhaCirculoSolido( 1.6f, 3.6f, 1.2f, verde_r, verde_g, verde_b); // Lateral Direita Baixa
    
    DesenhaCirculoSolido(-1.1f, 4.5f, 1.4f, verde_r, verde_g, verde_b); // Meio Esquerda
    DesenhaCirculoSolido( 1.1f, 4.5f, 1.4f, verde_r, verde_g, verde_b); // Meio Direita
    
    DesenhaCirculoSolido(-0.5f, 5.4f, 1.3f, verde_r, verde_g, verde_b); // Topo Esquerda
    DesenhaCirculoSolido( 0.5f, 5.4f, 1.3f, verde_r, verde_g, verde_b); // Topo Direita
    
    DesenhaCirculoSolido( 0.0f, 4.3f, 1.6f, verde_r, verde_g, verde_b); // Centro Grande Interno

    glPopMatrix();
}

void DesenhaNuvem(float base_x, float base_y) {
    glPushMatrix();

    glTranslatef(base_x, base_y, 0.0f);

    // Cor da Nuvem (Branco Puro)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Configurações geométricas (Agora o centro_y relativo é 0.0)
    int lados = 9;
    float raio = 1.0f;
    float centro_y = 0.0f;

    // NONÁGONO-E	
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-1.3f, centro_y); 
        for (int i = 0; i <= lados; i++) {
            float ang = i * 2.0f * 3.14159265f / lados;
            glVertex2f(-1.3f + cos(ang) * raio, centro_y + sin(ang) * raio);
        }
    glEnd();
    // NONÁGONO-C
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, centro_y); 
        for (int i = 0; i <= lados; i++) {
            float ang = i * 2.0f * 3.14159265f / lados;
            glVertex2f(0.0f + cos(ang) * raio, centro_y + sin(ang) * raio);
        }
    glEnd();
    //NONÁGONO-D
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(1.3f, centro_y); 
        for (int i = 0; i <= lados; i++) {
            float ang = i * 2.0f * 3.14159265f / lados;
            glVertex2f(1.3f + cos(ang) * raio, centro_y + sin(ang) * raio);
        }
    glEnd();
    glPopMatrix();
}

void DesenhaUI() {
    int i;
    char buffer[50];
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, g_largura, 0, g_altura, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Placar (Esquerda)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(15, g_altura - 30);
    sprintf(buffer, "SCORE: %d", g_score);
    i = 0;
    while (buffer[i]) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
        i++;
    }

    // Cronômetro (Direita superior)
    if (g_cronometro < 10.0f) {
        glColor3f(1.0f, 0.2f, 0.2f); 
    } else {
        glColor3f(1.0f, 1.0f, 0.0f); 
    }
    glRasterPos2i(g_largura - 150, g_altura - 30);
    sprintf(buffer, "TEMPO: %.1fs", g_cronometro);
    i = 0;
    while (buffer[i]) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
        i++;
    }

    // Tela de Fim de Jogo
    if (g_jogo_finalizado) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2i(g_largura / 2 - 100, g_altura / 2 + 20);
        char fim_msg[] = "TEMPO ESGOTADO!";
        i = 0;
        while (fim_msg[i]) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, fim_msg[i]);
            i++;
        }
        
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(g_largura / 2 - 70, g_altura / 2 - 15);
        char reset_msg[] = "Reiniciando...";
        i = 0;
        while (reset_msg[i]) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, reset_msg[i]);
            i++;
        }
    }

    if (g_foi_baleado && !g_jogo_finalizado) {
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

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
}

void DesenhaEixosComNumeracao() {
    glColor3f(0.65f, 0.65f, 0.65f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        for (int x = -12; x <= 12; x++) {
            glVertex2f((float)x, -5.0f);
            glVertex2f((float)x, 10.0f);
        }
        for (int y = -5; y <= 10; y++) {
            glVertex2f(-12.0f, (float)y);
            glVertex2f(12.0f, (float)y);
        }
    glEnd();

    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.5f, 0.0f);
        glVertex2f(-12.0f, 0.0f);
        glVertex2f(12.0f, 0.0f);

        glColor3f(0.8f, 0.0f, 0.0f);
        glVertex2f(0.0f, -5.0f);
        glVertex2f(0.0f, 10.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    for (int x = -10; x <= 10; x += 2) {
        glRasterPos2f((float)x, -0.5f); 
        char label[8];
        sprintf(label, "%d", x);
        for (int i = 0; label[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
        }
    }

    for (int y = -4; y <= 8; y += 2) {
        if (y == 0) continue; 
        glRasterPos2f(0.2f, (float)y); 
        char label[8];
        sprintf(label, "%d", y);
        for (int i = 0; label[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
        }
    }
}

void gMeusDesenhos() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-12.0, 12.0, -5.0, 10.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	DesenhaNuvem(2.5f, 7.5f);
    DesenhaNuvem(-5.0f, 8.0f);
	DesenhaArvorePlana(-8.0f, 1.3f); 
    DesenhaArvorePlana(8.0f, 1.3f); 
	DesenhaCenario();	
	DesenhaGrama();
	         
    DesenhaEixosComNumeracao();  
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
    if (altura == 0) altura = 1; 
    g_largura = largura; 
    g_altura = altura;
   
    glViewport (0, 0, largura, altura); 
}

void gTeclado (unsigned char tecla, int x, int y) {
    if(tecla == 27) exit(0);
}

void gEspeciais (int tecla, int x, int y) {
    glutPostRedisplay();
}

void gMouse (int botao, int estado, int x, int y) {
    if (g_jogo_finalizado) return;

    if (botao == GLUT_LEFT_BUTTON) {
        if (estado == GLUT_DOWN) {
            g_botao_pressionado = 1;
            
            if (g_pato_vivo && !g_foi_baleado) {
                int patoTelaX = (int)((g_pato_x + 12.0f) * (g_largura / 24.0f));
                int patoTelaY = (int)((g_pato_y + 5.0f) * (g_altura / 15.0f));
                
                float dx = (float)x - patoTelaX;
                float dy = (float)(g_altura - y) - patoTelaY;
                float dist = sqrt(dx*dx + dy*dy);
                
                if (dist < 25.0f) {  
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
    int tempo_atual = glutGet(GLUT_ELAPSED_TIME);
    float dt = (tempo_atual - g_tempo_anterior) / 1000.0f;
    g_tempo_anterior = tempo_atual;

    if (dt > 0.1f) dt = 0.1f;

    if (!g_jogo_finalizado) {
        g_cronometro -= dt; 
        
        if (g_cronometro <= 0.0f) {
            g_cronometro = 0.0f;
            g_jogo_finalizado = 1; 
        }

        if (!g_foi_baleado) {
            g_pato_x += g_pato_vel_x * dt;
            g_pato_y += g_pato_vel_y * dt;
            
            // Limite horizontal (esquerda/direita) - Mantém quicando nas laterais
            if (g_pato_x > 13.5f) { g_pato_x = 13.5f; g_pato_vel_x = -g_pato_vel_x; }
            if (g_pato_x < -13.5f) { g_pato_x = -13.5f; g_pato_vel_x = -g_pato_vel_x; }
            
            // ALTERAÇÃO AQUI: Limite vertical superior (Topo da tela)
            // Quando passa de 11.0f, desaparece e força o respawn lateral imediato
            if (g_pato_y > 11.0f) { 
                ResetaPato(); 
            }
            
            // Limite vertical inferior (Chão) - Mantém quicando para cima
            if (g_pato_y < -1.5f) { g_pato_y = -1.5f; g_pato_vel_y = -g_pato_vel_y; }
        } else {
            g_pato_y -= 12.0f * dt;
            if (g_pato_y < -2.0f) g_pato_y = -2.0f;

            static float acumulador_morte = 0;
            acumulador_morte += dt;
            if (acumulador_morte > 0.75f) { 
                ResetaPato();
                acumulador_morte = 0;
            }
        }
    } else {
        g_tempo_tela_final += dt;
        if (g_tempo_tela_final >= 2.0f) {
            ResetaJogoCompleto();
        }
    }
    
    glutPostRedisplay();
}

void gSistemaOcioso (void) {
    gTempoExecucao(0);
}

void gMousePressionado (int x, int y) {;}
void gMouseLiberado (int x, int y) {;}
void gMouseScroll (int botao, int direcao, int x, int y) {;}
void gMenuTeclado (void) {;}
void gMenuJanela (void) {;}

void gInicializa (void) {
    glDisable(GL_DEPTH_TEST); 
    glClearColor (0.53f, 0.81f, 0.92f, 1.0f); 
    srand((unsigned int)time(NULL));
    
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 
        (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(1); 
        printf("V-Sync ativated com sucesso!\n");
    } else {
        printf("Aviso: V-Sync nao suportado pelo driver.\n");
    }

    g_tempo_anterior = glutGet(GLUT_ELAPSED_TIME); 
    ResetaJogoCompleto(); 
}

int main (int argc, char *argv[]) {
    glutInit (&argc, argv); 
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); 
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
    
    return 0;
}
