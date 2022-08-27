// *********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica
//  Assunto: Rasterizacao e Transformacoes Geometricas
// 
//  Autor: Prof. Laurindo de Sousa Britto Neto
//  Aluno: Matheus Luís Webber
// *********************************************************************

/*
Comandos:
Cada tecla entra em um modo diferente, pra sair do modo atual basta apertar a
tecla de outro modo ou a tecla do modo atual

z - Limpa a tela

r - Retas: dois cliques, um para cada extremo da reta

q - Retangulos/Quadrados: dois cliques

t - Triangulos: tres cliques, um para cada vertice

p - Poligonos: numero indefinido de cliques no botao esquerdo do mouse, porem e necessario clicar uma vez no botao direito do mouse para fechar o poligono

c - Circunferencia: dois cliques, o centro e o raio

f - Flood fill para pintar: um clique, o ponto de onde o algoritmo iniciará
Atenção!!! Se o ponto clicado estiver exatamente na borda ou não estiver dentro de nenhuma forma geométrica o programa não vai mais funcionar

g - Translacao

h - Escala

j - Rotacao

k - Cisalhamento

l - Reflexao

Translacao(g):
	[ - Desloca 5 pixels para cima
	] - Desloca 5 pixels para baixo
	, - Desloca 5 pixels a esquerda
	. - Desloca 5 pixels a direita

Escala(h):
	[ - Escala 2x
	] - Escala 1.5x
	. - Escala 0.66x
	, - Escala 0.5x

Rotacao(j):
	, - Rotaciona no sentido anti-horario
	. - Rotaciona no sentido horario
Observacao: por algum motivo a rotacao diminui o tamanho da forma gradualmente ate\nsobrar apenas um ponto

Cisalhamento(k):
	, - Cisalhamento no eixo x com C = -1
	. - Cisalhamento no eixo x com C = 1
	[ - Cisalhamento no eixo y com C = 1
	] - Cisalhamento no eixo y com C = -1
Reflexao(l):
	[ - Reflexao com relacao ao eixo x
	, - Reflexao com relacao ao eixo y
	. - Reflexao com relacao a origem
*/

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <math.h>

// Variaveis Globais
bool click1 = false, click2 = false, desenha = false;
char modo = '-';

double x_1, y_1, x_2, y_2;

int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
	int x;
	int y;
	ponto * prox;
};

// Estrututa de dados para o armazenamento dinamico dos vertices
// das figuras geometricas criadas pelo programa
struct vertex{
	int x;
	int y;
	char type;
	vertex * prox;
};

// Lista encadeada de pontos
// indica o primeiro elemento da pilha
ponto * pontos = NULL;

//Lista encadeada usada para construir os triangulos
ponto * pontosTriangulo = NULL;

//Lista encadeada usada para construir os triangulos
ponto * pontosPoligono = NULL;

//Lista encadeada usada para armazenar os vértices das formas geométricas
vertex * pontosExtremos = NULL;

//Lista encadeada usada para armazenar o raio e o centro das circunferencias
vertex * pontosCicunferencias = NULL;

// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
vertex * pushVertex(vertex * pilha, int x, int y, char type){
	vertex * pnt = new vertex;
	pnt->x = x;
	pnt->y = y;
	pnt->type = type;
	pnt->prox = pilha;
	pilha = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
vertex * popVertex(vertex * pilha){
	vertex * pnt;
	pnt = NULL;
	if(pilha != NULL){
		pnt = pilha->prox;
		delete pilha;
		pilha = pnt;
	}
	return pnt;
}

// Limpa a pilha, deixando-a sem nenhum elemento
void freeVertexes(vertex * pilha){
	while (pilha != NULL){
		vertex * prox = pilha->prox;
		delete pilha;
		pilha = prox;
	}
}

void freeVertexes(){
	while (pontosExtremos != NULL){
		vertex * prox = pontosExtremos->prox;
		delete pontosExtremos;
		pontosExtremos = prox;
	}
	while (pontosCicunferencias != NULL){
		vertex * prox = pontosCicunferencias->prox;
		delete pontosCicunferencias;
		pontosCicunferencias = prox;
	}
}

// Informa o tamanho da pilha
int size(vertex * pnt){
	int size = 0;
	while (pnt != NULL){
		size += 1;
		pnt = pnt->prox;
	}
	return size;
}


// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto * pushPoint(ponto * pilha, int x, int y){
	ponto * pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = pilha;
	pilha = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto * popPoint(ponto * pilha){
	ponto * pnt;
	pnt = NULL;
	if(pilha != NULL){
		pnt = pilha->prox;
		delete pilha;
		pilha = pnt;
	}
	return pnt;
}

// Limpa a pilha, deixando-a sem nenhum elemento
void freePoints(){
	while (pontos != NULL){
		ponto * prox = pontos->prox;
		delete pontos;
		pontos = prox;
	}
}

// Informa o tamanho da pilha
int size(ponto * pnt){
	int size = 0;
	while (pnt != NULL){
		size += 1;
		pnt = pnt->prox;
	}
	return size;
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcoes criadas
void bresenhamStraightLine(double x1, double x2, double y1, double y2);
void bresenhamCircumference(double x1, double x2, double Raio, bool isNotReconstruction);
void floodFillAlgorithm(double x, double y);
void drawSquare();
void drawTriangle();
void drawPolygon();
void translatePoints(int displace_x, int displace_y);
void scalePoints(float scale_x, float scale_y);
void rotatePoints(double angle);
void shearPoints(double displace, char axis);
void reflectionPoints(bool eixo_x, bool eixo_y);
void printMenu();
void printModos();

// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

// Funcao Principal do C
int main(int argc, char** argv){
	glutInit(&argc, argv); // Passagens de parametro C para o glut
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
	glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
	glutInitWindowPosition (800, 100); //Posicao inicial da janela do OpenGL
	glutCreateWindow ("Rasterizacao"); // Da nome para uma janela OpenGL
	init(); // Chama funcao init();
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutMouseFunc(mouse); //funcao callback do mouse
	glutDisplayFunc(display); //funcao callback de desenho
	glutMainLoop(); // executa o loop do OpenGL
	return 0; // retorna 0 para o tipo inteiro da funcao main();
}

// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
	printMenu();
	glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

void reshape(int w, int h){
	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	width = w;
	height = h;
	glOrtho (0, w, 0, h, -1 , 1);

	// muda para o modo GL_MODELVIEW (nao pretendemos alterar a projecao
	// quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
	switch (key)   // key - variavel que possui valor ASCII da tecla precionada
	{
	case 27: // codigo ASCII da tecla ESC
		exit(0); // comando pra finalizacao do programa
		break;
	case 113: // codigo ASCII da tecla q
		if (modo == 'q'){
			modo = '-';
			printModos();
		}
		else{
			pontosTriangulo = NULL, pontosPoligono = NULL;
			modo = 'q';
			printModos();
		}
		break;
	case 116: // codigo letra t
		if (modo == 't'){
			modo = '-';
			printModos();
		}
		else{
			pontosTriangulo = NULL, pontosPoligono = NULL;
			modo = 't';
			printModos();
		}
		break;
	case 112: // codigo letra p
		if (modo == 'p'){
			modo = '-';
			printModos();
		}
		else{
			pontosTriangulo = NULL, pontosPoligono = NULL;
			modo = 'p';
			printModos();
		}
		break;
	case 99: // codigo letra c
		if (modo == 'c'){
			modo = '-';
			printModos();
		}else{
			modo = 'c';
			printModos();
		}
		break;
	case 102: // codigo letra f
		if (modo == 'f'){
			modo = '-';
			printModos();
		}else{
			pontosTriangulo = NULL, pontosPoligono = NULL;
			modo = 'f';
			printModos();
		}
		break;
	case 114: // codigo letra r
		modo = '-';
		printModos();
		break;
	case 122: // codigo letra z
		freePoints();
		freeVertexes();
		printf("Tela limpa\n");
		printModos();
		display();
		break;
	case 103: // codigo letra g
		if (modo == 'g'){
			modo = '-';
			printModos();
		}
		else{
			modo = 'g';
			printModos();
			display();
		}
		break;
	case 104: // codigo letra h
		if (modo == 'h'){
			modo = '-';
			printModos();
		}
		else{
			modo = 'h';
			printModos();
			display();
		}
		break;
	case 106: // codigo letra j
		if (modo == 'j'){
			modo = '-';
			printModos();
		}
		else{
			modo = 'j';
			printModos();
			display();
		}
		break;
	case 107: // codigo letra k
		if (modo == 'k'){
			modo = '-';
			printModos();
		}
		else{
			modo = 'k';
			printModos();
			display();
		}
		break;
	case 108: // codigo letra l
		if (modo == 'l'){
			modo = '-';
			printModos();
		}
		else{
			modo = 'l';
			printModos();
			display();
		}
		break;
	
	// controle da rotacao/escala/translacao
	case 46: // codigo .
		if(modo == 'g'){
			translatePoints(5, 0);
		}
		if (modo == 'h'){
			scalePoints(0.66, 0.66);
		}
		if (modo == 'j'){
			rotatePoints(-0.5);
		}
		if (modo == 'k'){
			shearPoints(-1, 'x');
		}
		if(modo == 'l'){
			reflectionPoints(true, true);
		}
		display();
		break;
	case 44: // codigo ,
		if(modo == 'g'){
			translatePoints(-5, 0);
		}
		if (modo == 'h'){
			scalePoints(0.5, 0.5);
		}
		if (modo == 'j'){
			rotatePoints(0.5);
		}
		if (modo == 'k'){
			shearPoints(1, 'x');
		}
		if(modo == 'l'){
			reflectionPoints(false, true);
		}
		display();
		break;
	case 91: // codigo [
		if(modo == 'g'){
			translatePoints(0, 5);
		}
		if (modo == 'h'){
			scalePoints(2, 2);
		}
		if (modo == 'k'){
			shearPoints(1, 'y');
		}
		if(modo == 'l'){
			reflectionPoints(true, false);
		}
		display();
		break;
	case 93: // codigo ]
		if(modo == 'g'){
			translatePoints(0, -5);
		}
		if (modo == 'h'){
			scalePoints(1.5, 1.5);
		}
		if (modo == 'k'){
			shearPoints(-1, 'y');
		}
		display();
		break;
	}
}

// Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y){
	switch (button){
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN){
			if(click1){
				click2 = true;
				x_2 = x;
				y_2 = height - y;
				glutPostRedisplay();
			}
			else{
				click1 = true;
				x_1 = x;
				y_1 = height - y;
			}
		}
		display();
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			if (desenha){
				desenha = false;
			} else{
				desenha = true;
				display();
			}
			glutPostRedisplay();
		}
		break;
	default:
		break;
	}
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
	glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
	glLoadIdentity();

	glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto

	// Verificacao de qual modo está ativado atualmente para chamar a funcao
	// correta
	if (modo == 'q'){
		drawSquare();
	}else if(modo == 't'){
		drawTriangle();
	}else if(modo == 'p'){
		drawPolygon();
	}else if(modo == 'f' && (click1 || click2)){
		if (!click2){
			floodFillAlgorithm(x_1, y_1);
		}else{
			floodFillAlgorithm(x_2, y_2);			
		}
		click1 = false;
		click2 = false;
	}else if(modo == 'c' && click1 && click2){
		double Raio = round(sqrt(pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2)));
		bresenhamCircumference(x_1, y_1, Raio, true);
		click1 = false;
		click2 = false;
	}else if (click1 && click2){
		pontosExtremos = pushVertex(pontosExtremos, x_1, y_1, 'f');
		pontosExtremos = pushVertex(pontosExtremos, x_2, y_2, 'i');
		bresenhamStraightLine(x_1, y_1, x_2, y_2);
		click1 = false;
		click2 = false;
	}
	
	drawPontos();	
	glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

// Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(){
	ponto * pnt;
	pnt = pontos;
	glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
	while(pnt != NULL){
		glVertex2i(pnt->x, pnt->y);
		pnt = pnt->prox;
	}
	glEnd();  // indica o fim do desenho
}

// Funcao que utiliza as coordenadas de dois pontos para desenhar um retangulo
void drawSquare(){
	if (click1 && click2){
		pontosExtremos = pushVertex(pontosExtremos, x_1, y_1, 'f');
		bresenhamStraightLine(x_1, y_1, x_1, y_2);
		pontosExtremos = pushVertex(pontosExtremos, x_1, y_2, 'm');
		bresenhamStraightLine(x_1, y_2, x_2, y_2);
		pontosExtremos = pushVertex(pontosExtremos, x_2, y_2, 'm');
		bresenhamStraightLine(x_2, y_2, x_2, y_1);
		pontosExtremos = pushVertex(pontosExtremos, x_2, y_1, 'i');
		bresenhamStraightLine(x_2, y_1, x_1, y_1);
		click1 = false;
		click2 = false;
	}
}

// Funcao que utiliza as coordenadas de 3 pontos para desenhar um triangulo
void drawTriangle(){
	if (click1 && size(pontosTriangulo) < 3){
		pontosTriangulo = pushPoint(pontosTriangulo, x_1, y_1);
		click1 = false;
	}
	if (size(pontosTriangulo) == 2){
		bresenhamStraightLine(pontosTriangulo->x, pontosTriangulo->y,
		pontosTriangulo->prox->x, pontosTriangulo->prox->y);
		return;
	}
	if (size(pontosTriangulo) >= 3){
		bresenhamStraightLine(pontosTriangulo->x, pontosTriangulo->y,
		pontosTriangulo->prox->x, pontosTriangulo->prox->y);
		double lastX = pontosTriangulo->x;
		double lastY = pontosTriangulo->y;
		ponto * pnt = pontosTriangulo;
		pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'f');
		for (int i = size(pontosTriangulo); pnt != NULL; i--){
			if (i == 1){
				bresenhamStraightLine(lastX, lastY, pnt->x, pnt->y);
				pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'i');
				pnt = popPoint(pnt);
				continue;
			}
			pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'm');
			pnt = popPoint(pnt);
		}
		pontosTriangulo = NULL;
		click1 = false;
		click2 = false;
		return;
	}
}

