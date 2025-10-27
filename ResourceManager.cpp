#include "ResourceManager.h"

ResourceManager::ResourceManager() {

    // Skybox
    std::vector<std::string> blueSky;
    blueSky.push_back("Texturas/Skybox/blueSky/derecha.png");		// right (voltear verticalmente)
    blueSky.push_back("Texturas/Skybox/blueSky/izquierda.png");		// left
    blueSky.push_back("Texturas/Skybox/blueSky/abajo.png");			// bottom
    blueSky.push_back("Texturas/Skybox/blueSky/arriba.png");		// up (girar 90° en sentido antihorario)
    blueSky.push_back("Texturas/Skybox/blueSky/atras.png");			// back
    blueSky.push_back("Texturas/Skybox/blueSky/enfrente.png");		// front
    skybox = Skybox(blueSky);

    // Texturas
    plainTexture = Texture("Texturas/plain.png");   plainTexture.LoadTextureA();
    pastoTexture = Texture("Texturas/Pasto.png");   pastoTexture.LoadTextureA();
    croquisTexture = Texture("Texturas/Croquis.jpg");  croquisTexture.LoadTextureA();

    // Modelos de las piramides
    ArenaCentral.LoadModel("Modelos/Prehispanico/Arena Central.obj");
    PiramideChicken.LoadModel("Modelos/Piramides/Piramide de la Venta.obj");
    PiramideChilly.LoadModel("Modelos/Piramides/Piramide de Quetzalcoatl.obj");
    PiramideRikoche.LoadModel("Modelos/Piramides/Templo de Lucha.obj");
    PiramideTotoro.LoadModel("Modelos/Piramides/Templo de Kukulcan.obj");

    // Modelos de los animales del Zoológico
    Zoologico = 
    {
        {"Zorro",        "Modelos/Zoologico/Zorro.obj"},
        {"Tigre",        "Modelos/Zoologico/Tigre.obj"},
        {"Rinoceronte",  "Modelos/Zoologico/Rinoceronte.obj"},
        {"Pantera",      "Modelos/Zoologico/Pantera.obj"},
        {"Panda Rojo",   "Modelos/Zoologico/Panda Rojo.obj"},
        {"Orangutan",    "Modelos/Zoologico/Orangutan.obj"},
        {"Mapache",      "Modelos/Zoologico/Mapache.obj"}
    };
    for (auto& animal : Zoologico)
        animal.modelo.LoadModel(animal.ruta);

    // Personajes principales
    Chicken_Little.LoadModel("Modelos/Chicken Little/Chicken Little.obj");
    Chilly_Willy.LoadModel("Modelos/Chilly Willy/Chilly Willy.obj");
    Rikoche.LoadModel("Modelos/Rikoche/Rikoche.obj");
    Totoro.LoadModel("Modelos/Totoro/Totoro.obj");
    Totoro_mediano.LoadModel("Modelos/Totoro/Totoro_mediano.obj");
    Totoro_chiquito.LoadModel("Modelos/Totoro/Totoro_chiquito.obj");

    // Modelos de cada universo
    
    //...
}