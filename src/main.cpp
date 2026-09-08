//41230301816
#include <GL/glut.h>
#include <cmath>
#include <iostream>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera Variables
float camX = 0.0f, camY = 4.0f, camZ = 13.0f;
float camYaw = -90.0f;
float camPitch = -5.0f;
float lookX = 0.0f, lookY = 4.0f, lookZ = 0.0f;

// Mouse Interaction
int lastMouseX = -1, lastMouseY = -1;
bool isDragging = false;

// Lighting & Device States
bool light0_on = true; // Living Room Light
bool light1_on = true; // Dining Room Light
bool tv_on = true;     // TV Display State

// Animations
float fanAngle = 0.0f;
bool fanOn = true;

float doorAngle = 0.0f;
bool doorOpening = false;

// Print Controls to Console
void printInstructions() {
    cout << "========================================================\n";
    cout << "             OpenGL Final Project Controls              \n";
    cout << "========================================================\n";
    cout << " [W / S]       : Move Camera Forward / Backward\n";
    cout << " [A / D]       : Move Camera Left / Right (Strafe)\n";
    cout << " [UP / DOWN]   : Adjust Camera Eye Height\n";
    cout << " [LEFT / RIGHT]: Rotate Camera View Angle\n";
    cout << " [Mouse Drag]  : Look Around (FPS Style Camera)\n";
    cout << " [Mouse Scroll]: Zoom In / Zoom Out\n";
    cout << " ------------------------------------------------------ \n";
    cout << " [Key 1]       : Toggle Living Area Light (Light 0)\n";
    cout << " [Key 2]       : Toggle Dining Area Light (Light 1)\n";
    cout << " [Key T]       : Toggle Television Screen (ON/OFF)\n";
    cout << " [Key F]       : Toggle Ceiling Fan On/Off\n";
    cout << " [Key O]       : Toggle Door Inward Opening/Closing\n";
    cout << " [ESC]         : Exit Program\n";
    cout << "========================================================\n\n";
}

