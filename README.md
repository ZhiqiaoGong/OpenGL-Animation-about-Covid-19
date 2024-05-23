# OpenGL-based Animation for the Prevention of COVID-19

The project leverages computer animation to visually explain the transmission of the virus and preventive measures. It was developed using C++, OpenGL, glad, and glfw libraries, involving the specification of vertex data, composition of vertex and fragment shaders in GLSL, and the creation and application of textures independently.

**Animation Techniques Employed**:
- **Keyframe Animation**: Shape interpolation was utilized through defining ellipse and rectangle vertex arrays to simulate the movements of a mouth and a mask, controlled by variations in the interpolation variable 't'.
- **Curve Animation**: The path of virus transmission was visualized using a catmull-rom curve to depict the virus being expelled, airborne transmission, and eventual inhalation by another individual.
- **Gradient Animation**: A transition effect between two images representing a sneeze was achieved by using a coordinate grid method for continuous interpolation of grids.
- **Character Animation**: The project featured two characters who approach each other and simulate an elbow touch as a form of greeting, animated using path animation and forward kinematics for the arms.
- **Particle Animation**: Crowd simulation was achieved by defining particles with properties like position and velocity, with these properties being dynamically updated to represent crowd movement.

**Educational Value and Recognition**: The project was not only educational, illustrating health safety measures, but also received a perfect score and top ranking in the computer animation course, and was showcased to the entire class by the professor due to its exemplary execution.

**Learning Outcome**: The completion of this project facilitated a deeper understanding of complex topics such as the graphics pipeline and foundational concepts in OpenGL animation.

This project demonstrates the integration of technical skills and creative storytelling to convey critical health information through advanced computer graphics techniques.

Details can be viewed here: https://1141283192.wixsite.com/zqgong/covid
