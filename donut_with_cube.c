#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

float zBuffer[160 * 44];
char buffer[160 * 44];

int width = 160;
int height = 44;

float thetaSpacing = 0.02;
float phiSpacing = 0.01;

int R1 = 1;
int R2 = 2;

int K2 = 8;
int K1 = 40;

int cubeWidth = 10;

float xRotation = 0, zRotation = 0;

float horizontalOffset = -45;

int findBufferIndex(int xp, int yp) {
    return xp + ((yp - 1) * width);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, char ch) {
    float x = cos(zRotation) * (cubeX * cos(xRotation) + sin(xRotation) * (cubeY * sin(xRotation) + cubeZ * cos(xRotation))) + sin(zRotation) * (cubeY * cos(xRotation) - cubeZ * sin(xRotation));
    float y = cos(zRotation) * (cubeY * cos(xRotation) - cubeZ * sin(xRotation)) - sin(zRotation) * (cubeX * cos(xRotation) + sin(xRotation) * (cubeY * sin(xRotation) + cubeZ * cos(xRotation)));
    float z = cos(xRotation) * (cubeY * sin(xRotation) + cubeZ * cos(xRotation)) - cubeX * sin(xRotation) + 100;

    float ooz = 1 / z;

    float projectionX = (int)(width * 0.5 + horizontalOffset + K1 * ooz * x * 2);
    float projectionY = (int)(height * 0.5 - K1 * ooz * y);

    int bufferIndex = findBufferIndex(projectionX, projectionY);

    if (bufferIndex >= 0 && bufferIndex < width * height) {
        if (ooz > zBuffer[bufferIndex]) {
            buffer[bufferIndex] = ch;
            zBuffer[bufferIndex] = ooz;
        }
    }
}

void fillDonutDataInBufferArr(float xRotation, float yRot) {
    float cosX = cos(xRotation);
    float sinX = sin(xRotation);
    float cosZ = cos(yRot);
    float sinZ = sin(yRot);

    for (float theta = 0; theta < 6.28; theta += thetaSpacing) {
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (float phi = 0; phi < 6.28; phi += phiSpacing) {
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            float circleX = R2 + cosTheta;
            float circleY = sinTheta;

            float x = circleX * (cosZ * cosPhi + sinX * sinZ * sinPhi) - circleY * cosX * sinZ;
            float y = circleX * (cosPhi * sinZ - cosZ * sinX * sinPhi) + circleY * cosX * cosZ;
            float z = K2 + cosX * circleX * sinPhi + circleY * sinX;
            float ooz = 1 / z;

            int projectionX = (int)horizontalOffset + width * 0.5 + K1 * ooz * x * 2;
            int projectionY = (int)height * 0.5 - K1 * ooz * y;

            float nx = cosZ * R1 * cosTheta * cosPhi - sinZ * (cosX * sinTheta - sinX * R1 * cosTheta * sinPhi);
            float ny = sinZ * R1 * cosTheta * cosPhi + cosZ * (cosX * sinTheta - sinX * R1 * cosTheta * sinPhi);
            float nz = sinX * sinTheta + cosX * cosTheta * sinPhi;

            int bufferIndex = findBufferIndex(projectionX, projectionY);
            float L = nx * 0 + ny * 0.7071 + nz * -0.7071;
            int lumIndex = L * 6;

            if (bufferIndex >= 0 && bufferIndex < width * height) {
                if (ooz > zBuffer[bufferIndex]) {
                    zBuffer[bufferIndex] = ooz;
                    buffer[bufferIndex] = ".,-~:;=!*#$@"[lumIndex > 0 ? lumIndex : 0];
                }
            }
        }
    }
}

void fillCubeDataInBufferArray() {
    for (float cubeX = -cubeWidth; cubeX <= cubeWidth; cubeX += 0.9) {
        for (float cubeY = -cubeWidth; cubeY <= cubeWidth; cubeY += 0.9) {
            calculateForSurface(cubeX, cubeY, -cubeWidth, ':');
            calculateForSurface(cubeX, cubeY, cubeWidth, '.');
            calculateForSurface(cubeWidth, cubeY, cubeX, '!');
            calculateForSurface(-cubeWidth, cubeY, -cubeX, ',');
            calculateForSurface(cubeX, cubeWidth, cubeY, '-');
            calculateForSurface(cubeX, -cubeWidth, -cubeY, '~');
        }
    }
}


void renderFrame() {
    printf("\x1b[H");

    for (int i = 0; i <= width * height; i++) {
        putchar(i % width ? buffer[i] : 10);
    }
}

int main() {
    printf("\x1b[2J");

    for (;;) {
        memset(buffer, ' ', sizeof(buffer));
        memset(zBuffer, 0, sizeof(zBuffer));

        xRotation += 0.04;
        zRotation -= 0.02;

        horizontalOffset = -45;
        K2 = 8;
        fillDonutDataInBufferArr(xRotation, zRotation);

        horizontalOffset = 10;
        fillCubeDataInBufferArray();

        horizontalOffset = 50;
        K2 = 14;
        fillDonutDataInBufferArr(xRotation + 0.4, zRotation - 0.3);

        renderFrame();

        usleep(20000);
    }

    return 0;
}
