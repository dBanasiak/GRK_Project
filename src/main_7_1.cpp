#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <Windows.h>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programTexture;

Core::Shader_Loader shaderLoader;

obj::Model fishModel;
obj::Model sandModel;

glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir;
glm::vec3 cameraSide;
float cameraAngle = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

glm::quat rotation = glm::quat(1, 0, 0, 0);

GLuint textureFish;
GLuint textureFishMain;
GLuint textureFishRed;
GLuint textureSand;

float x_pos = 3.0f;
float y_pos = 1.0f;
float z_pos = -60.0f;
float reverseRad = 0.0f;
float x_posCos = 1.0f;
float y_posCos = 1.0f;

void keyboard(unsigned char key, int x, int y)
{
	
	float angleSpeed = 0.2f;
	float moveSpeed = 0.2f;
	switch(key)
	{
	case 'q': cameraAngle -= angleSpeed; break;
	case 'e': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
}



glm::mat4 createCameraMatrix()
{
	cameraDir = glm::vec3(cosf(cameraAngle - glm::radians(90.0f)), 0.0f, sinf(cameraAngle - glm::radians(90.0f)));
	glm::vec3 up = glm::vec3(0, 1, 0);
	cameraSide = glm::cross(cameraDir, up);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}


void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

float changePos = 1.2f;

void renderScene()
{
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.4f, 1.0f);

	glm::mat4 fishInitialTransformation = glm::translate(glm::vec3(0,0.01f,0.01f)) * glm::rotate(glm::radians(180.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 fishModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::rotate(-cameraAngle, glm::vec3(0,10,0)) * fishInitialTransformation;
	drawObjectTexture(&fishModel, fishModelMatrix, textureFishMain);

	glm::mat4 sandInitialTransformation = glm::translate(glm::vec3(0, -15.0f, 0)) * glm::rotate(glm::radians(0.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(15.0f));
	glm::mat4 redFishTransformation = glm::translate(glm::vec3(x_pos, y_pos, z_pos)) * glm::rotate(glm::radians(reverseRad), glm::vec3(0, 1, 0))* glm::scale(glm::vec3(10.0f));
	glm::mat4 blueFishTransformation = glm::translate(glm::vec3(x_pos+14, y_pos+6, z_pos+3)) * glm::rotate(glm::radians(reverseRad), glm::vec3(0, 1, 0))* glm::scale(glm::vec3(15.0f));
	glm::mat4 pinkFishTransformation = glm::translate(glm::vec3(x_pos+7, y_pos, z_pos-1)) * glm::rotate(glm::radians(reverseRad), glm::vec3(0, 1, 0))* glm::scale(glm::vec3(8.0f));

	drawObjectTexture(&fishModel, redFishTransformation, textureFishRed);
	drawObjectTexture(&fishModel, blueFishTransformation, textureFishMain);
	drawObjectTexture(&fishModel, pinkFishTransformation, textureFish);

	drawObjectTexture(&sandModel, sandInitialTransformation, textureSand);

	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	fishModel = obj::loadModelFromFile("models/troutFish.obj");
	sandModel = obj::loadModelFromFile("models/sand.obj");
	textureFish = Core::LoadTexture("textures/fish.png");
	textureFishMain = Core::LoadTexture("textures/fish3.png");
	textureFishRed = Core::LoadTexture("textures/fishRed.png");
	textureSand = Core::LoadTexture("textures/sand.png");
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

void timer(int);

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Where is Nemo?");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutTimerFunc(0, timer,0);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}

bool move = false;

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);
	if (move == false) {
		if (z_pos <= 25) {
			x_pos += 0.002;
			y_pos += 0.0001f;
			z_pos += 0.1f;
			
		}
		else {
			move = true;
			reverseRad = 180.0f;
		}
	}
	else {
		if (z_pos >= -60.0f) {
			x_pos -= 0.002;
			y_pos -= 0.0001f;
			z_pos -= 0.1f;
			
		}
		else {
			move = false;
			reverseRad = 0;
		}
	}
	
}