// Função que utiliza as coordenadas de um numero indefinido de pontos para
// construir um poligono, um clique no botao direito do mouse é usado para
// fechar o poligono
void drawPolygon(){
	if (click1 && !desenha){
		pontosPoligono = pushPoint(pontosPoligono, x_1, y_1);	
		click1 = false;
	}
	if (size(pontosPoligono) > 1){
		bresenhamStraightLine(pontosPoligono->x, pontosPoligono->y,
		pontosPoligono->prox->x, pontosPoligono->prox->y);
	}
	if (desenha){
		bresenhamStraightLine(pontosPoligono->x, pontosPoligono->y,
		pontosPoligono->prox->x, pontosPoligono->prox->y);
		double lastX = pontosPoligono->x;
		double lastY = pontosPoligono->y;
		ponto * pnt = pontosPoligono;
		pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'f');
		for (int i = size(pontosPoligono); pnt != NULL; i--){
			if (i == 1){
				bresenhamStraightLine(lastX, lastY, pnt->x, pnt->y);
				pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'i');
				pnt = popPoint(pnt);
				continue;
			}
			pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'm');
			pnt = popPoint(pnt);
		}
		pontosPoligono = NULL;
		desenha = false;
		click1 = false;
		click2 = false;
		return;
	}
}

// Algoritmo de Bresenham para rasterização de circunferencias
void bresenhamCircumference(double x1, double y1, double raio, bool isNotReconstruction){
	int xCentro = (int)x1;
	int yCentro = (int)y1;
	if(isNotReconstruction){
		pontosCicunferencias = pushVertex(pontosCicunferencias, raio, 0, 'r');
		pontosCicunferencias = pushVertex(pontosCicunferencias, x1, y1, 'c');		
	}
	int R = (int)raio, d = 1 - R, deltaE = 3, deltaSE = -2*R + 5, Xi = 0, Yi = R;
	pontos = pushPoint(pontos, xCentro, R + yCentro);
	pontos = pushPoint(pontos, R + xCentro, yCentro);
	pontos = pushPoint(pontos, xCentro, -1*R + yCentro);
	pontos = pushPoint(pontos, -1*R + xCentro, yCentro);
	Xi++;
	while (Xi < Yi){
		if (d < 0){
			d += deltaE;
			deltaE += 2;
			deltaSE += 2;
		}else{
			d += deltaSE;
			deltaE += 2;
			deltaSE += 4;
			Yi--;
		}
		// A rasterização da circunferencia do primeiro quadrante foi refletida
		// nos demais quadrantes para rasterizar a circunferência completa
		pontos = pushPoint(pontos, Xi + xCentro, Yi + yCentro);
		pontos = pushPoint(pontos, Yi + xCentro, Xi + yCentro);
		pontos = pushPoint(pontos, Xi + xCentro, -1*Yi + yCentro);
		pontos = pushPoint(pontos, Yi + xCentro, -1*Xi + yCentro);
		pontos = pushPoint(pontos, -1*Xi + xCentro, -1*Yi + yCentro);
		pontos = pushPoint(pontos, -1*Yi + xCentro, -1*Xi + yCentro);
		pontos = pushPoint(pontos, -1*Xi + xCentro, Yi + yCentro);
		pontos = pushPoint(pontos, -1*Yi + xCentro, Xi + yCentro);
		Xi++;
	}
}

