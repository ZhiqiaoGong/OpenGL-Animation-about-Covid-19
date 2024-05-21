/*the synthesize experiment
* just connect exp 1~5
* add a special shader for text! (however, text is just picture...) 
*/
#define STB_IMAGE_IMPLEMENTATION


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb-master/stb.h>
#include <iostream>
#include <stb-master/stb_image.h>
#include<glm-master/glm/glm.hpp>
#include<glm-master/glm/gtc/matrix_transform.hpp>
#include<glm-master/glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include<istream>
#include<fstream>
#include<math.h>
#include "shader_m.h"
#include "CHCurve.h"
#include"AudioClip.h"

using namespace std;

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

//function for global
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//function for part 1
void drawEllipse();
void drawSquare();

//function for part 2
void drawCircle();
float drawPath(float x);
float getAngle(float x);

//function for part 3
void readObj(char* path, float* r);

//function for part 4
class man {
public:
    glm::mat4 trans_body;
    glm::mat4 trans_upperarm;
    glm::mat4 trans_lowerarm;

    man();
    man(glm::mat4 tb, glm::mat4 tu, glm::mat4 tl) {
        trans_body = tb;
        trans_upperarm = tu;
        trans_lowerarm = tl;
    }
};

//functioon for part 5
void changePPos(int i);//change the place of particles(people)


//data for global
int part = 1;
float delt = 0.01;
int flag=1;


//data for part1
float t1_0 = 0;//interpolation value 1
float le;
float se;
float a;
float b;
int ippoints;//ellipse a,b,points,i
float elip[999]; //position of ellipse
float sqre[999]; //position of square

//data for part 2
const float Pi = 3.14;
float r;//size of ncov
int points;//number of points
float circle[999];//points of the circle of the ncov
glm::vec3 crPoints[4];//points to draw the catmall-rom curve

//data for part 3
const int s = 121;
float sourc[s * 3]; //source image points (screen coordinate)
float destn[s * 3]; //target image points (screen coordinate)
float t3_1 = 0;//interpolation value
float t3_2 = 1;//interpolation value 2

//data for part 4
glm::vec3 t1[3];  //save the transformation
glm::vec3 t2[3];

//the transformation&rotation!
glm::mat4 t11 = glm::mat4(1.0f);
glm::mat4 t12 = glm::mat4(1.0f);
glm::mat4 t13 = glm::mat4(1.0f);
glm::mat4 t21 = glm::mat4(1.0f);
glm::mat4 t22 = glm::mat4(1.0f);
glm::mat4 t23 = glm::mat4(1.0f);
glm::mat4 tmp1 = glm::mat4(1.0f);
glm::mat4 tmp2 = glm::mat4(1.0f);

float tran11 = 0;
float rota11 = 0;
float tran12 = 0;
float rota12 = 0;
float tran13 = 0;
float tran13_2 = 0;
float rota13 = 0;
float tran21 = 0;
float rota21 = 0;
float tran22 = 0;
float rota22 = 0;
float tran22_2 = 0;
float tran23 = 0;
float tran23_2 = 0;
float rota23 = 0;

int i = 0;//fps

//data for part 5
const int pppoints = 50;//number of particle(people)  
//we can define many kinds of people to simulate the crowd -> just bind different texture for different texture
glm::vec3 particles[pppoints]; //position of particles
int ptex[pppoints]; //define which testure the particles bind

float xdelt = 0;
float ydelt = 0;
float zdelt = 0;

float ax = 0.01; //accelerate speed on x axis
float ay = 0.01; //accelerate speed on y axis

int movtime = 0;

