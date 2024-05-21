#pragma once

#include<glm-master/glm/glm.hpp>
#include<math.h>
#include<iostream>

/*draw catmull-rom curve using 4 points first
* according to the formula, the coefficient are:
*    a = p1;
     b = (p2 - p0)/2;
     c = (2 * p0 - 5 * p1 + 4 * p2 - p3)/2;
     d = (-p0 + 3 * p1 - 3 * p2 + p3)/2;
     p0,p1,p2,p3 are the input points (vectors -> so above are vector calculation!)

     the formula is:
     B(t)=a+b∗t+c∗t^2+d∗t^3
     t from -1 to 1
*/

class CHCurve {
public:
	CHCurve(glm::vec3 *p);

    //return the value(get y by x), x from -1 to 1
	float pos(float x);

    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 d;

};