// Algoritmo de rasterização de retas de Bresenham
void bresenhamStraightLine(double x1, double y1, double x2, double y2){
	int deltaX = (int)x2 - x1;
	int deltaY = (int)y2 - y1;
	char avanco;

	pontos = pushPoint(pontos, x1, y1);
	pontos = pushPoint(pontos, x2, y2);

	// Reducao ao primeiro octante
	// Verifica se delta x vezes delta y é negativo
	bool simetrico = deltaX * deltaY < 0 ? true : false;
	if (simetrico){
		y1 *= -1;
		y2 *= -1;
		deltaX = (int)x2 - x1;
		deltaY = (int)y2 - y1;
	}
	// Verifica se o modulo de delta x é maior que o modulo de delta y
	bool declive = (deltaX < 0 ? -1 * deltaX : deltaX) < (deltaY < 0 ? -1 * deltaY : deltaY) ? true : false;
	if (declive){
		double temp = x1;
		x1 = y1;
		y1 = temp;
		temp = x2;
		x2 = y2;
		y2 = temp;
		deltaX = (int)x2 - x1;
		deltaY = (int)y2 - y1;
	}
	// Verifica se o valor de x1 é maior que o de x2
	if (x1 > x2){
		double temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		deltaX = (int)x2 - x1;
		deltaY = (int)y2 - y1;
	}

	int d = 2 * deltaY - deltaX;
	int incE = 2 * deltaY;
	int incNE = 2 * deltaY - 2 * deltaX;

	double xi = x1;
	double yi = y1;

	// Algoritmo de Bresenham para rasterizar a reta
	for (int i = 1; i < deltaX; i++){
		avanco = d > 0 ? 'N' : 'E';
		d = avanco == 'E' ? d + incE : d + incNE;

		xi += 1;
		yi = avanco == 'E' ? yi : yi + 1;
		int tempXi = (int)xi;
		int tempYi = (int)yi;
		if (declive)
		{
			int temp = tempXi;
			tempXi = tempYi;
			tempYi = temp;
		}
		if (simetrico){
			tempYi *= -1;
		}
		pontos = pushPoint(pontos, tempXi, tempYi);
	}
}

void floodFillAlgorithm(double x, double y){
	drawPontos();
	glutPostRedisplay();
	glutSwapBuffers();
	GLubyte pixel[4];
	// Verifica se o pixel atual é um pixel branco para pintá-lo de preto
	glReadPixels((int)x, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		pontos = pushPoint(pontos, (int)x, (int)y);
	}
	// Verifica se o vizinho direito do pixel atual
	// é um pixel branco para chamar a função para esse vizinho
	glReadPixels((int)x+1, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x+1, (int)y);
	}
	// Verifica se o vizinho de baixo do pixel atual
	// é um pixel branco para chamar a função para esse vizinho
	glReadPixels((int)x, (int)y-1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x, (int)y-1);
	}
	// Verifica se o vizinho esquerdo do pixel atual
	// é um pixel branco para chamar a função para esse vizinho
	glReadPixels((int)x-1, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x-1, (int)y);
	}
	// Verifica se o vizinho de cima do pixel atual
	// é um pixel branco para chamar a função para esse vizinho
	glReadPixels((int)x, (int)y+1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x, (int)y+1);
	}
}

// Função de translação
void translatePoints(int displace_x, int displace_y){
	ponto * pnt = pontos;
	// Soma os valores de deslocamento com as coordenadas x e y de cada
	// ponto das formas geométricas rasterizadas
	while(pnt != NULL){
		pnt->x += displace_x;
		pnt->y += displace_y;
		pnt = pnt->prox;
	}
}

