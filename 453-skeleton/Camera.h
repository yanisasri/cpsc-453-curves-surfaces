#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera() : distance(5.0f), azimuth(0.0f), elevation(0.0f) {
        updateCameraPosition();
    }

    void processMouseMovement(double xpos, double ypos);

    void processScroll(double yoffset);

    glm::vec3 getPosition() { return position; }

    void updateCameraPosition();

protected:
    float distance;
    float azimuth; // horizontal angle
    float elevation; // vertical angle
    glm::vec3 position;
    bool firstMouse = true;
    double lastX, lastY;

private:

};