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

l - limpa a tela e ajusta para o programa limpar a tela a cada vez que ocorre
uma mudanca de modo

r - volta ao desenho das retas, saindo de todos os modos

q - desenha quadrados com 2 cliques

t - desenha triangulos com 3 cliques

p - desenha poligonos com 4 ou mais cliques

c - desenha uma circunferencia com 2 cliques

f - ativa o modo do flood fill que usa o algoritmo de flood fill para pintar,
começa a partir do ponto clicado
Atenção!!! Se o ponto clicado não estiver dentro de nenhuma forma geométrica
pode travar o programa

g - Translacao
h - Escala
j - Rotacao
k - Cisalhamento
l - Reflexao

Comandos das transformacoes geometricas:

Translação(g):
Desloca todas as formas rasterizadas de acordo com o comando utilizado
	[ - Desloca 5 pixels para cima
	] - Desloca 5 pixels para baixo
	, - Desloca 5 pixels a esquerda
	. - Desloca 5 pixels a direita

Escala(h):
	[ - Escala 2x
	] - Escala 1.5x
	, - Escala 0.75x
	. - Escala 0.5x

Rotação(j):
	, - Rotaciona 5 graus a esquerda
	. - Rotaciona 5 graus a direita

Cisalhamento(k):

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
bool click1 = false, click2 = false, quadrado = false, circunferencia = false,
 triangulo = false, poligono = false, desenha = false, refreshScreen = false,
 raster_poligono = false, floodFill = false, translate = false, scale = false,
 rotate = false, shear = false, reflection = false;

double x_1, y_1, x_2, y_2;

int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
	int x;
	int y;
	ponto * prox;
};


struct vertex{
	int x;
	int y;
	char type;
	vertex * prox;
};

// struct edge{
// 	int ymax;
// 	int x-ymin;
// 	float m;
// 	int ymin;
// 	edge * prox;
// };
// 
// struct edgeList{
// 	edge * first;
// 	edge * last;
// };

// Lista encadeada de pontos
// indica o primeiro elemento da pilha
ponto * pontos = NULL;

//Lista encadeada usada para construir os triangulos
ponto * pontosTriangulo = NULL;

//Lista encadeada usada para construir os triangulos
ponto * pontosPoligono = NULL;

//Lista encadeada usada para armazenar os vértices das formas geométricas
vertex * pontosExtremos = NULL;


// Armazena como uma Pilha (empilha)
// void pushEdge(edgeList * list, int ymax, int x-ymin, float m, int ymin){
// 	if (list->first == NULL){
// 		edge * pnt = new edge;
// 		pnt->ymax = ymax;
// 		pnt->x-ymin = x-ymin;
// 		pnt->m = m;
// 		pnt->ymin = ymin;
// 		pnt->prox = NULL;
// 		list->first = pnt;
// 		list->last = pnt;
// 		return;		
// 	}
// 	edge * pnt = new edge;
// 	pnt->ymax = ymax;
// 	pnt->x-ymin = x-ymin;
// 	pnt->m = m;
// 	pnt->ymin = ymin;
// 	pnt->prox = NULL;
// 	list->last = pnt;
// 	return;
// }
// 
// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
// void popEdge(vertex * list){
// 	edge * pnt;
// 	pnt = NULL;
// 	if(list->last != NULL){
// 		pnt = pilha->prox;
// 		delete pilha;
// 		pilha = pnt;
// 	}
// 	return;
// }
// 
// Limpa a pilha, deixando-a sem nenhum elemento
// void freeEdges(vertex * list){
// 	edge * pnt = list->start;
// 	while (pnt != NULL){
// 		edge * prox = pnt->prox;
// 		delete pnt;
// 		pnt = prox;
// 	}
// }
// 
// Informa o tamanho da pilha
// int size(edgeList * list){
// 	int size = 0;
// 	edge * pnt = list->first;
// 	while (pnt != NULL){
// 		size += 1;
// 		pnt = pnt->prox;
// 	}
// 	return size;
// }

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
void retaBresenham(double x1, double x2, double y1, double y2);
void bresenhamCircumference(double x1, double x2, double Raio);
void polygonRasterAlgorithm();
void floodFillAlgorithm(double x, double y);
void drawSquare();
void drawTriangle();
void drawPolygon();
void translationPoints(int displace_x, int displace_y);
void rotatePoints(double angle);
void reflectionPoints(bool eixo_x, bool eixo_y);
void printMenu();

// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

// Funcao Principal do C
int main(int argc, char** argv){
	glutInit(&argc, argv); // Passagens de parametro C para o glut
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
	glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
	glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
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
		if (quadrado){
			if (refreshScreen){
				freePoints();
			}
			quadrado = false;
		}
		else{
			if (refreshScreen){
				freePoints();
				freeVertexes();		
			}
			pontosTriangulo = NULL, pontosPoligono = NULL;
			desenha = false, poligono = false, triangulo = false,
			circunferencia = false, floodFill = false, translate = false,
			scale = false, rotate = false, shear = false, reflection = false;
			quadrado = true;
		}
		break;
	case 116: // codigo letra t
		if (triangulo){
			if (refreshScreen){
				freePoints();	
			}
			triangulo = false;
		}
		else{
			if (refreshScreen){
				freePoints();
				freeVertexes();
			}
			pontosTriangulo = NULL, pontosPoligono = NULL;
			quadrado = false, desenha = false, poligono = false,
			circunferencia = false, floodFill = false, translate = false,
			scale = false, rotate = false, shear = false, reflection = false;
			triangulo = true;
		}
		break;
	case 112: // codigo letra p
		if (poligono){
			if (refreshScreen){
				freePoints();	
			}
			poligono = false;
		}
		else{
			if (refreshScreen){
				freePoints();
				freeVertexes();	
			}
			pontosTriangulo = NULL, pontosPoligono = NULL;
			quadrado = false, triangulo = false, desenha = false,
			circunferencia = false, floodFill = false, translate = false,
			scale = false, rotate = false, shear = false, reflection = false;
			poligono = true;
		}
		break;
	case 99: // codigo letra c
		if (circunferencia){
			if (refreshScreen){
				freePoints();	
			}
			circunferencia = false;
		}else{
			if (refreshScreen){
				freePoints();
				freeVertexes();	
			}
			pontosTriangulo = NULL, pontosPoligono = NULL;
			quadrado = false, triangulo = false, desenha = false,
			poligono = false, floodFill = false, translate = false,
			scale = false, rotate = false, shear = false, reflection = false;
			circunferencia = true;
		}
		break;
	case 102: // codigo letra f
		if (floodFill){
			if (refreshScreen){
				freePoints();	
			}
			floodFill = false;
		}else{
			freeVertexes();	
			pontosTriangulo = NULL, pontosPoligono = NULL;
			quadrado = false, triangulo = false, desenha = false,
			poligono = false, circunferencia = false, floodFill = false,
			translate = false, scale = false, rotate = false, shear = false,
			reflection = false;
			floodFill = true;
		}
		break;
	case 114: // codigo letra r
		if (refreshScreen){
			freePoints();
			freeVertexes();	
		}
		quadrado = false, triangulo = false, poligono = false,
		circunferencia = false, desenha = false, floodFill = false,
		translate = false, scale = false, rotate = false, shear = false,
		reflection = false;
		break;
	case 122: // codigo letra z
		if (refreshScreen){
			refreshScreen = false;
		}
		else{
			freePoints();	
			refreshScreen = true;
			display();
		}
		break;
	case 103: // codigo letra g
		if (translate){
			translate = false;
		}
		else{
			quadrado = false, triangulo = false, poligono = false,
			circunferencia = false, desenha = false, floodFill = false,
			scale = false, rotate = false, shear = false, reflection = false;
			translate = true;
			display();
		}
		break;
	case 104: // codigo letra h
		if (scale){
			scale = false;
		}
		else{
			quadrado = false, triangulo = false, poligono = false,
			circunferencia = false, desenha = false, floodFill = false,
			translate = false, rotate = false, shear = false, reflection = false;
			scale = true;
			display();
		}
		break;
	case 106: // codigo letra j
		if (rotate){
			rotate = false;
		}
		else{
			quadrado = false, triangulo = false, poligono = false,
			circunferencia = false, desenha = false, floodFill = false,
			translate = false, scale = false, shear = false, reflection = false;
			rotate = true;
			display();
		}
		break;
	case 107: // codigo letra k
		if (shear){
			shear = false;
		}
		else{
			quadrado = false, triangulo = false, poligono = false,
			circunferencia = false, desenha = false, floodFill = false,
			translate = false, scale = false, rotate = false, reflection = false;
			shear = true;
			display();
		}
		break;
	case 108: // codigo letra l
		if (reflection){
			reflection = false;
		}
		else{
			quadrado = false, triangulo = false, poligono = false,
			circunferencia = false, desenha = false, floodFill = false,
			translate = false, scale = false, rotate = false, shear = false;
			reflection = true;
			display();
		}
		break;
	
	// controle da rotacao/escala/translacao
	case 46: // codigo .
		if(translate){
			translationPoints(5, 0);
		}
		if (rotate){
			rotatePoints(-0.5);
		}
		if(reflection){
			reflectionPoints(true, true);
		}
		display();
		break;
	case 44: // codigo ,
		if(translate){
			translationPoints(-5, 0);
		}
		if (rotate){
			rotatePoints(0.5);
		}
		if(reflection){
			reflectionPoints(false, true);
		}
		display();
		break;
	case 91: // codigo [
		if(translate){
			translationPoints(0, 5);
		}
		if(reflection){
			reflectionPoints(true, false);
		}
		display();
		break;
	case 93: // codigo ]
		if(translate){
			translationPoints(0, -5);
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
				printf("x2y2(%.0f,%.0f)\n", x_2, y_2);
				glutPostRedisplay();
			}
			else{
				click1 = true;
				x_1 = x;
				y_1 = height - y;
				printf("x1y1(%.0f,%.0f)\n", x_1, y_1);
			}
		}
		display();
		break;
  //   case GLUT_MIDDLE_BUTTON:
  //   	if (state == GLUT_DOWN) {
		// 	glutPostRedisplay();
		// }
		// break;
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

	if(scale){
		
	}else if(shear){
		
	}else if (quadrado){
		drawSquare();
	}else if(triangulo){
		drawTriangle();
	}else if(poligono){
		drawPolygon();
	}else if(raster_poligono){
		polygonRasterAlgorithm();
	}else if(floodFill && (click1 || click2)){
		if (!click2){
			floodFillAlgorithm(x_1, y_1);
		}else{
			floodFillAlgorithm(x_2, y_2);			
		}
		click1 = false;
		click2 = false;
	}else if(circunferencia && click1 && click2){
		double Raio = round(sqrt(pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2)));
		bresenhamCircumference(x_1, y_1, Raio);
		click1 = false;
		click2 = false;
	}else if (click1 && click2){
		// pushVertex(pontosExtremos, x_1, y_1, 'f');
		// pushVertex(pontosExtremos, x_2, y_2, 'i');
		retaBresenham(x_1, y_1, x_2, y_2);
		click1 = false;
		click2 = false;
	}
	
	vertex * pnt = pontosExtremos;
	printf("Printando vertices:");
	for (int i = 0; i < size(pontosExtremos) && pnt != NULL; i++){
		printf("%d %d %c\n", pnt->x, pnt->y, pnt->type);
		pnt = pnt->prox;
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

void drawSquare(){
	if (click1 && click2){
		pontosExtremos = pushVertex(pontosExtremos, x_1, y_1, 'f');
		retaBresenham(x_1, y_1, x_1, y_2);
		pontosExtremos = pushVertex(pontosExtremos, x_1, y_2, 'm');
		retaBresenham(x_1, y_2, x_2, y_2);
		pontosExtremos = pushVertex(pontosExtremos, x_2, y_2, 'm');
		retaBresenham(x_2, y_2, x_2, y_1);
		pontosExtremos = pushVertex(pontosExtremos, x_2, y_1, 'i');
		retaBresenham(x_2, y_1, x_1, y_1);
		click1 = false;
		click2 = false;
	}
}

void drawTriangle(){
	if (click1 && size(pontosTriangulo) < 3){
		pontosTriangulo = pushPoint(pontosTriangulo, x_1, y_1);
		click1 = false;
	}
	if (size(pontosTriangulo) == 2){
		retaBresenham(pontosTriangulo->x, pontosTriangulo->y,
		pontosTriangulo->prox->x, pontosTriangulo->prox->y);
		return;
	}
	if (size(pontosTriangulo) >= 3){
		retaBresenham(pontosTriangulo->x, pontosTriangulo->y,
		pontosTriangulo->prox->x, pontosTriangulo->prox->y);
		double lastX = pontosTriangulo->x;
		double lastY = pontosTriangulo->y;
		ponto * pnt = pontosTriangulo;
		pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'f');
		for (int i = size(pontosTriangulo); pnt != NULL; i--){
			if (i == 1){
				retaBresenham(lastX, lastY, pnt->x, pnt->y);
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

void drawPolygon(){
	if (click1 && !desenha){
		printf(" Tamanho: %d\n", size(pontos));
		pontosPoligono = pushPoint(pontosPoligono, x_1, y_1);	
		click1 = false;
	}
	if (size(pontosPoligono) > 1){
		retaBresenham(pontosPoligono->x, pontosPoligono->y,
		pontosPoligono->prox->x, pontosPoligono->prox->y);
	}
	if (desenha){
		retaBresenham(pontosPoligono->x, pontosPoligono->y,
		pontosPoligono->prox->x, pontosPoligono->prox->y);
		double lastX = pontosPoligono->x;
		double lastY = pontosPoligono->y;
		ponto * pnt = pontosPoligono;
		pontosExtremos = pushVertex(pontosExtremos, pnt->x, pnt->y, 'f');
		for (int i = size(pontosPoligono); pnt != NULL; i--){
			if (i == 1){
				retaBresenham(lastX, lastY, pnt->x, pnt->y);
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

void bresenhamCircumference(double x1, double y1, double Raio){
	int xCentro = (int)x1;
	int yCentro = (int)y1;
	int R = (int)Raio, d = 1 - R, deltaE = 3, deltaSE = -2*R + 5, Xi = 0, Yi = R;
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

void retaBresenham(double x1, double y1, double x2, double y2){
	int deltaX = (int)x2 - x1;
	int deltaY = (int)y2 - y1;
	char avanco;

	pontos = pushPoint(pontos, x1, y1);
	pontos = pushPoint(pontos, x2, y2);

	// reducao ao primeiro octante
	bool simetrico = deltaX * deltaY < 0 ? true : false;
	if (simetrico){
		y1 *= -1;
		y2 *= -1;
		deltaX = (int)x2 - x1;
		deltaY = (int)y2 - y1;
	}
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

	// algoritmo de Bresenham para calcular os pontos
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
	// GLubyte cor = 255;
	// GLubyte novaCor = 0;
	drawPontos();
	GLubyte pixel[4];
	glReadPixels((int)x, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		pontos = pushPoint(pontos, (int)x, (int)y);
	}
	glReadPixels((int)x+1, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x+1, (int)y);
	}
	glReadPixels((int)x, (int)y-1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x, (int)y-1);
	}
	glReadPixels((int)x-1, (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x-1, (int)y);
	}
	glReadPixels((int)x, (int)y+1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255){
		floodFillAlgorithm((int)x, (int)y+1);
	}
}

void polygonRasterAlgorithm(){
	vertex * pnt = pontosExtremos;
	// int tam = 0;
	// vertex firstVertex;
	// double ta[100][4];
	// if (pnt->prox != NULL){
	// 	if (pnt->type == 'i'){
	// 		firstVertex = *pnt;
	// 	}
	// 	if (pnt->prox->type != 'i'){
	// 		tam++;
	// 		if (pnt->y > pnt->prox->y){
	// 			ta[i][0] = pnt->y;
	// 			ta[i][1] = pnt->prox->x;
	// 			ta[i][2] = (pnt->prox->x - pnt->x)/(pnt->prox->y - pnt->y);
	// 			ta[i][3] = pnt->prox->y;
	// 		}else if(pnt->y < pnt->prox->y){
	// 			ta[i][0] = pnt->prox->y;
	// 			ta[i][1] = pnt->x;
	// 			ta[i][2] = (pnt->prox->x - pnt->x)/(pnt->prox->y - pnt->y);
	// 			ta[i][3] = pnt->y;				
	// 		}
	// 	}else{
	// 		if (pnt->y > firstVertex->y){
	// 			ta[i][0] = pnt->y;
	// 			ta[i][1] = firstVertex->x;
	// 			ta[i][2] = (firstVertex->x - pnt->x)/(firstVertex->y - pnt->y);
	// 			ta[i][3] = firstVertex->y;
	// 		}else if(pnt->y < firstVertex->y){
	// 			ta[i][0] = firstVertex->y;
	// 			ta[i][1] = pnt->x;
	// 			ta[i][2] = (firstVertex->x - pnt->x)/(firstVertex->y - pnt->y);
	// 			ta[i][3] = pnt->y;				
	// 		}	
	// 	}
	// }
	// for (int i = 0; i < tam; i++){
	// 	
	// }
	// 
	// while (pnt != NULL){
	// 	while (pnt != NULL && pnt->type != 'f'){
	// 		
	// 	}
	// 	pnt = pnt->prox;
	// }
}

void translationPoints(int displace_x, int displace_y){
	ponto * pnt = pontos;
	while(pnt != NULL){
		pnt->x += displace_x;
		pnt->y += displace_y;
		pnt = pnt->prox;
	}
}

void rotatePoints(double angle){
	freePoints();
	vertex * pnt = pontosExtremos;
	vertex * vertInicio; 
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
	while(pnt != NULL){
		if (pnt->type == 'i'){
			vertInicio = pnt;
			retaBresenham(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'm'){
			retaBresenham(pnt->x, pnt->y, pnt->prox->x, pnt->prox->y);
		}
		if (pnt->type == 'f'){
			retaBresenham(pnt->x, pnt->y, vertInicio->x, vertInicio->y);
		}
		pnt = pnt->prox;
	}
}

void reflectionPoints(bool eixo_x, bool eixo_y){
	ponto * pnt = pontos;
	if (eixo_x && eixo_y){
		while(pnt != NULL){
			pnt->x *= -1;
			pnt->y *= -1;
			pnt = pnt->prox;
		}
		return;	
	}
	if (eixo_x){
		while(pnt != NULL){
			pnt->x *= -1;
			pnt = pnt->prox;
		}
		return;		
	}
	if (eixo_y){
		while(pnt != NULL){
			pnt->y *= -1;
			pnt = pnt->prox;
		}
		return;	
	}
}

void printMenu(){
	printf("MENU:\nTecla q: desenhar retangulos/quadrados\nTecla t: desenhar triangulos\n");
	printf("Tecla p: desenhar poligonos\nTecla r: limpa a tela se a limpeza estiver ativada e desenha retas\n");
	printf("Tecla l: ativa/desativa a limpeza da tela\n");
	printf("MODO: ");
	if (triangulo){
		printf("Desenhando triangulos\n");
	} else if (quadrado){
		printf("Desenhando quadrados\n");
	} else if (poligono){
		printf("Desenhando poligonos\n");
	} else if (circunferencia){
		printf("Desenhando circunferencias\n");
	} else{
		printf("Desenhando retas\n");
	}
	printf("Limpeza da tela: ");
	if (refreshScreen){
		printf("Verdadeiro\n");
	}else{
		printf("Falso\n");
	}
}