// Função de escala
void scalePoints(float scale_x, float scale_y){
	freePoints();
	vertex * pnt = pontosCicunferencias;
	vertex * vertInicio; 
	//Reconstroi as circunferencias, se tiver
	while(pnt != NULL){
		if(pnt->type == 'c'){
			pnt->prox->x *= scale_x;
			bresenhamCircumference(pnt->x, pnt->y, pnt->prox->x, false);
		}
		pnt = pnt->prox;
	}
	// Varre a lista de vértices das formas geométricas subtraindo as
	// as coordenadas no primeiro vértice desenhado dos demais vértices do
	// poligono, o vertice inicial não é modificado
	pnt = pontosExtremos;
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type == 'm'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		if (pnt->type == 'f'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista de vértices fazendo a escala ponto a ponto
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type != 'i'){
			double x = pnt->x;
			double y = pnt->y;
			pnt->x = x*scale_x;
			pnt->y = y*scale_y;
			pnt->x += vertInicio->x;
			pnt->y += vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista reconstruindo a forma geométrica que foi aumentada/diminuida,
	// recriando as arestas entre os vértices
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'm'){
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'f'){
			bresenhamStraightLine(pnt->x, pnt->y, vertInicio->x, vertInicio->y);
		}
		pnt = pnt->prox;
	}
}

void rotatePoints(double angle){
	freePoints();
	vertex * pnt = pontosCicunferencias;
	vertex * vertInicio; 
	//Reconstroi as circunferencias, se tiver
	while(pnt != NULL){
		if(pnt->type == 'c'){
			bresenhamCircumference(pnt->x, pnt->y, pnt->prox->x, false);
		}
		pnt = pnt->prox;
	}
	// Varre a lista de vértices das formas geométricas subtraindo as
	// as coordenadas no primeiro vértice desenhado dos demais vértices do
	// poligono, o vertice inicial não é modificado
	pnt = pontosExtremos;
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type == 'm'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		if (pnt->type == 'f'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista de vértices fazendo a rotacao ponto a ponto
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type != 'i'){
			double x = pnt->x;
			double y = pnt->y;
			pnt->x = (x*cos(angle))-(y*sin(angle));
			pnt->y = (x*sin(angle))+(y*cos(angle));
			pnt->x += vertInicio->x;
			pnt->y += vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista reconstruindo a forma geométrica que foi rotacionada,
	// recriando as arestas entre os vértices
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'm'){
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'f'){
			bresenhamStraightLine(pnt->x, pnt->y, vertInicio->x, vertInicio->y);
		}
		pnt = pnt->prox;
	}
}

void shearPoints(double displace, char axis){
	freePoints();
	vertex * pnt = pontosCicunferencias;
	vertex * vertInicio; 
	//Reconstroi as circunferencias, se tiver
	while(pnt != NULL){
		if(pnt->type == 'c'){
			bresenhamCircumference(pnt->x, pnt->y, pnt->prox->x, false);
		}
		pnt = pnt->prox;
	}
	// Varre a lista de vértices das formas geométricas subtraindo as
	// as coordenadas no primeiro vértice desenhado dos demais vértices do
	// poligono, o vertice inicial não é modificado
	pnt = pontosExtremos;
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type == 'm'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		if (pnt->type == 'f'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista de vértices fazendo o cisalhamento ponto a ponto
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type != 'i'){
			double x = pnt->x;
			double y = pnt->y;
			if (axis == 'x'){
				pnt->x += displace*y;
				pnt->x += vertInicio->x;
				pnt->y += vertInicio->y;				
			}
			if (axis == 'y'){
				pnt->y += displace*x;
				pnt->x += vertInicio->x;
				pnt->y += vertInicio->y;				
			}
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista reconstruindo a forma geométrica que foi cisalhada,
	// recriando as arestas entre os vértices
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'm'){
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'f'){
			bresenhamStraightLine(pnt->x, pnt->y, vertInicio->x, vertInicio->y);
		}
		pnt = pnt->prox;
	}
}

