
#define STB_IMAGE_IMPLEMENTATION	// Para cargar imágenes
#include <stdio.h>					// .
#include <string.h>					// .
#include <cmath>					// .
#include <vector>					// .
#include <math.h>					// .
#include <glew.h>					// .
#include <glfw3.h>					// .
#include <glm.hpp>					// .
#include <gtc/matrix_transform.hpp>	// .
#include <gtc/type_ptr.hpp>			// .
#include "Window.h"					// Limpieza en el código
#include "Mesh.h"					// .
#include "Shader_light.h"			// .
#include "Camera.h"					// . 
#include "Texture.h"				// .
#include "Sphere.h"					// .
#include "Model.h"					// .
#include "Skybox.h"					// .
#include "CommonValues.h"			// .
#include "DirectionalLight.h"		// Iluminación
#include "PointLight.h"				// .
#include "SpotLight.h"				// .
#include "Material.h"				// .
#include "ResourceManager.h"		// Recursos
//#include<assimp/Importer.hpp>		// Para probar el importer

// ------------------------------------------------------------------ Ventana
Window mainWindow;

std::vector<Mesh*> meshList;		// Vector de Meshs
std::vector<Shader> shaderList;		// Vector de Shaders

// ------------------------------------------------------------------ Camara
Camera camera;

static double limitFPS = 1.0 / 60.0;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// ------------------------------------------------------------------ Shader
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// ------------------------------------------------------------------ Material
Material Material_brillante;
Material Material_opaco;

// ------------------------------------------------------------------ Iluminacion
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

DirectionalLight mainLight;					// Luz Direccional (Sol)
PointLight PL_0[MAX_POINT_LIGHTS];			// Arreglos de luces PointLight
SpotLight PL_1[MAX_SPOT_LIGHTS];			//.
SpotLight SP_RGB[MAX_SPOT_LIGHTS];			// Arreglos de luces SpotLight
SpotLight SP_lamparas[MAX_SPOT_LIGHTS];		//.

// ------------------------------------------------------------------ Funciones

// Función - Cálculo de Normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
  		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

