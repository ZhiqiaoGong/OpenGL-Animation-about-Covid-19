#include"CHCurve.h"

int CStepCount = 4;
float m_hmatrix[4];

CHCurve::CHCurve(glm::vec3 *p) {
    a = p[1];
    b = (p[2] - p[1]) / 2.0f;
    c = (2.0f * p[0] - 5.0f * p[1] + 4.0f * p[2] - p[3]) / 2.0f;
    d = (-p[0] + 3.0f * p[1] - 3.0f * p[2] + p[3]) / 2.0f;
}

float CHCurve::pos(float x){
    /*x = x * 2;
    std::cout << "x:" << x;
    float t = (a + x * b + (x * x) * c + (x * x * x) * d).y;
    if (x > 2 * 15/20) {
        t = t + 0.12;
        std::cout << "!";
        return t;
    }
    if (x > 2 *16/20) {
        t = t + 0.2;
        std::cout << "?";
        return t;
    }*/
    float t = (a + x * b + (x * x) * c + (x * x * x) * d).y;
    //if (t +0.176265<0.01)return -0.096265;
    if (t +0.176264 < 0.01)return -0.117626;
    else if (t+0.30876 < 0.01)return -0.133087;
    else if (t +0.4692955 < 0.01)return -0.156692;
    else if (t +0.66 < 0.01)return -0.17;

    return (a + x * b + (x * x) * c + (x * x * x) * d).y;
}