void reflectionPoints(bool eixo_x, bool eixo_y){
	freePoints();
	vertex * pnt = pontosCicunferencias;
	vertex * vertInicio; 
	//Reconstroi as circunferencias, se tiver
	while(pnt != NULL){
		if(pnt->type == 'c'){
			bresenhamCircumference(pnt->x, pnt->y, pnt->prox->x, false);
		}
		pnt = pnt->prox;
	}
	// Varre a lista de vértices das formas geométricas subtraindo as
	// as coordenadas no primeiro vértice desenhado dos demais vértices do
	// poligono, o vertice inicial não é modificado
	pnt = pontosExtremos;
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type == 'm'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		if (pnt->type == 'f'){
			pnt->x -= vertInicio->x;
			pnt->y -= vertInicio->y;
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista de vértices fazendo a reflexao ponto a ponto
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
		}
		if (pnt->type != 'i'){
			if (eixo_x && eixo_y){
				pnt->x *= -1;
				pnt->y *= -1;
				pnt->x += vertInicio->x;
				pnt->y += vertInicio->y;
			}else if (eixo_x){
				pnt->x *= -1;
				pnt->x += vertInicio->x;
				pnt->y += vertInicio->y;	
			}else if (eixo_y){
				pnt->y *= -1;
				pnt->x += vertInicio->x;
				pnt->y += vertInicio->y;
			}
		}
		pnt = pnt->prox;
	}
	
	pnt = pontosExtremos;
	// Varre a lista reconstruindo a forma geométrica que foi refletida,
	// recriando as arestas entre os vértices
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'm'){
			bresenhamStraightLine(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'f'){
			bresenhamStraightLine(pnt->x, pnt->y, vertInicio->x, vertInicio->y);
		}
		pnt = pnt->prox;
	}
}

void printMenu(){
	printf("COMANDOS:\n");
	printf("Cada tecla entra em um modo diferente, pra sair do modo atual basta apertar a tecla\nde outro modo ou a tecla do modo atual\n");
	printf("z - Limpa a tela\n\n");
	printf("r - Retas: dois cliques, um para cada extremo da reta\n\n");
	printf("q - Retangulos/Quadrados: dois cliques\n\n");
	printf("t - Triangulos: tres cliques, um para cada vertice\n\n");
	printf("p - Poligonos: numero indefinido de cliques no botao esquerdo do mouse, e\num clique no botao direito do mouse para fechar o poligono\n\n");
	printf("c - Circunferencia: dois cliques, o centro e o raio\n\n");
	printf("f - Flood fill para pintar: um clique, o ponto de onde o algoritmo iniciara\n");
	printf("Observacao 1: Se o ponto clicado estiver exatamente na borda ou nao estiver dentro de\nnenhuma forma geometrica o programa nao vai mais funcionar\n");
	printf("Observacao 2: Caso seja feita alguma transformacao geometrica nas figuras pintadas\nelas serao rasterizadas sem a pintura\n");
	printf("\nCOMANDOS TRANSFORMACOES:\n");
	printf("g - Translacao\n\n");
	printf("h - Escala\n\n");
	printf("j - Rotacao\n\n");
	printf("k - Cisalhamento\n\n");
	printf("l - Reflexao\n\n");
}

void printModos(){
	printf("MODO: ");
	if (modo == 't'){
		printf("Triangulos\n");
	}else if (modo == 'q'){
		printf("Quadrados\n");
	}else if (modo == 'p'){
		printf("Poligonos\n");
	}else if (modo == 'c'){
		printf("Circunferencias\n");
	}else if(modo == 'g'){
		printf("Translacao(g):\n");
		printf("	[ - Desloca 5 pixels para cima\n");
		printf("	] - Desloca 5 pixels para baixo\n");
		printf("	, - Desloca 5 pixels a esquerda\n");
		printf("	. - Desloca 5 pixels a direita\n");
	}else if(modo == 'h'){
		printf("Escala(h):\n");
		printf("	[ - Escala 2x\n");
		printf("	] - Escala 1.5x\n");
		printf("	. - Escala 0.66x\n");
		printf("	, - Escala 0.5x\n");
	}else if(modo == 'j'){
		printf("Rotacao(j):\n");
		printf("	, - Rotaciona no sentido anti-horario\n");
		printf("	. - Rotaciona no sentido horario\n");
		printf("Observacao: por algum motivo a rotacao diminui o tamanho da forma gradualmente ate\nsobrar apenas um ponto\n");
	}else if(modo == 'k'){
		printf("Cisalhamento(k):\n");
		printf("	, - Cisalhamento no eixo x com C = -1\n");
		printf("	. - Cisalhamento no eixo x com C = 1\n");
		printf("	[ - Cisalhamento no eixo y com C = 1\n");
		printf("	] - Cisalhamento no eixo y com C = -1\n");
	}else if(modo == 'l'){
		printf("Reflexao(l):\n");
		printf("	[ - Reflexao com relacao ao eixo x\n");
		printf("	, - Reflexao com relacao ao eixo y\n");
		printf("	. - Reflexao com relacao a origem\n");
	}else if(modo == '-'){
		printf("Retas\n");
	}else if(modo == 'f'){
		printf("FloofFill\n");
	}
}