// Material Helper Function
void setMaterial(float r, float g, float b, float shininess = 40.0f) {
    GLfloat mat_ambient[]  = { r * 0.35f, g * 0.35f, b * 0.35f, 1.0f };
    GLfloat mat_diffuse[]  = { r, g, b, 1.0f };
    GLfloat mat_specular[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat no_emission[]  = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
}

// Glowing Material for Displays & Light Bulbs
void setEmissive(float r, float g, float b) {
    GLfloat mat_ambient[]  = { r, g, b, 1.0f };
    GLfloat mat_diffuse[]  = { r, g, b, 1.0f };
    GLfloat mat_emission[] = { r * 0.9f, g * 0.9f, b * 0.9f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);
}

// 1. Tessellated Floor for Localized Lighting
void drawTessellatedFloor() {
    setMaterial(0.62f, 0.42f, 0.26f);
    glNormal3f(0.0f, 1.0f, 0.0f);

    float stepX = 26.0f / 26.0f;
    float stepZ = 22.0f / 22.0f;

    for (float x = -13.0f; x < 13.0f; x += stepX) {
        glBegin(GL_TRIANGLE_STRIP);
        for (float z = -11.0f; z <= 11.0f; z += stepZ) {
            glVertex3f(x, 0.0f, z);
            glVertex3f(x + stepX, 0.0f, z);
        }
        glEnd();
    }
}

// Room Structure (Floor and Walls)
void drawRoom() {
    drawTessellatedFloor();

    // Back Wall
    setMaterial(0.92f, 0.90f, 0.86f);
    glPushMatrix();
    glTranslatef(0.0f, 5.0f, -11.0f);
    glScalef(26.0f, 10.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right Wall
    glPushMatrix();
    glTranslatef(13.0f, 5.0f, 0.0f);
    glScalef(0.2f, 10.0f, 22.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Wall - Long section behind sofa
    setMaterial(0.86f, 0.88f, 0.92f);
    glPushMatrix();
    glTranslatef(-13.0f, 5.0f, -3.0f);
    glScalef(0.2f, 10.0f, 16.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Wall - Front short corner
    glPushMatrix();
    glTranslatef(-13.0f, 5.0f, 10.0f);
    glScalef(0.2f, 10.0f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Wall - Lintel over door
    glPushMatrix();
    glTranslatef(-13.0f, 8.5f, 7.0f);
    glScalef(0.2f, 3.0f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();
}

// 2. Interactive Animated Door (Swings Inward into the Room)
void drawDoor() {
    glPushMatrix();
    // Hinge anchor at Left Wall: (-12.9, 0.0, 5.0)
    glTranslatef(-12.9f, 0.0f, 5.0f);
    glRotatef(doorAngle, 0.0f, 1.0f, 0.0f); // Positive angle swings inward into room (+X direction)

    // Door Panel
    setMaterial(0.40f, 0.20f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 3.5f, 2.0f);
    glScalef(0.12f, 7.0f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Golden Metallic Door Handle
    setMaterial(0.95f, 0.8f, 0.2f, 100.0f);
    glPushMatrix();
    glTranslatef(0.12f, 3.5f, 3.5f);
    glutSolidSphere(0.18, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

// 3. Scenic Glass Window
void drawWindow() {
    setMaterial(0.25f, 0.12f, 0.05f);
    glPushMatrix();
    glTranslatef(5.5f, 5.5f, -10.88f);
    glScalef(6.4f, 4.4f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    setEmissive(0.35f, 0.70f, 0.95f);
    glPushMatrix();
    glTranslatef(5.5f, 5.5f, -10.82f);
    glScalef(6.0f, 4.0f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMaterial(0.25f, 0.12f, 0.05f);
    glPushMatrix();
    glTranslatef(5.5f, 5.5f, -10.80f);
    glScalef(0.15f, 4.0f, 0.04f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.5f, 5.5f, -10.80f);
    glScalef(6.0f, 0.15f, 0.04f);
    glutSolidCube(1.0);
    glPopMatrix();
}

// 4. Analog Wall Clock
void drawWallClock() {
    glPushMatrix();
    glTranslatef(-6.5f, 7.8f, -10.85f);

    setMaterial(0.85f, 0.65f, 0.15f);
    glPushMatrix();
    glScalef(1.8f, 1.8f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMaterial(0.98f, 0.98f, 0.98f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.06f);
    glScalef(1.5f, 1.5f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Hour Hand
    setMaterial(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.08f);
    glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.22f, 0.0f);
    glScalef(0.08f, 0.44f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Minute Hand
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.08f);
    glRotatef(-50.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.32f, 0.0f);
    glScalef(0.06f, 0.64f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMaterial(0.85f, 0.65f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    glutSolidSphere(0.07, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

// 5. Lamp Fixture
void drawWallLamp(float x, float y, float z, bool isOn, float rotY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    setMaterial(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glScalef(0.15f, 0.8f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, 0.0f, 0.0f);
    glScalef(0.7f, 0.1f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMaterial(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.75f, 0.2f, 0.0f);
    glScalef(0.5f, 0.3f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    if (isOn) {
        setEmissive(1.0f, 0.9f, 0.4f);
    } else {
        setMaterial(0.25f, 0.25f, 0.25f);
    }
    glPushMatrix();
    glTranslatef(0.75f, 0.0f, 0.0f);
    glutSolidSphere(0.32, 16, 16);
    glPopMatrix();

    glPopMatrix();
}

// 6. Living Area: TV Cabinet + Interactive TV Screen
void drawTVWithCabinet(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Cabinet Table
    setMaterial(0.30f, 0.16f, 0.06f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glScalef(5.2f, 1.2f, 1.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMaterial(0.22f, 0.11f, 0.04f);
    glPushMatrix();
    glTranslatef(-1.3f, 0.7f, 0.91f);
    glScalef(2.2f, 0.9f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.3f, 0.7f, 0.91f);
    glScalef(2.2f, 0.9f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    // TV Base Stand
    setMaterial(0.12f, 0.12f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, 0.0f);
    glScalef(1.8f, 0.08f, 0.9f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.6f, 0.0f);
    glScalef(0.3f, 0.5f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // TV Bezel
    setMaterial(0.06f, 0.06f, 0.06f);
    glPushMatrix();
    glTranslatef(0.0f, 3.2f, 0.0f);
    glScalef(4.8f, 2.7f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // TV Screen (Changes based on tv_on state)
    if (tv_on) {
        // Sky portion (Emissive Active Screen)
        setEmissive(0.15f, 0.55f, 0.95f);
        glPushMatrix();
        glTranslatef(0.0f, 3.55f, 0.085f);
        glScalef(4.5f, 1.8f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Grass/Landscape portion
        setEmissive(0.2f, 0.75f, 0.25f);
        glPushMatrix();
        glTranslatef(0.0f, 2.45f, 0.085f);
        glScalef(4.5f, 0.8f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
    } else {
        // Dark Black Off Screen
        setMaterial(0.05f, 0.05f, 0.05f);
        glPushMatrix();
        glTranslatef(0.0f, 3.2f, 0.085f);
        glScalef(4.5f, 2.5f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    glPopMatrix();
}


void drawBox(float x, float y, float z, float sx, float sy, float sz,
             float r, float g, float b) {
    setMaterial(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawCylinder(float x, float y, float z, float radius, float height,
                  float r, float g, float b) {
    setMaterial(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, radius, radius, height, 24, 4);
    gluDisk(q, 0.0, radius, 24, 1);
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(q, 0.0, radius, 24, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Modern parquet-style floor with alternating boards and a dark border.
void drawBeautifulFloor() {
    drawBox(0.0f, -0.12f, 0.0f, 26.0f, 0.24f, 22.0f,
            0.28f, 0.14f, 0.055f);

    for (int row = 0; row < 11; ++row) {
        float z = -10.0f + row * 2.0f;
        for (int col = 0; col < 13; ++col) {
            float x = -12.0f + col * 2.0f + ((row % 2) ? 1.0f : 0.0f);
            drawBox(x, 0.015f, z, 1.85f, 0.06f, 1.82f,
                    (col + row) % 2 ? 0.58f : 0.68f,
                    (col + row) % 2 ? 0.31f : 0.39f,
                    (col + row) % 2 ? 0.13f : 0.18f);
        }
    }

    // Floor border/skirting
    drawBox(0.0f, 0.12f, -10.88f, 25.6f, 0.24f, 0.18f,
            0.20f, 0.09f, 0.035f);
    drawBox(0.0f, 0.12f, 10.88f, 25.6f, 0.24f, 0.18f,
            0.20f, 0.09f, 0.035f);
    drawBox(-12.88f, 0.12f, 0.0f, 0.18f, 0.24f, 21.6f,
            0.20f, 0.09f, 0.035f);
    drawBox(12.88f, 0.12f, 0.0f, 0.18f, 0.24f, 21.6f,
            0.20f, 0.09f, 0.035f);
}

// False ceiling: central panel + four border beams + decorative lights.
void drawBeautifulCeiling() {
    drawBox(0.0f, 9.88f, 0.0f, 25.5f, 0.24f, 21.5f,
            0.93f, 0.93f, 0.90f);

    drawBox(0.0f, 9.70f, -10.1f, 25.0f, 0.22f, 0.45f,
            0.72f, 0.72f, 0.68f);
    drawBox(0.0f, 9.70f, 10.1f, 25.0f, 0.22f, 0.45f,
            0.72f, 0.72f, 0.68f);
    drawBox(-12.1f, 9.70f, 0.0f, 0.45f, 0.22f, 20.0f,
            0.72f, 0.72f, 0.68f);
    drawBox(12.1f, 9.70f, 0.0f, 0.45f, 0.22f, 20.0f,
            0.72f, 0.72f, 0.68f);

    // Recessed ceiling lights
    float lights[][2] = {
        {-8.0f,-7.0f}, {-2.5f,-7.0f}, {3.0f,-7.0f}, {8.0f,-7.0f},
        {-8.0f, 2.0f}, {-2.5f, 2.0f}, {3.0f, 2.0f}, {8.0f, 2.0f},
        {-8.0f, 8.0f}, {8.0f, 8.0f}
    };

    for (int i = 0; i < 10; ++i) {
        setEmissive(1.0f, 0.82f, 0.42f);
        glPushMatrix();
        glTranslatef(lights[i][0], 9.52f, lights[i][1]);
        glScalef(0.30f, 0.06f, 0.30f);
        glutSolidSphere(1.0, 16, 16);
        glPopMatrix();
    }
}

// Rug under the living-room tea table.
void drawLivingRug() {
    drawBox(-6.5f, 0.075f, -4.2f, 6.6f, 0.08f, 3.8f,
            0.15f, 0.22f, 0.30f);
    drawBox(-6.5f, 0.125f, -4.2f, 6.0f, 0.035f, 3.2f,
            0.42f, 0.55f, 0.62f);
}

// Cushions make the sofa much less box-like.
void drawSofaCushions() {
    setMaterial(0.30f, 0.48f, 0.68f, 30.0f);

    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(-6.5f + i * 1.35f, 1.55f, 1.02f);
        glScalef(1.15f, 0.72f, 0.35f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    setMaterial(0.75f, 0.80f, 0.84f, 20.0f);
    glPushMatrix();
    glTranslatef(-8.55f, 1.25f, 0.35f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.75f, 0.75f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.45f, 1.25f, 0.35f);
    glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.75f, 0.75f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();
}

// Curtains around the existing window.
void drawCurtains() {
    setMaterial(0.48f, 0.12f, 0.18f, 20.0f);

    drawBox(1.9f, 5.6f, -10.72f, 0.9f, 4.8f, 0.12f,
            0.48f, 0.12f, 0.18f);
    drawBox(9.1f, 5.6f, -10.72f, 0.9f, 4.8f, 0.12f,
            0.48f, 0.12f, 0.18f);

    // Curtain rod
    drawCylinder(1.4f, 8.05f, -10.65f, 0.07f, 8.2f,
                 0.80f, 0.62f, 0.20f);
}

// Indoor plant near the living-room corner.
void drawPlant(float x, float z) {
    drawCylinder(x, 0.15f, z, 0.65f, 0.75f,
                 0.38f, 0.17f, 0.06f);

    setMaterial(0.12f, 0.48f, 0.18f, 15.0f);
    for (int i = 0; i < 7; ++i) {
        glPushMatrix();
        glTranslatef(x + cos(i * 0.9f) * 0.38f,
                     1.10f + (i % 3) * 0.35f,
                     z + sin(i * 0.9f) * 0.38f);
        glRotatef((float)(i * 25), 0.0f, 1.0f, 0.0f);
        glScalef(0.22f, 0.80f, 0.08f);
        glutSolidSphere(1.0, 12, 12);
        glPopMatrix();
    }
}


void drawLivingArea() {
    drawTVWithCabinet(-6.5f, -8.0f);

    // Tea Table
    setMaterial(0.45f, 0.24f, 0.10f);
    glPushMatrix();
    glTranslatef(-6.5f, 0.45f, -4.2f);
    glScalef(4.0f, 0.9f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Sofa
    setMaterial(0.18f, 0.30f, 0.50f);
    glPushMatrix();
    glTranslatef(-6.5f, 0.6f, 0.0f);

    glPushMatrix();
    glScalef(4.8f, 0.8f, 2.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 1.0f);
    glScalef(4.8f, 1.6f, 0.45f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5f, 0.55f, 0.0f);
    glScalef(0.45f, 1.1f, 2.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 0.55f, 0.0f);
    glScalef(0.45f, 1.1f, 2.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

// 7. Dining Area - Modern Dining Set
void drawDiningChair(float x, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Chair legs
    setMaterial(0.18f, 0.09f, 0.035f, 35.0f);
    const float lx[4] = {-0.43f, 0.43f, -0.43f, 0.43f};
    const float lz[4] = {-0.43f, -0.43f, 0.43f, 0.43f};

    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glTranslatef(lx[i], 0.55f, lz[i]);
        glRotatef((i < 2 ? -6.0f : 6.0f), 1.0f, 0.0f, 0.0f);
        glScalef(0.11f, 1.15f, 0.11f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // Seat frame
    setMaterial(0.32f, 0.15f, 0.05f, 40.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.16f, 0.0f);
    glScalef(1.12f, 0.16f, 1.08f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Soft seat cushion
    setMaterial(0.82f, 0.77f, 0.68f, 25.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.27f, 0.0f);
    glScalef(0.90f, 0.18f, 0.86f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Back support frame
    setMaterial(0.32f, 0.15f, 0.05f, 40.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.05f, 0.42f);
    glScalef(1.12f, 1.65f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Padded backrest
    setMaterial(0.82f, 0.77f, 0.68f, 25.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.02f, 0.34f);
    glScalef(0.86f, 1.28f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Decorative top rail
    setMaterial(0.42f, 0.21f, 0.07f, 45.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.82f, 0.43f);
    glScalef(1.18f, 0.12f, 0.16f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawDiningArea() {
    // Dining table is intentionally drawn here only once.
    glPushMatrix();
    glTranslatef(6.0f, 0.0f, -2.5f);

    // Main tabletop
    setMaterial(0.30f, 0.13f, 0.035f, 60.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.80f, 0.0f);
    glScalef(4.9f, 0.28f, 3.25f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Lighter top surface
    setMaterial(0.60f, 0.30f, 0.085f, 75.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.97f, 0.0f);
    glScalef(4.55f, 0.10f, 2.90f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Table apron
    setMaterial(0.24f, 0.095f, 0.025f, 45.0f);
    drawBox(0.0f, 1.55f, -1.42f, 4.25f, 0.40f, 0.12f,
            0.24f, 0.095f, 0.025f);
    drawBox(0.0f, 1.55f,  1.42f, 4.25f, 0.40f, 0.12f,
            0.24f, 0.095f, 0.025f);

    // Four sturdy legs with brass foot caps.
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            drawCylinder(i * 2.0f, 0.12f, j * 1.20f,
                         0.17f, 1.45f, 0.20f, 0.07f, 0.018f);

            setMaterial(0.68f, 0.47f, 0.12f, 90.0f);
            glPushMatrix();
            glTranslatef(i * 2.0f, 0.10f, j * 1.20f);
            glutSolidSphere(0.18, 16, 12);
            glPopMatrix();
        }
    }

    // Table runner
    drawBox(0.0f, 2.035f, 0.0f, 2.75f, 0.035f, 0.48f,
            0.74f, 0.55f, 0.29f);

    // Vase
    setMaterial(0.90f, 0.90f, 0.86f, 80.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.20f, 0.0f);
    glScalef(0.32f, 0.65f, 0.32f);
    glutSolidSphere(1.0, 20, 16);
    glPopMatrix();

    // Flowers
    setMaterial(0.16f, 0.45f, 0.16f, 10.0f);
    for (int i = 0; i < 6; ++i) {
        glPushMatrix();
        glTranslatef((i - 2.5f) * 0.08f, 2.75f + (i % 2) * 0.10f,
                     ((i % 3) - 1) * 0.07f);
        glScalef(0.04f, 0.45f, 0.04f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    setMaterial(0.85f, 0.28f, 0.35f, 25.0f);
    for (int i = 0; i < 5; ++i) {
        glPushMatrix();
        glTranslatef((i - 2) * 0.16f, 2.98f + (i % 2) * 0.08f,
                     ((i % 3) - 1) * 0.09f);
        glutSolidSphere(0.12, 12, 12);
        glPopMatrix();
    }

    glPopMatrix();

    // Four modern upholstered chairs.
    drawDiningChair(6.0f, -0.55f, 0.0f);
    drawDiningChair(6.0f, -4.45f, 180.0f);
    drawDiningChair(3.15f, -2.5f, -90.0f);
    drawDiningChair(8.85f, -2.5f, 90.0f);
}

// 8. Ceiling Fan
void drawCeilingFan() {
    glPushMatrix();
    glTranslatef(0.0f, 8.8f, -1.0f);

    setMaterial(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.12f, 1.0f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    glRotatef(fanAngle, 0.0f, 1.0f, 0.0f);
    setMaterial(0.95f, 0.95f, 0.95f);
    glutSolidSphere(0.45, 18, 18);

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.6f, 0.0f, 0.0f);
        glScalef(2.6f, 0.04f, 0.45f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glPopMatrix();
}

// 9. Sharp Localized Lighting
void setupLighting() {
    glEnable(GL_LIGHTING);

    GLfloat global_ambient[] = { 0.20f, 0.20f, 0.20f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Light 0 - Living Area Lamp
    if (light0_on) {
        glEnable(GL_LIGHT0);
        GLfloat light0_pos[]  = { -12.0f, 7.0f, -3.0f, 1.0f };
        GLfloat light0_diff[] = { 1.4f, 1.3f, 1.1f, 1.0f };
        GLfloat light0_spec[] = { 0.6f, 0.6f, 0.6f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spec);

        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.08f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
    } else {
        glDisable(GL_LIGHT0);
    }

    // Light 1 - Dining Area Lamp
    if (light1_on) {
        glEnable(GL_LIGHT1);
        GLfloat light1_pos[]  = { 12.0f, 7.0f, -2.5f, 1.0f };
        GLfloat light1_diff[] = { 1.4f, 1.2f, 0.8f, 1.0f };
        GLfloat light1_spec[] = { 0.6f, 0.5f, 0.3f, 1.0f };

        glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_spec);

        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.08f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
    } else {
        glDisable(GL_LIGHT1);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera Look Vector
    float yawRad = camYaw * (float)M_PI / 180.0f;
    float pitchRad = camPitch * (float)M_PI / 180.0f;

    lookX = camX + cos(pitchRad) * cos(yawRad);
    lookY = camY + sin(pitchRad);
    lookZ = camZ + cos(pitchRad) * sin(yawRad);

    gluLookAt(camX, camY, camZ, lookX, camY, lookZ, 0.0f, 1.0f, 0.0f);

    setupLighting();

    drawRoom();

    // Enhanced architectural details
    drawBeautifulFloor();
    drawBeautifulCeiling();
    drawCurtains();

    drawDoor();
    drawWindow();
    drawWallClock();

    drawWallLamp(-12.8f, 7.0f, -3.0f, light0_on, 0.0f);
    drawWallLamp(12.8f, 7.0f, -2.5f, light1_on, 180.0f);

    drawLivingArea();
    drawLivingRug();
    drawSofaCushions();

    drawDiningArea();

    drawPlant(-10.5f, -7.8f);
    drawPlant(10.5f, 7.5f);

    drawCeilingFan();

    glutSwapBuffers();
}

void idle() {
    if (fanOn) {
        fanAngle += 4.5f;
        if (fanAngle > 360.0f) fanAngle -= 360.0f;
    }

    if (doorOpening && doorAngle < 90.0f) {
        doorAngle += 2.0f;
    } else if (!doorOpening && doorAngle > 0.0f) {
        doorAngle -= 2.0f;
    }

    glutPostRedisplay();
}

// Mouse Controls
void mouseMotion(int x, int y) {
    if (isDragging) {
        if (lastMouseX >= 0 && lastMouseY >= 0) {
            float dx = (float)(x - lastMouseX);
            float dy = (float)(y - lastMouseY);

            camYaw += dx * 0.25f;
            camPitch -= dy * 0.25f;

            if (camPitch > 85.0f) camPitch = 85.0f;
            if (camPitch < -85.0f) camPitch = -85.0f;
        }
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            isDragging = false;
        }
    }
    // Mouse Wheel Zoom
    if (state == GLUT_DOWN) {
        float speed = 0.8f;
        float yawRad = camYaw * (float)M_PI / 180.0f;
        if (button == 3) {
            camX += cos(yawRad) * speed;
            camZ += sin(yawRad) * speed;
            glutPostRedisplay();
        } else if (button == 4) {
            camX -= cos(yawRad) * speed;
            camZ -= sin(yawRad) * speed;
            glutPostRedisplay();
        }
    }
}

// Keyboard with Live Status Reporting
void keyboard(unsigned char key, int x, int y) {
    float speed = 0.5f;
    float yawRad = camYaw * (float)M_PI / 180.0f;

    switch (key) {
        case 'w': case 'W':
            camX += cos(yawRad) * speed;
            camZ += sin(yawRad) * speed;
            break;
        case 's': case 'S':
            camX -= cos(yawRad) * speed;
            camZ -= sin(yawRad) * speed;
            break;
        case 'a': case 'A':
            camX += sin(yawRad) * speed;
            camZ -= cos(yawRad) * speed;
            break;
        case 'd': case 'D':
            camX -= sin(yawRad) * speed;
            camZ -= cos(yawRad) * speed;
            break;
        case '1':
            light0_on = !light0_on;
            cout << ">> [STATUS] Living Room Light (Light 0): " << (light0_on ? "ON" : "OFF") << endl;
            break;
        case '2':
            light1_on = !light1_on;
            cout << ">> [STATUS] Dining Room Light (Light 1): " << (light1_on ? "ON" : "OFF") << endl;
            break;
        case 't': case 'T':
            tv_on = !tv_on;
            cout << ">> [STATUS] Television Display: " << (tv_on ? "ON" : "OFF") << endl;
            break;
        case 'f': case 'F':
            fanOn = !fanOn;
            cout << ">> [STATUS] Ceiling Fan Animation: " << (fanOn ? "RUNNING" : "STOPPED") << endl;
            break;
        case 'o': case 'O':
            doorOpening = !doorOpening;
            cout << ">> [STATUS] Interactive Door: " << (doorOpening ? "OPENING INWARD" : "CLOSING") << endl;
            break;
        case 27:
            cout << ">> Exiting Project. Goodbye!\n";
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP)    camY += 0.3f;
    if (key == GLUT_KEY_DOWN)  camY -= 0.3f;
    if (key == GLUT_KEY_LEFT)  camYaw -= 3.5f;
    if (key == GLUT_KEY_RIGHT) camYaw += 3.5f;
    glutPostRedisplay();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
}

int main(int argc, char** argv) {
    printInstructions();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1080, 720);
    glutCreateWindow("Interactive Living & Dining Room Scene");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
