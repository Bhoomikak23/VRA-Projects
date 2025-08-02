#include <GL/glut.h>                     // OpenGL Utility Toolkit header for rendering and window functions
#include <cmath>                         // Math functions like sin(), cos()
#include <cstdlib>                       // For random number generation: rand()
#include <ctime>                         // For time() to seed rand()

#define M_PI 3.14159265358979323846      // Define value of π (pi) if not already defined

// Boat movement variables
float boatX = -300.0f;                   // Initial X position of the boat
float speed = 0.2f;                      // Speed at which the boat moves
bool boatMoving = false;                 // Flag to check if the boat should move

// Camera position variables
float camZ = 250.0f;                     // Z-position of the camera
float camX = 0.0f;                       // X-position of the camera

// Fish structure holding position, speed, and size
struct Fish {
    float x, y, z;                       // Position of the fish
    float speed;                         // Speed of the fish
    float scale;                         // Size of the fish
};

const int NUM_FISH = 6;                  // Total number of fish
Fish fishGroup[NUM_FISH];               // Array of fish

// Bird structure holding position and flying speed
struct Bird {
    float x, y, z;                       // Position of the bird
    float speedX, speedY;                // Speed in X and Y direction
};

const int NUM_BIRDS = 8;                 // Total number of birds
Bird birds[NUM_BIRDS];                   // Array of birds

// Initialize fish with random positions, speeds, and sizes
void initFishes() {
    for (int i = 0; i < NUM_FISH; i++) {
        fishGroup[i].x = -550.0f + rand() % 1100;                              // Random X between -550 and 550
        fishGroup[i].y = 0.5f + static_cast<float>(rand() % 20) / 10.0f;       // Y from 0.5 to 2.5
        fishGroup[i].z = -250.0f + rand() % 400;                               // Z from -250 to 150
        fishGroup[i].speed = 0.15f + static_cast<float>(rand() % 10) / 100.0f; // Speed from 0.15 to 0.25
        fishGroup[i].scale = 10.0f + static_cast<float>(rand() % 50) / 10.0f;  // Scale from 10.0 to 15.0
    }
}

// Initialize birds with spread positions and alternating vertical speeds
void initBirds() {
    for (int i = 0; i < NUM_BIRDS; i++) {
        birds[i].x = -400 + i * 70;                      // Spread out horizontally
        birds[i].y = 80 + rand() % 10;                   // Height between 80 and 89
        birds[i].z = -140 - rand() % 40;                 // Depth between -140 and -180
        birds[i].speedX = 0.5f + (rand() % 3) * 0.05f;    // SpeedX between 0.5 and 0.6
        birds[i].speedY = ((i % 2 == 0) ? 0.05f : -0.05f); // Alternate up/down flying
    }
}

// OpenGL initialization
void init() {
    srand(time(0));                                      // Seed random with current time
    glEnable(GL_DEPTH_TEST);                             // Enable depth testing for 3D
    glEnable(GL_BLEND);                                  // Enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   // Transparency blending mode
    glClearColor(1.0f, 0.85f, 0.4f, 1.0f);                // Background color: light yellow
    initFishes();                                        // Initialize fish
    initBirds();                                         // Initialize birds
}

// Draw blue water using a large quad
void drawSea() {
    glColor3f(0.3f, 0.7f, 0.9f);                         // Light blue
    glBegin(GL_QUADS);
    glVertex3f(-600, 0.01f, -300);
    glVertex3f(-600, 0.01f, 200);
    glVertex3f(600, 0.01f, 200);
    glVertex3f(600, 0.01f, -300);
    glEnd();
}

// Draw sand/beach area
void drawBeach() {
    glColor3f(0.84f, 0.72f, 0.54f);                      // Sandy yellow
    glBegin(GL_QUADS);
    glVertex3f(-600, 0, 200);
    glVertex3f(-600, 0, 250);
    glVertex3f(600, 0, 250);
    glVertex3f(600, 0, 200);
    glEnd();
}

// Draw the sun and glowing effect
void drawSun() {
    glPushMatrix();
    glTranslatef(300.0f, 200.0f, -280.0f);               // Move sun to corner
    glColor3f(1.0f, 0.45f, 0.0f);                        // Orange core
    glutSolidSphere(18, 30, 30);                         // Inner solid sun
    glColor4f(1.0f, 0.3f, 0.0f, 0.25f);                  // Transparent glow
    glutSolidSphere(26, 30, 30);                         // Glowing outer shell
    glPopMatrix();
}