// Función - Creamos las primitivas
void CreatePrimitives()
{
	// --------------------------------------------------------- Suelo
	GLfloat vertices_Suelo[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-10.0f, 0.0f, -10.0f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 0 - arriba a la izquierda
			10.0f, 0.0f, -10.0f,	1.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 1 - arriba a la derecha
			-10.0f, 0.0f, 10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f, // indice 2 - abajo a la izquierda
			10.0f, 0.0f, 10.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f  // indice 3 - abajo a la derecha
	};
	unsigned int indices_Suelo[] = {
		0, 2, 1,
		1, 2, 3
	};

	Mesh* Suelo = new Mesh();
	Suelo->CreateMesh(vertices_Suelo, indices_Suelo, 32, 6);
	meshList.push_back(Suelo);

	// --------------------------------------------------------- Vegetación
	GLfloat vertices_Vegetacion[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

			0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	unsigned int indices_Vegetacion[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	Mesh* Vegetacion = new Mesh();
	Vegetacion->CreateMesh(vertices_Vegetacion, indices_Vegetacion, 64, 12);
	meshList.push_back(Vegetacion);

	calcAverageNormals(indices_Vegetacion, 12, vertices_Vegetacion, 64, 8, 5);
}

// Función - Creamos los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// Función - Creamos la iluminación
void SetLights()
{
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(
		0.83f, 0.78f, 0.92f,	// Color
		1.0f, 0.3f,				// Intensidades ambiente y difusa más bajas
		-1.0f, 0.0f, 0.0f		// Dirección
	);

	// Arreglo 1 para cambio de luces
	// Verde
	SP_lamparas[0] = SpotLight(0.0f, 1.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		20.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.0f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Azul
	SP_lamparas[1] = SpotLight(0.0f, 0.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		10.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.01f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Roja
	SP_lamparas[2] = SpotLight(1.0f, 0.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		0.0f, 15.0f, 0.0f,                      // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.0f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Blanca
	SP_lamparas[3] = SpotLight(1.0f, 1.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		-10.0f, 15.0f, 0.0f,                    // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.01f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;

	// Arreglo 2 para cambio de luces
	// Blanca
	SP_RGB[0] = SP_lamparas[3];
	// Verde
	SP_RGB[1] = SP_lamparas[0];
	// Azul
	SP_RGB[2] = SP_lamparas[1];
	// Roja
	SP_RGB[3] = SP_lamparas[2];
}

// Función - Renderizar modelo
void RenderModel(glm::mat4 model, GLuint uniformModel, glm::vec3 origen, glm::vec3 pos, Model& modelRender)
{
	model = glm::translate(model, origen + pos);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	modelRender.RenderModel();
}

// Función - Renderizar mesh
void RenderMeshWithTexture(Mesh* mesh, glm::vec3 color, glm::vec3 position, glm::vec3 scale,
	GLuint uniformModel, GLuint uniformColor, GLuint uniformSpecularIntensity, GLuint uniformShininess,
	Material* material = nullptr, Texture* texture = nullptr)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);                    // Reiniciar la matriz
	model = glm::translate(model, position);	// Posición
	model = glm::scale(model, scale);			// Escala

	if (texture != nullptr)
		texture->UseTexture();

	if (material != nullptr)
		material->UseMaterial(uniformSpecularIntensity, uniformShininess);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));

	mesh->RenderMesh();
}

// -------------------------------------------------------------------------- Main
int main()
{
	// Iniciamos el contexto
	mainWindow = Window(1366, 768); // Resolución: 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	// Recursos
	ResourceManager resources;

	CreatePrimitives();				// Cargamos las primitivas en meshList
	CreateShaders();				// Creamos los shaders
	SetLights();					// Creamos las luces

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// Variables uniform
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, 
			uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;

	// Vector de proyección
	glm::mat4 projection = glm::perspective(
			45.0f,				// Ángulo de vision
			(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), // Aspecto
			0.1f, 2800.0f);		// Alcance: cerca | lejos
	
	// Matrices para los modelos
	glm::mat4 modelBase(1.0);
	glm::mat4 modelZoo(1.0);		// Matriz para los animales
	glm::mat4 modelChicken(1.0);	// Matriz para los modelos de Chicken Little
	glm::mat4 modelTotoro(1.0);		// Matriz para los modelos de Totoro
	glm::mat4 modelChilly(1.0);		// Matriz para los modelos de Chilly Willy
	glm::mat4 modelRikoche(1.0);	// Matriz para los modelos de Rikoche

	// Posicion del origen
	glm::vec3 origen = glm::vec3(0.0f, 0.0f, 0.0f);

	// Posiciones de cada cuadrante
	glm::vec3 origenChicken = glm::vec3(-680.0f, 0.0f, -690.0f);// Cuadrante superior izq
	glm::vec3 origenChilly = glm::vec3(-690.0f, 0.0f, 690.0f);	// Cuadrante inferior izq
	glm::vec3 origenRikoche = glm::vec3(720.0f, 0.0f, 700.0f);	// Cuadrante inferior der
	glm::vec3 origenTotoro = glm::vec3(690.0f, 0.0f, -690.0f);	// Cuadrante superior der

	// Posiciones para los animales
	std::vector<glm::vec3> posAnimals;

	// Variables auxiliares
	glm::vec3 color = glm::vec3(1.0f);		// Para mandar un color
	glm::vec3 pos = glm::vec3(1.0f);		// Para mover los modelos
	glm::vec3 scal = glm::vec3(1.0f);		// Para escalar los modelos
	float rot = 0.0f;						// Para rotar los modelos
	
	// Ajustes para la cámara
	camera = Camera(
		origen + glm::vec3(0.0f, 100.0f, 100.0f),// Posicion inicial
		glm::vec3(0.0f, 1.0f, 0.0f),			// Direccion de nuestro "arriba"
		-60.0f, 0.0f,						// Rotacion horizontal | Rotacion vertical
		4.0f, 0.45f);					// Velocidad de movimiento | Velocidad de rotacion

	while (!mainWindow.getShouldClose()) // Loop Principal 
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario 
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpiamos la window
		mainWindow.Clear();

		// Dibujamos el Skybox 
		resources.skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Configuracion para la iluminación

		// código...

		// Info en el Shader 
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();				// modelo
		uniformProjection = shaderList[0].GetProjectionLocation();			// proyección
		uniformView = shaderList[0].GetViewLocation();				// vista
		uniformEyePosition = shaderList[0].GetEyePositionLocation();			// -
		uniformColor = shaderList[0].getColorLocation();				// color
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();	// intensidad especular
		uniformShininess = shaderList[0].GetShininessLocation();			// brillo

		shaderList[0].SetDirectionalLight(&mainLight);								// iluminación direccional
		shaderList[0].SetPointLights(PL_0, pointLightCount);						// iluminación pointlight
		shaderList[0].SetSpotLights(SP_lamparas, spotLightCount);					// iluminación spotlight

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// ------------------------------------------------------------------ Primitivas

		/*
				Mesh	indice
			Suelo		 [0]
			Vegetacion   [1]

		*/

		// Suelo
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		scal = glm::vec3(100.0f, 1.0f, 100.0f);

		RenderMeshWithTexture(meshList[0], color, pos, scal, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess,
			&Material_opaco, &resources.croquisTexture);

		// ------------------------------------------------------------------ Zoológico
		std::vector<glm::vec3> posAnimals = {
			{50.0f, 0.0f, 80.0f},		// Zorro
			{40.0f, 0.0f, 80.0f},		// Tigre
			{30.0f, 0.0f, 80.0f},		// Rinoceronte
			{20.0f, 0.0f, 80.0f},		// Pantera
			{10.0f, 0.0f, 80.0f},		// Panda Rojo
			{0.0f, 0.0f, 80.0f},		// Orangutan
			{-10.0f, 0.0f, 80.0f}		// Pedro
		};
		for (size_t i = 0; i < resources.Zoologico.size(); i++)
			RenderModel(modelZoo, uniformModel, origen, posAnimals[i], resources.Zoologico[i].modelo);

		// ------------------------------------------------------------------ Arena Central
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.ArenaCentral);

		// ------------------------------------------------------------------ Universo de Chicken Little

		// Piramide de Chicken Little
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.PiramideChicken);

		// Chicken Little
		pos = glm::vec3(-10.0f, 35.0f, 25.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Chicken_Little);

		// Personajes secundarios
		// Edificios
		// Vehiculos
		// Árboles

		// ------------------------------------------------------------------ Universo de Chilly Willy

		// Piramide de Chilli Willy
		pos = glm::vec3(-550.0f, 105.0f, -250.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.PiramideChilly);

		// Chilly Willy
		pos = glm::vec3(10.0f, 35.0f, 25.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Chilly_Willy);

		// Lago de hielo
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.Lago);

		// Árbol de hielo
		pos = glm::vec3(5.0f, 250.0f, -50.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.Arbol_Hielo);
		
		// Árbol de hielo 2
		pos = glm::vec3(120.0f, 250.0f, 50.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.Arbol_Hielo);

		//Iglu
		pos = glm::vec3(-80.0f, 35.0f, 50.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.Iglu);

		//Anuncio
		//pos = glm::vec3(-200.0f, 35.0f, -70.0f);
		//RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.Anuncio);
		// ------------------------------------------------------------------ Universo de Rikoche 
		
		// Piramide de Rikoche
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelRikoche, uniformModel, origenRikoche, pos, resources.PiramideRikoche);

		// Rikoche
		pos = glm::vec3(0.0f, 35.0f, 35.0f);
		RenderModel(modelRikoche, uniformModel, origen, pos, resources.Rikoche);

		// ------------------------------------------------------------------ Universo de Totoro 

		// Piramide de Totoro
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origenTotoro, pos, resources.PiramideTotoro);

		// Totoro (Ō-Totoro)
		pos = glm::vec3(0.0f, 30.0f, 20.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro);

		// Totoro mediano (Chū-Totoro)
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_mediano);

		// Totoro chiquito (Chibi-Totoro)
		pos = glm::vec3(-7.0f, 35.0f, 20.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_chiquito);

		// ------------------------------------------------------------------ Modelos con Blending (transparencia o traslucidez)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// Instanciar modelos aqui...

		glDisable(GL_BLEND);

		// ------------------------------------------------------------------ End of program 
		glUseProgram(0);
		mainWindow.swapBuffers();
	}	
	return 0;
}