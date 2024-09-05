#pragma once

#define WIN32_LEAN_AND_MEAN 1
#define GLFW_NO_GLU 1
#define GLFW_INCLUDE_GLCOREARB 1

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <Windows.h>

namespace ES
{    
    GLFWwindow* CreateAppWindow(int, int, const char*);
}