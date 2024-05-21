#define STB_IMAGE_IMPLEMENTATION


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb-master/stb.h>
#include <iostream>
#include <stb-master/stb_image.h>
#include <Windows.h>
#include<math.h>
#include "shader_m.h"

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

//interpolation value
float t = 0;
float delt = 0.01;

//ellipse a,b,points,i
float le;
float se;
float a;
float b;
int points;

float elip[999]; //position of ellipse
float sqre[999]; //position of square

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void drawEllipse();
void drawSquare();



class part1 {
public:
	part1(GLFWwindow *window);
};

part1::part1(GLFWwindow* window) {
    Shader ourShader("vshader.txt", "fshader.txt");

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("mouth5.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        a = (float)width / 1200;
        b = (float)height / 1200;
        //std::cout << "width " << width << " ";

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);




    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //int width1, height1, nrChannels1;
    data = stbi_load("mask.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        le = (float)width / 1200;
        se = (float)height / 1200;

        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    points = (int)2 * 70 * 2 * le;
    //100 overflow?



    drawEllipse();
    drawSquare();

    unsigned int indices[999];

    for (int j = 0; j < points - 3; j++) {
        indices[j * 3] = j;
        indices[j * 3 + 1] = j + 1;
        indices[j * 3 + 2] = j + 2;
    }



    unsigned int VBOs[2], VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBOs);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(elip), elip, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sqre), sqre, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);


    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);



    std::cout << " a b le se " << a << " " << b << " " << le << " " << se << " ";


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.use();
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, points + 1, GL_UNSIGNED_INT, 0);


        //change the value of t(interpolation value)
        t += delt;
        //std::cout << "t++ ";
        Sleep(30);
        if (t > 1.0 || t < 0) {	//draw back and forth.
            //std::cout << "delt change ";
            delt = -delt;
        }

        // set the value
        ourShader.use();
        ourShader.setFloat("t", t);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

float elipY(float x) {
    return sqrt(b * b * (1 - x * x / (a * a)));
}

void drawEllipse() {
    float s = 3 * 2 * 2 * a / points; //distance
    float x, y;
    for (int t = 0, t1 = 0; t <= points + 6; t += 6, t1++) {  //total (points/3) points to draw the ellipse!!!
        x = -a + s * t1;
        y = elipY(x);

        elip[t] = x;
        elip[t + 1] = y;
        elip[t + 2] = 0;

        elip[t + 3] = x;
        elip[t + 4] = -y;
        elip[t + 5] = 0;

    }
}

void drawSquare() {
    float n = points / (2 * 3); //number of points of one long edge

    float s = 2 * le / n; //distance

    float x;
    for (int t = 0, t2 = 0; t <= points + 6; t += 6, t2++) {
        x = -le + t2 * s;

        sqre[t] = x;
        sqre[t + 1] = se;
        sqre[t + 2] = 0;

        sqre[t + 3] = x;
        sqre[t + 4] = -se;
        sqre[t + 5] = 0;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}