int main()
{
    AudioClip ac;
    ac.load("Roam.mp3");
    ac.play();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glfwSetWindowPos(window, 100, 100);



    // build and compile our shader program
    // ------------------------------------
    Shader ourShader_text("vshader0.txt", "fshader0.txt");

    Shader ourShader_intp("vshader1.txt", "fshader1.txt");

    Shader ourShader_bg("vshader2_1.txt", "fshader2.txt");  //for the circle ->virus
    Shader ourShader_cov("vshader2_2.txt", "fshader2.txt"); //for the square -> background

    Shader ourShader_sd("vshader3.txt", "fshader3.txt");

    Shader ourShader_bg4("vshader4_1.txt", "fshader4_1.txt");  //for the square -> background
    Shader ourShader_b1("vshader4_2.txt", "fshader4_2.txt"); //for the square -> body1
    Shader ourShader_ua1("vshader4_2.txt", "fshader4_2.txt"); //for the square -> upper arm1
    Shader ourShader_la1("vshader4_3.txt", "fshader4_2.txt"); //for the square -> lower arm1
    Shader ourShader_b2("vshader4_2.txt", "fshader4_2.txt"); //for the square -> body2
    Shader ourShader_ua2("vshader4_2.txt", "fshader4_2.txt"); //for the square -> upper arm2
    Shader ourShader_la2("vshader4_4.txt", "fshader4_2.txt"); //for the square -> lower arm2

    //Shader ourShader_("vshader5_1.txt", "fshader5_1.txt");  //for the square -> background
    Shader ourShader_ptcl("vshader5_2.txt", "fshader5_2.txt"); //for the square -> particle 1

    //part 0
    float tpos[12] = {
        -0.9,-0.65,-0.02,
        -0.9,-1.05,-0.02,
         0.9,-0.65,-0.02,
         0.9,-1.05,-0.02
    };
    float ttex[8] = {
        0,0,
        0,1,
        1,0,
        1,1
    };

    unsigned int text01,text02, text03, text04, text05;
    // ---------
    glGenTextures(1, &text01);
    glBindTexture(GL_TEXTURE_2D, text01);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("cv1.png", &width, &height, &nrChannels, 0);
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &text02);
    glBindTexture(GL_TEXTURE_2D, text02);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("cv2.png", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &text03);
    glBindTexture(GL_TEXTURE_2D, text03);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("cv3.png", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &text04);
    glBindTexture(GL_TEXTURE_2D, text04);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("cv4.png", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glGenTextures(1, &text05);
    glBindTexture(GL_TEXTURE_2D, text05);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("cv5.png", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    ourShader_text.use();
    ourShader_text.setInt("texture", 13);




    //part1
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
    //int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("mouth.png", &width, &height, &nrChannels, 0);
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

    ippoints = (int)2 * 90 * 2 * le;
    drawEllipse();
    drawSquare();
    unsigned int indices[999];
    for (int j = 0; j < ippoints - 3; j++) {
        indices[j * 3] = j;
        indices[j * 3 + 1] = j + 1;
        indices[j * 3 + 2] = j + 2;
    }
    ourShader_intp.use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader_intp.setInt("texture1", 0);
    ourShader_intp.setInt("texture2", 1);


    //part2
    //set the curve points and draw cr curve
    crPoints[0] = glm::vec3(-0.6, -0.1, 0);
    crPoints[1] = glm::vec3(-0.25, -0.11, 0);
    crPoints[2] = glm::vec3(0.25, 0.1, 0);
    crPoints[3] = glm::vec3(0.8, -0.0, 0);
    CHCurve cr = CHCurve(crPoints);
    //set the size of ncov
    r = 0.1;
    points = 2 * r * 50;
    //fraw the ncov circle
    drawCircle();
    //draw the background square
    float bgPoints[12] = {
        -1.2,0.8,-1,
        -1.2,-0.8,-1,
        1.2,0.8,-1,
        1.2,-0.8,-1
    };
    //set background texcoord
    float bgTex[8] = {
        0,1,
        0,0,
        1,1,
        1,0
    };
    unsigned int indices2[999];
    for (int j = 0; j < 50; j++) {
        indices2[j * 3] = j;
        indices2[j * 3 + 1] = j + 1;
        indices2[j * 3 + 2] = j + 2;
    }
    unsigned int texture2_1, texture2_2;
    // texture 1
    // ---------
    glGenTextures(1, &texture2_1);
    glBindTexture(GL_TEXTURE_2D, texture2_1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("getcov.jpg", &width, &height, &nrChannels, 0);

    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2_2);
    glBindTexture(GL_TEXTURE_2D, texture2_2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("ncov3.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    ourShader_bg.use();
    ourShader_bg.setInt("texture", 2);

    ourShader_cov.use();
    ourShader_cov.setInt("texture", 3);

    int times = 0;
    float angle = 0;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 tmodel = glm::mat4(1.0f);


    //part 3
    //1.get the points of the source and destination image
    readObj((char*)"pic111.txt", sourc);
    readObj((char*)"pic222.txt", destn);

    int indc[20 * 10 * 3];
    for (size_t l = 0; l < 10; l++)//each line
    {
        for (size_t i = 0; i < 10; i++)
        {
            indc[l * 20 * 3 + 6 * i] = l * 11 + i;
            indc[l * 20 * 3 + 6 * i + 1] = (l + 1) * 11 + i;
            indc[l * 20 * 3 + 6 * i + 2] = l * 11 + i + 1;

            indc[l * 20 * 3 + 6 * i + 3] = (l + 1) * 11 + i;
            indc[l * 20 * 3 + 6 * i + 4] = l * 11 + i + 1;
            indc[l * 20 * 3 + 6 * i + 5] = (l + 1) * 11 + i + 1;
        }
    }
    // load and create a texture
    // -------------------------
    unsigned int texture3_1, texture3_2;
    // texture 1
    // ---------
    glGenTextures(1, &texture3_1);
    glBindTexture(GL_TEXTURE_2D, texture3_1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("cough1.jpg", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture3_2);
    glBindTexture(GL_TEXTURE_2D, texture3_2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("cough0.jpg", &width, &height, &nrChannels, 0);
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader_sd.use();
    ourShader_sd.setInt("texture1", 4);
    ourShader_sd.setInt("texture2", 5);


    //part4
    //init the transformation
    tran11 = -0.16;
    tran21 = 0.16;
    rota12 = -10;
    rota22 = 23;



    t11 = glm::translate(t11, glm::vec3(tran11, 0.0f, 0.0f));

    t12 = glm::translate(t12, glm::vec3(tran12, 0.0f, 0.0f));
    t12 = glm::rotate(t12, glm::radians(rota12), glm::vec3(0.0, 0.0, 1.0));

    t13 = glm::translate(t13, glm::vec3(tran13, 0.0f, 0.0f));
    t13 = glm::rotate(t13, glm::radians(rota13), glm::vec3(0.0, 0.0, 1.0));


    t21 = glm::translate(t21, glm::vec3(tran21, 0.0f, 0.0f));

    t22 = glm::translate(t22, glm::vec3(tran22, 0.0f, 0.0f));
    t22 = glm::rotate(t22, glm::radians(rota22), glm::vec3(0.0, 0.0, 1.0));

    t23 = glm::translate(t23, glm::vec3(tran23, 0.0f, 0.0f));
    t23 = glm::rotate(t23, glm::radians(rota23), glm::vec3(0.0, 0.0, 1.0));


    //draw the people1&2 square
    float pp[12] = {
        -0.3, 0.3,-1,
        -0.3,-0.3,-1,
         0.3, 0.3,-1,
         0.3,-0.3,-1,
    };
    //set the people texcoord
    //same as bgTex

    //for the hower hand 1...
    float pp2[12] = {
        -0.08, 0.08,-1.01,
        -0.08,-0.08,-1.01,
         0.08, 0.08,-1.01,
         0.08,-0.08,-1.01,
    };
    //for the hower hand 2...
    float pp3[12] = {
        -0.5, 0.5,-1.01,
        -0.5,-0.5,-1.01,
         0.5, 0.5,-1.01,
         0.5,-0.5,-1.01,
    };

    //draw the background square
    float bgPoints4[12] = {
        -1.2,1.2,-1,
        -1.2,-1.2,-1,
         1.2,1.2,-1,
         1.2,-1.2,-1
    };
    //set background texcoord
    float bgTex4[8] = {
        0,1,
        0,0,
        1,1,
        1,0
    };


    unsigned int texture4_1, texture4_2, texture4_3, texture4_4, texture4_5, texture4_6, texture4_7;
    // texture 1
    // ---------
    glGenTextures(1, &texture4_1);
    glBindTexture(GL_TEXTURE_2D, texture4_1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("manmask1.png", &width, &height, &nrChannels, 0);

    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture4_2);
    glBindTexture(GL_TEXTURE_2D, texture4_2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("manmask1_1.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 3
    // ---------
    glGenTextures(1, &texture4_3);
    glBindTexture(GL_TEXTURE_2D, texture4_3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("manmask1_2.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 4
    // ---------
    glGenTextures(1, &texture4_4);
    glBindTexture(GL_TEXTURE_2D, texture4_4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("manmask2.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 5
    // ---------
    glGenTextures(1, &texture4_5);
    glBindTexture(GL_TEXTURE_2D, texture4_5);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("manmask2_1.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // texture 6
    // ---------
    glGenTextures(1, &texture4_6);
    glBindTexture(GL_TEXTURE_2D, texture4_6);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("manmask2_2.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 7
    // ---------
    glGenTextures(1, &texture4_7);
    glBindTexture(GL_TEXTURE_2D, texture4_7);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("street.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader_bg4.use();
    ourShader_bg4.setInt("texture", 12);

    ourShader_b1.use();
    ourShader_b1.setInt("texture", 6);

    ourShader_ua1.use();
    ourShader_ua1.setInt("texture", 7);

    ourShader_la1.use();
    ourShader_la1.setInt("texture", 8);

    ourShader_b2.use();
    ourShader_b2.setInt("texture", 9);

    ourShader_ua2.use();
    ourShader_ua2.setInt("texture", 10);

    ourShader_la2.use();
    ourShader_la2.setInt("texture", 11);

    //set the transformation!
    man man1(t11, t12, t13), man2(t21, t22, t23);

    //part 5
    //draw the people square
    float pp_5[12] = {
        -0.5, 0.5,0,
        -0.5,-0.5,0,
         0.5, 0.5,0,
         0.5,-0.5,0,
    };

    //set the people texcoord
    //same as bgTex


    //draw the background square
    //float bgPoints[12] = {
    //    -1.2,1.2,-1,
    //    -1.2,-1.2,-1,
    //     1.2,1.2,-1,
    //     1.2,-1.2,-1
    //};
    ////set background texcoord
    //float bgTex[8] = {
    //    0,1,
    //    0,0,
    //    1,1,
    //    1,0
    //};

    //init the particles translation
    for (int i = 0; i < points; i++) {
        particles[i].x = 0;
        particles[i].y = 0.035;
        particles[i].z = -0.5;
    }

    //init binding
    //1->tex1 2->tex2 3->tex3 4->tex4
    srand((unsigned)time(NULL));
    for (int i = 0; i < pppoints; i++) {
        ptex[i] = rand() % 5 ;
    }



    unsigned int texture5_1, texture5_2, texture5_3, texture5_4, texture5_5;
    // texture 1
    // ---------
    //glGenTextures(1, &texture5_1);
    //glBindTexture(GL_TEXTURE_2D, texture5_1);
    //// set the texture wrapping parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //// set texture filtering parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //// load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    //data = stbi_load("street.png", &width, &height, &nrChannels, 0);

    //GLenum format;
    //if (nrChannels == 1)
    //    format = GL_RED;
    //else if (nrChannels == 3)
    //    format = GL_RGB;
    //else if (nrChannels == 4)
    //    format = GL_RGBA;

    //if (data)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
    //    std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture5_2);
    glBindTexture(GL_TEXTURE_2D, texture5_2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("p1.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 3
    // ---------
    glGenTextures(1, &texture5_3);
    glBindTexture(GL_TEXTURE_2D, texture5_3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("p2.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 4
    // ---------
    glGenTextures(1, &texture5_4);
    glBindTexture(GL_TEXTURE_2D, texture5_4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("p3.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 5
    // ---------
    glGenTextures(1, &texture5_5);
    glBindTexture(GL_TEXTURE_2D, texture5_5);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("p4.png", &width, &height, &nrChannels, 0);


    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    //ourShader_ptcl.use();
    //ourShader_ptcl.setInt("texture", 0);

    glm::mat4 init = glm::mat4(1.0f);
    init = glm::translate(init, glm::vec3(0.0f, -0.1f, -3.0f));



    //setting vao and vbo
    
    //000
    unsigned int vaot, vbopt,vbott;
    glGenVertexArrays(1, &vaot);
    glGenBuffers(1, &vbopt);
    glGenBuffers(1, &vbott);

    glBindVertexArray(vaot);
    glBindBuffer(GL_ARRAY_BUFFER, vbopt);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tpos), tpos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbott);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ttex), ttex, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //111
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

    //222
    unsigned int VAO_cl, VAO_bg, VBO_cl, VBO_bgpos, VBO_bgtex, EBO2;
    glGenVertexArrays(1, &VAO_cl);
    glGenVertexArrays(1, &VAO_bg);
    glGenBuffers(1, &VBO_cl);
    glGenBuffers(1, &VBO_bgpos);
    glGenBuffers(1, &VBO_bgtex);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO_cl);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cl);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);


    glBindVertexArray(VAO_bg);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_bgpos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgPoints), bgPoints, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_bgtex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgTex), bgTex, GL_STATIC_DRAW);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    //333
    unsigned int VAO_sd, VBO_s, VBO_d, EBO3;
    glGenVertexArrays(1, &VAO_sd);
    glGenBuffers(1, &VBO_s);
    glGenBuffers(1, &VBO_d);
    glGenBuffers(1, &EBO3);


    glBindVertexArray(VAO_sd);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_s);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sourc), sourc, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indc), indc, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_d);
    glBufferData(GL_ARRAY_BUFFER, sizeof(destn), destn, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indc), indc, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);

    //444
    //different vao but use the same shader
    unsigned int VAO_pp11, VAO_pp12, VAO_pp13, VAO_pp21, VAO_pp22, VAO_pp23, VAO_bg4,
        VBO_pp11, VBO_pp12, VBO_pp13, VBO_pp21, VBO_pp22, VBO_pp23, VBO_bgpos4, VBO_bgtex4, EBO4;
    glGenVertexArrays(1, &VAO_pp11);
    glGenVertexArrays(1, &VAO_pp12);
    glGenVertexArrays(1, &VAO_pp13);
    glGenVertexArrays(1, &VAO_pp21);
    glGenVertexArrays(1, &VAO_pp22);
    glGenVertexArrays(1, &VAO_pp23);
    glGenVertexArrays(1, &VAO_bg4);
    glGenBuffers(1, &VBO_pp11);
    glGenBuffers(1, &VBO_pp12);
    glGenBuffers(1, &VBO_pp13);
    glGenBuffers(1, &VBO_pp21);
    glGenBuffers(1, &VBO_pp22);
    glGenBuffers(1, &VBO_pp23);
    glGenBuffers(1, &VBO_bgpos4);
    glGenBuffers(1, &VBO_bgtex4);
    glGenBuffers(1, &EBO4);


    glBindVertexArray(VAO_pp11);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp11);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp), pp, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    ////indices
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_pptex);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(bgTex), bgTex, GL_STATIC_DRAW);
    //// texture attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);

    glBindVertexArray(VAO_pp12);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp12);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp), pp, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_pp13);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp13);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp2), pp2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_pp21);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp21);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp), pp, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_pp22);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp22);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp), pp, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_pp23);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp23);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp3), pp3, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindVertexArray(VAO_bg4);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_bgpos4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgPoints4), bgPoints4, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_bgtex4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgTex4), bgTex4, GL_STATIC_DRAW);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    //555
    unsigned int VAO_pp,  VBO_pp;
    glGenVertexArrays(1, &VAO_pp);
    //glGenVertexArrays(1, &VAO_bg);
    glGenBuffers(1, &VBO_pp);
    //glGenBuffers(1, &VBO_bgpos);
    //glGenBuffers(1, &VBO_bgtex);
    //glGenBuffers(1, &EBO);


    glBindVertexArray(VAO_pp);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pp);
    //vecToArry(tex1, tex1P);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pp_5), pp_5, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    ////indices
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_pptex);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(bgTex), bgTex, GL_STATIC_DRAW);
    //// texture attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);



    //glBindVertexArray(VAO_bg);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_bgpos);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(bgPoints), bgPoints, GL_STATIC_DRAW);
    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_bgtex);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(bgTex), bgTex, GL_STATIC_DRAW);
    //// texture attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)&&flag)
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!


        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture2_1);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture2_2);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture3_1);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture3_2);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture4_1);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture4_2);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture4_3);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture4_4);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture4_5);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture4_6);
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_2D, texture4_7);
        
        

        switch (part) {
        case 1:
            //cout << "part1" << endl;

            Sleep(25);

            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, text01);
            ourShader_text.use();
            glBindVertexArray(vaot);
            glDrawArrays(GL_TRIANGLE_STRIP,0,4);

            ourShader_intp.use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, ippoints, GL_UNSIGNED_INT, 0);
            //change the value of t(interpolation value)
            if (t1_0 >= 0 && t1_0 < 1) {
                // set the value
                //cout << t1 << endl;
                t1_0 += delt;
                ourShader_intp.use();
                ourShader_intp.setFloat("t", t1_0);
                Sleep(20);
            }
            else{
                Sleep(15);
                part = 2;
            }
            break;

        case 2:
            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, text02);
            ourShader_text.use();
            glBindVertexArray(vaot);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            //cout << "part2" << endl;
            //cout<<times<<endl;
            //background!
            // activate shader
            ourShader_bg.use();
            // create transformations
            glm::mat4 model1 = glm::mat4(1.0f);
            glm::mat4 view1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 projection1 = glm::mat4(1.0f);
            projection1 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view1 = glm::translate(view1, glm::vec3(0.0f, 0.0f, -3.0f));
            view1 = glm::scale(view1, glm::vec3(1.1, 1.1, 0));
            // pass transformation matrices to the shader
            ourShader_bg.setMat4("projection1", projection1); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_bg.setMat4("view1", view1);
            //glm::mat4 model = glm::mat4(1.0f);
            ourShader_bg.setMat4("model1", model1);
            glBindVertexArray(VAO_bg);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            //virus!
            ourShader_cov.use();
            glm::mat4 view2 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            view2 = glm::scale(view2, glm::vec3(1.15, 1.15, 1.15));
            glm::mat4 projection2 = glm::mat4(1.0f);
            ourShader_cov.setMat4("projection2", projection2); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_cov.setMat4("view2", view2);
            // calculate the model matrix for each object and pass it to shader before drawing
            if (times > 38) {
                part = 3;
                break;
                //times = 0;
                //std::cout << "redraw!!!   ";
            }
            else if (times == 0) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-0.5, -0.2, 0));
            }
            model = glm::translate(model, glm::vec3(1.0 / 40, cr.pos(times * 2.0 / 40), 0));
            tmodel = model;
            model = glm::rotate(model, getAngle(-0.5 + times * 2.0 / 40), glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader_cov.setMat4("model2", model);
            model = tmodel;
            times++;
            Sleep(100);
            glBindVertexArray(VAO_cl);
            glDrawElements(GL_TRIANGLES, 50 * 3, GL_UNSIGNED_INT, 0);

            Sleep(15);
            break;

        case 3:    
            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, text03);
            ourShader_text.use();
            glBindVertexArray(vaot);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            ourShader_sd.use();
            //change the value of t(interpolation value)
            t3_1 += delt;
            //std::cout << "t++ ";
            if (t3_1 <= 1.0 && t3_1 >= 0) {	//draw back and forth.
                Sleep(15);
                t3_1 += delt;
                ourShader_sd.use();
                ourShader_sd.setFloat("t", t3_1);
            }
            if ((t3_1 >= 0.9 && t3_1 <=1) || (t3_1 <= 0.1&&t2>=0))Sleep(15);
            //draw the second time
            if (t3_2 >= 0 && t3_2 <= 1.0 && (t3_1 > 1.0)) {
                Sleep(15);
                t3_2 -= delt;
                ourShader_sd.use();
                ourShader_sd.setFloat("t", t3_2);
            }
            if ((t3_2 >= 0.9 && t3_2 <= 1) || (t3_2 <= 0.1 && t3_2 >= 0))Sleep(15);
            glBindVertexArray(VAO_sd);
            glDrawElements(GL_TRIANGLES, 20 * 10 * 3, GL_UNSIGNED_INT, 0);
            if ((t3_1 >1)&&t3_2<0) {
                part = 4;
                Sleep(15);
                break;
            }
            break;

        case 4:
            Sleep(5);
            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, text04);
            ourShader_text.use();
            glBindVertexArray(vaot);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            if (i <= 66) {//change the people place
                tran11 += 0.0009;
                tran21 -= 0.0009;

                rota12 += 1.15 / 1.5;
                rota22 -= 2.3 / 1.5;
                if (rota13 < 95)
                    rota13 += 2.4 / 1.5;
                if (rota23 > -70)
                    rota23 -= 1.8 / 1.5;
                //cout << "walk and clap" << endl;
            }
            else if (i > 66 && i <= 90) {
                rota12 += 1.15 / 1.5;
                rota22 -= 2.3 / 1.5;
                if (rota13 < 95)
                    rota13 += 2.4 / 1.5;
                if (rota23 > -70)
                    rota23 -= 1.8 / 1.5;
                //cout << "clap" << endl;
            }
            else if (i > 90) {
                /*i = 0;
                tran11 = -0.16;
                tran21 = 0.16;
                rota12 = -10;
                rota22 = 23;
                rota13 = rota23 = 0;
                cout << "restart" << endl;*/
                Sleep(50);
                part = 5;
                break;
            }
            t11 = glm::mat4(1.0f);
            t12 = glm::mat4(1.0f);
            t13 = glm::mat4(1.0f);
            t21 = glm::mat4(1.0f);
            t22 = glm::mat4(1.0f);
            t23 = glm::mat4(1.0f);

            //upper arms
            tran12 = -0.01;
            tran22 = 0.015;
            tran22_2 = -0.01;
            //lower arms
            tran13 = 0.08;
            tran13_2 = -0.074;
            tran23 = -0.0066;
            tran23_2 = -0.068;

            if (rota13 > 90) {
                tran13 = 0.081;
                tran13_2 = -0.076;
                tran12 = -0.018;
            }
            if (rota23 < -68) {
                tran22 = 0.004;
                tran22_2 = -0.01;
            }

            t11 = glm::translate(t11, glm::vec3(tran11, 0.0f, 0.0f));

            t12 = glm::rotate(t12, glm::radians(rota12), glm::vec3(0.0, 0.0, 1.0));
            t12 = glm::translate(t12, glm::vec3(0.0f, tran12, 0.0f));
            //t12 = glm::rotate(t12, glm::radians(rota12), glm::vec3(0.0, 0.0, 1.0));

            //t13 = glm::rotate(t13, glm::radians(rota13), glm::vec3(0.0, 0.0, 1.0));
            t13 = glm::translate(t13, glm::vec3(0.0f, tran13_2, 0.0f));
            t13 = glm::translate(t13, glm::vec3(tran13, 0.0f, 0.0f));
            t13 = glm::rotate(t13, glm::radians(rota13), glm::vec3(0.0, 0.0, 1.0));


            t21 = glm::translate(t21, glm::vec3(tran21, 0.0f, 0.0f));

            t22 = glm::translate(t22, glm::vec3(tran22, 0.0f, 0.0f));
            t22 = glm::translate(t22, glm::vec3(0.0f, tran22_2, 0.0f));
            t22 = glm::rotate(t22, glm::radians(rota22), glm::vec3(0.0, 0.0, 1.0));

            t23 = glm::translate(t23, glm::vec3(tran23, 0.0f, 0.0f));
            t23 = glm::translate(t23, glm::vec3(0.0f, tran23_2, 0.0f));
            t23 = glm::rotate(t23, glm::radians(rota23), glm::vec3(0.0, 0.0, 1.0));

            man1 = man(t11, t12, t13);
            man2 = man(t21, t22, t23);

            //background!
            // activate shader
            ourShader_bg4.use();
            // create transformations
            glm::mat4 model14 = glm::mat4(1.0f);
            glm::mat4 view14 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 projection14 = glm::mat4(1.0f);
            projection14 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view14 = glm::translate(view14, glm::vec3(-0.3f, 0.0f, -3.0f));
            view14 = glm::scale(view14, glm::vec3(2, 2, 2));

            // pass transformation matrices to the shader
            ourShader_bg4.setMat4("projection1", projection14); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_bg4.setMat4("view1", view14);
            ourShader_bg4.setMat4("model1", model14);

            glBindVertexArray(VAO_bg4);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            //body 1 !

            // activate shader
            ourShader_b1.use();

            // create transformations
            glm::mat4 model24 = glm::mat4(1.0f);
            glm::mat4 view24 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 projection24 = glm::mat4(1.0f);
            projection24 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            //view2 = glm::scale(view2, glm::vec3(0.4, 0.4, 0.4));
            ourShader_b1.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_b1.setMat4("model1", model24);

            //body
            view24 = view24 * man1.trans_body;
            ourShader_b1.setMat4("view1", view24);
            glBindVertexArray(VAO_pp11);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


            //upper  arm 1 !

            // activate shader
            ourShader_ua1.use();

            glBindVertexArray(VAO_pp12);

            // create transformations
            ourShader_ua1.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_ua1.setMat4("model1", model24);

            //upperarm
            view24 = view24 * man1.trans_upperarm;
            ourShader_ua1.setMat4("view1", view24);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            //lower  arm 1 !

            // activate shader
            ourShader_la1.use();

            glBindVertexArray(VAO_pp13);

            // create transformations
            ourShader_la1.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_la1.setMat4("model1", model24);
            //lowerarm
            view24 = view24 * man1.trans_lowerarm;
            ourShader_la1.setMat4("view1", view24);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


            //body 2 !

            // activate shader
            ourShader_b2.use();

            glBindVertexArray(VAO_pp21);

            // create transformations
            //glm::mat4 model2 = glm::mat4(1.0f);
            view24 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            ourShader_b2.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_b2.setMat4("model1", model24);

            //body
            view24 = view24 * man2.trans_body;
            ourShader_b2.setMat4("view1", view24);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


            //upper  arm 2 !

            // activate shader
            ourShader_ua2.use();

            glBindVertexArray(VAO_pp22);

            ourShader_ua2.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_ua2.setMat4("model1", model24);

            //upperarm
            view24 = view24 * man2.trans_upperarm;
            ourShader_ua2.setMat4("view1", view24);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


            //lower  arm 2 !

            // activate shader
            ourShader_la2.use();

            glBindVertexArray(VAO_pp13);

            ourShader_la2.setMat4("projection1", projection24); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_la2.setMat4("model1", model24);
            //lowerarm
            view24 = view24 * man2.trans_lowerarm;
            ourShader_la2.setMat4("view1", view24);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            i++;
            Sleep(10);
            break;

        case 5:
            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, text05);
            ourShader_text.use();
            glBindVertexArray(vaot);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            if (movtime > 1500) {
                part = 0;
                break;
            }
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture5_2);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture5_3);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture5_4);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture5_5);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, texture2_2);

            //background!
            // activate shader
            ourShader_bg4.use();
            // create transformations
            glm::mat4 model15 = glm::mat4(1.0f);
            glm::mat4 view15 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 projection15 = glm::mat4(1.0f);
            projection15 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view15 = glm::translate(view15, glm::vec3(-0.3f, 0.0f, -3.0f));
            view15 = glm::scale(view15, glm::vec3(2, 2, 2));

            // pass transformation matrices to the shader
            ourShader_bg4.setMat4("projection1", projection15); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_bg4.setMat4("view1", view15);
            ourShader_bg4.setMat4("model1", model15);

            glBindVertexArray(VAO_bg4);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


            //particles!
            // activate shader
            ourShader_ptcl.use();
            // create transformations
            glm::mat4 model25 = glm::mat4(1.0f);
            glm::mat4 view25 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            //model1 = glm::mat4(1.0f);
            //view1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            view25 = glm::translate(view25, glm::vec3(0.0f, -0.1f, -3.0f));
            glm::mat4 projection25 = glm::mat4(1.0f);
            projection25 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view25 = glm::scale(view25, glm::vec3(0.4, 0.4, 0.4));
            ourShader_ptcl.setMat4("projection1", projection25); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader_ptcl.setMat4("model1", model25);


            glBindVertexArray(VAO_pp);

            for (int i = 0; i < pppoints; i++) {

                changePPos(i);
                view25 = init;
                view25 = glm::translate(view25, glm::vec3(particles[i].x, particles[i].y, particles[i].z));
                // pass transformation matrices to the shader
                //ourShader_ptcl.setMat4("view1", view25);


                //set the texture
                if (ptex[i] == 4) {
                    view25 = glm::scale(view25, glm::vec3(0.2, 0.2, 0.2));
                    view25 = glm::translate(view25, glm::vec3(0, 1, 0.5));

                }
                ourShader_ptcl.setMat4("view1", view25);
                ourShader_ptcl.setInt("texture", ptex[i]);

                //cout << endl;
                //Sleep(1.0);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * 100);

            }
            Sleep(2);
            movtime++;
            break;
            //cout << part << endl;
            

        case 0:
            cout << "Thanks for watching!!!" << endl;
            flag = 0;
            break;

        }

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
    glDeleteVertexArrays(1, &VAO_cl);
    glDeleteVertexArrays(1, &VAO_cl);
    glDeleteVertexArrays(1, &VBO_cl);
    glDeleteVertexArrays(1, &VBO_bgpos);
    glDeleteVertexArrays(1, &VBO_bgtex);
    glDeleteBuffers(1, &EBO2);
    glDeleteVertexArrays(1, &VAO_sd);
    glDeleteVertexArrays(1, &VBO_s);
    glDeleteVertexArrays(1, &VBO_d);
    glDeleteBuffers(1, &EBO3);
    glDeleteVertexArrays(1, &VAO_pp11);
    glDeleteVertexArrays(1, &VAO_pp12);
    glDeleteVertexArrays(1, &VAO_pp13);
    glDeleteVertexArrays(1, &VAO_pp21);
    glDeleteVertexArrays(1, &VAO_pp22);
    glDeleteVertexArrays(1, &VAO_pp23);
    glDeleteVertexArrays(1, &VAO_bg4);
    glDeleteVertexArrays(1, &VBO_pp11);
    glDeleteVertexArrays(1, &VBO_pp12);
    glDeleteVertexArrays(1, &VBO_pp13);
    glDeleteVertexArrays(1, &VBO_pp21);
    glDeleteVertexArrays(1, &VBO_pp22);
    glDeleteVertexArrays(1, &VBO_pp23);
    glDeleteVertexArrays(1, &VBO_bgpos4);
    glDeleteVertexArrays(1, &VBO_bgtex4);
    glDeleteBuffers(1, &EBO4);
    glDeleteVertexArrays(1, &VAO_pp);
    glDeleteVertexArrays(1, &VBO_pp);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