// Draw a cloud from multiple white spheres
void drawCloud(float x, float y, float z, float scale) {
    glColor3f(1.0f, 1.0f, 1.0f);                         // White
    glPushMatrix();
    glTranslatef(x, y, z);                               // Move to position
    glScalef(scale, scale * 0.6f, 1.0f);                 // Resize cloud

    glutSolidSphere(4.5, 20, 20);                        // Center puff
    glTranslatef(3.5, 0.5, 0);                           // Right
    glutSolidSphere(3.5, 20, 20);
    glTranslatef(-7.0, -0.2, 0);                         // Left
    glutSolidSphere(3.8, 20, 20);
    glTranslatef(2.5, 0.6, 0);                           // Middle top-right
    glutSolidSphere(3.2, 20, 20);
    glTranslatef(-1.0, -0.5, 0);                         // Bottom middle
    glutSolidSphere(2.5, 20, 20);

    glPopMatrix();
}

// Draw a simple bird as a V shape
void drawBird(float x, float y, float z) {
    glColor3f(0.1f, 0.1f, 0.1f);                         // Dark grey
    glPushMatrix();
    glTranslatef(x, y, z);                               // Move to position
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0); glVertex3f(2, 1, 0);            // Right wing
    glVertex3f(0, 0, 0); glVertex3f(-2, 1, 0);           // Left wing
    glEnd();
    glPopMatrix();
}

// Draw the boat model with hull, deck, sides, sail, and seat
void drawBoat(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);                              // Position boat
    glScalef(3.0f, 3.0f, 3.0f);                         // Resize boat

    glColor3f(0.45f, 0.26f, 0.1f);                      // Brown wood
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = -20; i <= 20; ++i) {
        float theta = i * M_PI / 40.0f;
        float x = sin(theta) * 12;
        float z1 = cos(theta) * 3;
        float z2 = cos(theta) * -3;
        glVertex3f(x, -4.0f + 0.3f * sin(theta), z1);   // Right curve
        glVertex3f(x, -4.0f + 0.3f * sin(theta), z2);   // Left curve
    }
    glEnd();

    glColor3f(0.55f, 0.35f, 0.2f);                      // Deck
    glBegin(GL_QUADS);
    glVertex3f(-12, 0, -3); glVertex3f(-12, 0, 3);
    glVertex3f(12, 0, 3); glVertex3f(12, 0, -3);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.2f);                        // Side walls
    glBegin(GL_QUADS);                                  // Left wall
    glVertex3f(-12, 0, -3); glVertex3f(-12, 3, -3);
    glVertex3f(12, 3, -3); glVertex3f(12, 0, -3);
    glVertex3f(-12, 0, 3); glVertex3f(-12, 3, 3);        // Right wall
    glVertex3f(12, 3, 3); glVertex3f(12, 0, 3);
    glEnd();

    glBegin(GL_QUADS);                                  // Back wall
    glVertex3f(-12, 0, -3); glVertex3f(-12, 3, -3);
    glVertex3f(-12, 3, 3); glVertex3f(-12, 0, 3);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.2f);                        // Bow
    glBegin(GL_TRIANGLES);
    glVertex3f(12, 0, -3); glVertex3f(12, 0, 3);
    glVertex3f(14, 2, 0);
    glEnd();

    glColor3f(0.3f, 0.2f, 0.1f);                        // Mast
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.9f);                        // Sail
    glBegin(GL_TRIANGLES);
    glVertex3f(0, 10, 0); glVertex3f(0, 3, 0);
    glVertex3f(6, 6, 0);
    glEnd();

    glColor3f(0.6f, 0.4f, 0.25f);                       // Seat
    glBegin(GL_QUADS);
    glVertex3f(-4, 2.8f, -3); glVertex3f(-4, 2.8f, 3);
    glVertex3f(4, 2.8f, 3); glVertex3f(4, 2.8f, -3);
    glEnd();

    glPopMatrix();                                      // Restore
}

// ... Previous code and comments continue ...

void draw_fish(Fish f) {                            // Function to draw a single fish based on properties
    glPushMatrix();                                 // Save current matrix
    glTranslatef(f.x, f.y, f.z);                    // Move to fish position
    glColor3f(1.0, 0.3f + 0.05f * ((int)f.z % 5),    // Fish body color based on Z position
        0.1f + 0.05f * ((int)f.x % 4));
    glScalef(f.scale, f.scale * 0.5f, f.scale * 0.5f); // Scale fish
    glutSolidSphere(1.0, 14, 14);                   // Draw fish body as a sphere

    glColor3f(0.9, 0.3, 0.1);                        // Tail color
    glBegin(GL_TRIANGLES);                          // Draw tail as triangle
    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(-2.0f, 0.6f, 0.0f);
    glVertex3f(-2.0f, -0.6f, 0.0f);
    glEnd();
    glPopMatrix();                                  // Restore matrix
}

