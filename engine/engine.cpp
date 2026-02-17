#include "engine.h"
#include <iostream>   
#include <GL/freeglut.h>
#include "FreeImage.h"
#include <glm/gtc/type_ptr.hpp>
#include "orthographicCamera.h"
#include "perspectiveCamera.h"


struct TextRequest {
    std::string text;
    float x, y;
    float r, g, b;
};

struct Eng::Base::Reserved
{
    bool initFlag = false;

    // --- DATI SCENA ---
    Camera* currentCamera = nullptr;
    List* currentList = nullptr;
    List* reflectionList = nullptr;

    // --- CAMERA UI (Ortografica) ---
    std::unique_ptr<OrthographicCamera> uiCamera;

    // -- WINDOW STATE -- 
    int windowWidth = 800;
    int windowHeight = 600;

    // -- FPS -- 
    bool show_fps = false;
    float fps = 0.0f;
    int frameCounter = 0;
    std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();

    // -- TESTO --
    std::vector<std::string> consoleText;
    std::vector<TextRequest> guiText;

    // Callback
    Eng::DisplayCallback   clientDisplayCb = nullptr;
    Eng::ReshapeCallback  clientReshapeCb = nullptr;
    Eng::KeyboardCallback clientKeyboardCb = nullptr;
    Eng::SpecialCallback  clientSpecialCb = nullptr;

    Reserved() {
        uiCamera = std::make_unique<OrthographicCamera>("UI_Cam", 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    }
};

// --- STATIC WRAPPERS ---
static void glutDisplayWrapper() { Eng::Base::getInstance().handleDisplayRequest(); }
static void glutReshapeWrapper(int width, int height) { Eng::Base::getInstance().handleReshapeRequest(width, height); }
static void glutKeyboardWrapper(unsigned char key, int x, int y) { Eng::Base::getInstance().handleKeyboardRequest(key, x, y); }
static void glutSpecialWrapper(int key, int x, int y) { Eng::Base::getInstance().handleSpecialRequest(key, x, y); }

// --- IMPLEMENTAZIONE BASE ---

Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>()) {}
Eng::Base::~Base() { free(); }

Eng::Base& Eng::Base::getInstance() {
    static Base instance;
    return instance;
}

bool Eng::Base::init(int argc, char* argv[]) {
    if (reserved->initFlag) return false;
    glutInit(&argc, argv);
    FreeImage_Initialise();
    std::cout << "[Engine] Initialized" << std::endl;
    reserved->initFlag = true;
    return true;
}

bool Eng::Base::free() {
    if (!reserved->initFlag) return false;
    reserved->initFlag = false;
    std::cout << "[Engine] Freed" << std::endl;
    return true;
}

void Eng::Base::createWindow(int width, int height, int x, int y, const char* title) {
    if (!reserved->initFlag) return;

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x, y);
    glutCreateWindow(title);

    // Set frustum iniziale per la UI
    reserved->uiCamera->setFrustum(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    glutDisplayFunc(glutDisplayWrapper);
    glutReshapeFunc(glutReshapeWrapper);
    glutKeyboardFunc(glutKeyboardWrapper);
    glutSpecialFunc(glutSpecialWrapper);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Eng::Base::update() { glutMainLoop(); }

// Setters
void Eng::Base::setDisplayCallback(DisplayCallback cb) { reserved->clientDisplayCb = cb; }
void Eng::Base::setReshapeCallback(ReshapeCallback cb) { reserved->clientReshapeCb = cb; }
void Eng::Base::setKeyboardCallback(KeyboardCallback cb) { reserved->clientKeyboardCb = cb; }
void Eng::Base::setSpecialCallback(SpecialCallback cb) { reserved->clientSpecialCb = cb; }

void Eng::Base::setClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }

void Eng::Base::setLighting(bool enable) { 
    if (enable) 
        glEnable(GL_LIGHTING); 
    else 
        glDisable(GL_LIGHTING); }

void Eng::Base::setTexture(bool enable) { 
    if (enable) 
        glEnable(GL_TEXTURE_2D); 
    else 
        glDisable(GL_TEXTURE_2D); 
}