float elipY(float x) {
    return sqrt(b * b * (1 - x * x / (a * a)));
}

void drawEllipse() {
    float s = 3 * 2 * 2 * a / ippoints; //distance
    float x, y;
    for (int t = 0, t1 = 0; t <= ippoints + 6; t += 6, t1++) {  //total (points/3) points to draw the ellipse!!!
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
    float n = ippoints / (2 * 3); //number of points of one long edge

    float s = 2 * le / n; //distance

    float x;
    for (int t = 0, t2 = 0; t <= ippoints + 6; t += 6, t2++) {
        x = -le + t2 * s;

        sqre[t] = x;
        sqre[t + 1] = se;
        sqre[t + 2] = 0;

        sqre[t + 3] = x;
        sqre[t + 4] = -se;
        sqre[t + 5] = 0;
    }
}

void drawCircle() {
    float s = 2 * r / 25;
    float x, y;
    for (int i = 0, j = 0; j < 25 + 1; i += 6, j++) {
        x = -r + s * j;
        y = sqrt(r * r - x * x);

        circle[i] = x;
        circle[i + 1] = y;
        circle[i + 2] = -0.5;

        circle[i + 3] = x;
        circle[i + 4] = -y;
        circle[i + 5] = -0.5;
    }

}

float drawPath(float x) {
    return  -0.05 * sin(2 * 2 * Pi * x);//curve of the path
}

float getAngle(float x) {
    glm::vec3 con = glm::vec3(0.5, -0.8, 0);
    float at = con.y / (con.x - x);
    return -atan(at);
}

void readObj(char* path, float* r) {
    int i = 0;

    ifstream ifs(path, ios::in);

    if (!ifs) {
        cout << "open file error!" << endl;
        exit(1);
    }
    float t = 0;
    char l;
    for (i = 0; i < 121 * 3; i += 3) {
        ifs >> l;

        ifs >> t;
        r[i] = t / 4;
        ifs >> t;
        r[i + 1] = t / 4;
        ifs >> t;
        r[i + 2] = t / 4;


        //cout << r[i] << " " << r[i+1] << " " << r[i+2] << endl;
    }

    ifs.close();

}

//update the place of particles circular!
void changePPos(int j) {
    //new position
    srand((unsigned int)time(0));
    for (int i = 0; i < pppoints; i++) {

        particles[i].x += (rand() % 1200 / 1200.0 - 0.5) / 10000.0;
        particles[i].y += (rand() % 1200 / 1200.0 - 0.5) / 12000.0;
        particles[i].z = 0.5 - particles[i].y;

        if (particles[i].x > 1.4 || particles[i].x < -0.3 || particles[i].y > 0.09 || particles[i].y < -1.5 || particles[i].z > 1.7 || particles[i].z < -1) {
            particles[i].x = 0;
            particles[i].y = 0;
            particles[i].z = 0;
        }
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