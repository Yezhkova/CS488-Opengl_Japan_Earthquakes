#include <cstring>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits.h>
#include "helper.h"
#include <chrono>

using namespace std;
void specialKeyboard(int c, int x, int y){
    switch(c)
    {
    case GLUT_KEY_LEFT:
        zoomlevelA -= viewportDelta;
        zoomlevelD += viewportDelta;
        break;
    case GLUT_KEY_RIGHT:
        zoomlevelA += viewportDelta;
        zoomlevelD -= viewportDelta;
        break;
    case GLUT_KEY_UP:
        zoomlevelS += viewportDelta;
        zoomlevelW -= viewportDelta;
        break;
    case GLUT_KEY_DOWN:
        zoomlevelS -= viewportDelta;
        zoomlevelW += viewportDelta;
        break;
    }
}

void registerViewportTransform(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(canvasSize[0]- zoomlevelA,
               canvasSize[1]+ zoomlevelD,
               canvasSize[2]- zoomlevelS,
               canvasSize[3]+ zoomlevelW);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

void registerBorderedQuakes(){
    glBegin(GL_LINES);
    for(auto& e: earthquakes) {
        if(e.mag <= magBorder) continue;
        drawQuakePoints(e);
    }
    glEnd();
}

void registerLabeledQuakes(){
    glBegin(GL_LINES);
    for(auto& e: earthquakes) {
        if(e.absTime < startTimer || e.absTime > endTimer) continue;
        if(e.mag <= magBorder) continue;
        text = e.year + '.' + e.month + '.' + e.day;
        drawQuakePoints(e);
    }
    glEnd();
}

void registerText(){
    glColor3f(1,1,1);
    glRasterPos2f(canvasSize[0]-0.7, canvasSize[2]-0.7);
    int len = text.size();
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void registerTerrain(){
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    for(auto& e: geoPoints){
        glVertex2f(e.lat, e.lon);
    }
    glEnd();
}

void anim(int x){
    switch (x) {
    case 0: // play default animation
        if(forwardMode) {
            startTimer += spanDelta;
            endTimer += spanDelta;
            glutPostRedisplay();
            glutTimerFunc(1, anim, 0);
        }
        break;

    case 1: //stop animation
        setDefault();
        magBorder = 3;
        text = "magnitude > 3";
        break;

    case 2: // backwards
        if(backwardsMode){
            startTimer -= spanDelta;
            endTimer -= spanDelta;
            glutPostRedisplay();
            glutTimerFunc(1, anim, 2);
        }
        break;

    case 3: // pause
        if(pauseMode){
            glutPostRedisplay();
            glutTimerFunc(1, anim, 3);
        }
        break;
    } 
}

void display(void) {
    registerViewportTransform();
    if(forwardMode || backwardsMode || pauseMode) {
        registerLabeledQuakes();
    }
    else {
        registerBorderedQuakes();
    }
    registerText();
    registerTerrain();
    glutSwapBuffers();
}

void idle(void){
    display();
}

void keyboard(unsigned char c, int x, int y) {
    if (c == 27) { // press escape
        exit(0);
    }
    if (c == '7'|| c == '8' || c == '9'){
        magBorder = 7;
        text = "magnitude > 7";
    }
    if (c == '6'){
        magBorder = 6;
        text = "magnitude > 6";
    }
    if (c == '5'){
        magBorder = 5;
        text = "magnitude > 5";
    }
    if (c == '4'){
        magBorder = 4;
        text = "magnitude > 4";
    }
    if (c == '3' || c == '2' || c == '1' || c == '0'){
        magBorder = 3;
        text = "magnitude > 3";
    }
    if(c == 'A' || c == 'a'){
        zoomIn();
    }
    if(c == 'Z' || c == 'z'){
        zoomOut();
    }
    if(c == 'O' || c == 'o'){
        animSet(0,0,0);
        glutTimerFunc(1, anim, 1);
    }
    if(c == 'P' || c == 'p'){
        if(!forwardMode && !backwardsMode && !pauseMode){
            animSet(1,0,0);
            startTimer = earthquakes.begin()->absTime;
            endTimer = startTimer + month;
            magBorder = 3;
            glutTimerFunc(1, anim, 0);
        }
    }
    if(c == 'G' || c == 'g'){
        if(!backwardsMode){
            animSet(0,1,0);
            glutTimerFunc(1, anim, 2);
        }
    }
    if(c == 'H' || c == 'h'){
        if(!forwardMode){
            animSet(1,0,0);
            glutTimerFunc(1, anim, 0);
        }
    }
    if(c == 32){ // spacebar
        if(!pauseMode){
            if(forwardMode){
                prevF = 1;
                prevB = 0;
            }
            if(backwardsMode){
                prevF = 0;
                prevB = 1;
            }
            animSet(0,0,1);
            glutTimerFunc(1, anim, 3);
        }
        else{
            if(prevF){
                animSet(1,0,0);
                glutTimerFunc(1, anim, 0);
            }
            if(prevB){
                animSet(0,1,0);
                glutTimerFunc(1, anim, 2);
            }
        }
    }
}

int main(int argc, char **argv) {
    readJapanMap();
    evalCanvas();
    readQuakesMap();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W_SIZE_X, W_SIZE_Y);
    glutCreateWindow("Earthquakes Japan");

    if (glutGet(GLUT_WINDOW_COLORMAP_SIZE) != 0) {
        printf("FAIL: bad RGBA colormap size\n");
        exit(1);
    }

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