void display() {                                     // Display callback function
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glLoadIdentity();                                // Reset transformations
    gluLookAt(camX, 30, camZ,                        // Camera eye position
        camX, 0, -100,                         // Look-at point
        0, 1, 0);                              // Up vector

    drawSun();                                       // Draw sun

    drawCloud(-520, 200, -220, 7);                   // Draw multiple clouds with different positions and sizes
    drawCloud(80, 210, -250, 4.8);
    drawCloud(460, 95, -200, 10);
    drawCloud(-100, 100, -220, 4);

    for (int i = 0; i < NUM_BIRDS; ++i)              // Draw all birds
        drawBird(birds[i].x, birds[i].y, birds[i].z);

    drawSea();                                       // Draw sea
    drawBeach();                                     // Draw beach
    // drawPalmTree(400, 0, 230);                    // Optional: draw palm tree (currently commented)

    for (int i = 0; i < NUM_FISH; ++i)               // Draw all fish
        draw_fish(fishGroup[i]);

    drawBoat(boatX, 5.0f, -100);                     // Draw boat at its current position

    glutSwapBuffers();                               // Swap front and back buffers for smooth animation
}

void timer(int) {                                    // Timer callback function for animation
    if (boatMoving) {                                // If boat should move
        boatX += speed;                              // Move boat along X axis
        if (boatX > 600)                             // Reset boat if it goes off screen
            boatX = -600;
    }

    for (int i = 0; i < NUM_FISH; ++i) {             // Move each fish
        fishGroup[i].x += fishGroup[i].speed;
        if (fishGroup[i].x > 600)                    // Wrap fish to left if it goes out of screen
            fishGroup[i].x = -600;
    }

    for (int i = 0; i < NUM_BIRDS; ++i) {            // Animate birds
        birds[i].x += birds[i].speedX;               // Move in X
        birds[i].y += birds[i].speedY;               // Move in Y

        if (birds[i].y > 95 || birds[i].y < 75)      // Reverse Y direction at bounds
            birds[i].speedY *= -1;

        if (birds[i].x > 600) {                      // Wrap bird to left if it flies out of screen
            birds[i].x = -600;
            birds[i].y = 80 + rand() % 10;           // Reset Y randomly
        }
    }

    glutPostRedisplay();                             // Request display update
    glutTimerFunc(16, timer, 0);                     // Call timer again after ~16 ms (~60 FPS)
}

void reshape(int w, int h) {                         // Reshape callback when window size changes
    if (h == 0) h = 1;                               // Avoid division by zero
    glViewport(0, 0, w, h);                          // Set viewport to new dimensions
    glMatrixMode(GL_PROJECTION);                    // Switch to projection matrix
    glLoadIdentity();
    gluPerspective(70, (float)w / h, 1, 1000);       // Set perspective projection
    glMatrixMode(GL_MODELVIEW);                     // Switch back to modelview
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {     // Handle keyboard input
    switch (key) {
    case 'w': case 'W': camZ -= 5.0f; break;         // Move camera forward
    case 's': case 'S': camZ += 5.0f; break;         // Move camera backward
    case 'a': case 'A': camX -= 5.0f; break;         // Move camera left
    case 'd': case 'D': camX += 5.0f; break;         // Move camera right
    }
    glutPostRedisplay();                             // Update scene
}

void mouse(int button, int state, int x, int y) {    // Handle mouse clicks
    if (state == GLUT_DOWN) {                        // On button press
        if (button == GLUT_LEFT_BUTTON) boatMoving = true;  // Start boat
        else if (button == GLUT_RIGHT_BUTTON) boatMoving = false; // Stop boat
    }
}

int main(int argc, char** argv) {                    // Main function
    glutInit(&argc, argv);                           // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Double buffer, RGB color, depth buffer
    glutInitWindowSize(1000, 700);                   // Window size
    glutCreateWindow("River Scene - Boat and Clouds"); // Create window with title
    init();                                          // Call init function
    glutDisplayFunc(display);                        // Register display callback
    glutReshapeFunc(reshape);                        // Register reshape callback
    glutKeyboardFunc(keyboard);                      // Register keyboard callback
    glutMouseFunc(mouse);                            // Register mouse callback
    glutTimerFunc(0, timer, 0);                      // Start timer
    glutMainLoop();                                  // Start GLUT event loop
    return 0;                                        // Exit program
}