void Eng::Base::setRenderList(List* list) { reserved->currentList = list; }
void Eng::Base::setReflectionList(List* list) { reserved->reflectionList = list; }
void Eng::Base::setMainCamera(Camera* camera) { reserved->currentCamera = camera; }

void Eng::Base::render() {
    if (!reserved->currentCamera || !reserved->currentList) return;

    // === SCENA 3D ===
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Proiezione Camera Client (Perspective)
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(reserved->currentCamera->getProjectionMatrix()));

    glm::mat4 viewMatrix = reserved->currentCamera->getInvCameraMatrix();
    // prima questa se no sarebbe sopra il tavolo riflesso
    if (reserved->reflectionList) {
        glFrontFace(GL_CW);
        reserved->reflectionList->render(viewMatrix);
        glFrontFace(GL_CCW);
    }
    reserved->currentList->render(viewMatrix);

    // === OVERLAY 2D (UI) ===
    calculateFPS();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    // Proiezione Ortogonale (Usa la Camera UI Interna)
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(reserved->uiCamera->getProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Visualizzazione FPS 
    if (reserved->show_fps) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FPS: %.2f", reserved->fps);
        glRasterPos2f(reserved->windowWidth - 100.0f, reserved->windowHeight - 12.0f);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
    }

    // Visualizzazione Menu
    float textYPosition = reserved->windowHeight - 12.0f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (const auto& toPrint : reserved->consoleText) {
        glRasterPos2f(0.0f, textYPosition);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)toPrint.c_str());
        textYPosition -= 12;
    }

    // Messaggio di vittoria
    void* guiFont = GLUT_BITMAP_TIMES_ROMAN_24;
    for (const auto& item : reserved->guiText) {
        glColor3f(item.r, item.g, item.b);
        glRasterPos2f(item.x, item.y);
        glutBitmapString(guiFont, (const unsigned char*)item.text.c_str());
    }

    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

void Eng::Base::handleDisplayRequest() {
    if (reserved->clientDisplayCb) reserved->clientDisplayCb();
    this->render();
}

void Eng::Base::handleReshapeRequest(int width, int height) {
    if (height == 0) height = 1;
    reserved->windowWidth = width;
    reserved->windowHeight = height;
    glViewport(0, 0, width, height);

    // Aggiorna la Camera UI 
    reserved->uiCamera->setFrustum(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    if (reserved->clientReshapeCb) reserved->clientReshapeCb(width, height);
}

void Eng::Base::handleKeyboardRequest(unsigned char key, int x, int y) {
    if (reserved->clientKeyboardCb) reserved->clientKeyboardCb(key, x, y);
}

void Eng::Base::handleSpecialRequest(int key, int x, int y) {
    if (reserved->clientSpecialCb) reserved->clientSpecialCb(key, x, y);
}

void ENG_API Eng::Base::calculateFPS() {
    reserved->frameCounter++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - reserved->lastTime;
    if (elapsedTime.count() >= 1.0f) {
        reserved->fps = reserved->frameCounter / elapsedTime.count();
        reserved->frameCounter = 0;
        reserved->lastTime = currentTime;
    }
}

void ENG_API Eng::Base::enableFPS() { reserved->show_fps = true; }
void ENG_API Eng::Base::disableFPS() { reserved->show_fps = false; }
void Eng::Base::postRedisplay() { glutPostRedisplay(); }

void Eng::Base::addToScreenText(std::string text) { reserved->consoleText.push_back(text); }
void Eng::Base::addString(float x, float y, std::string text, float r, float g, float b) {
    TextRequest req = { text, x, y, r, g, b };
    reserved->guiText.push_back(req);
}
void Eng::Base::clearScreenText() { reserved->consoleText.clear(); reserved->guiText.clear(); }
int Eng::Base::getWindowWidth() { return reserved->windowWidth; }
int Eng::Base::getWindowHeight() { return reserved->windowHeight; }
int Eng::Base::getTextWidth(const std::string& text) {
    return glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)text.c_str());
}