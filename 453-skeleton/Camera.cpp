#include "Camera.h"

void Camera::processMouseMovement(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    azimuth += xoffset;
    elevation += yoffset;

    // clamp elevation angle
    if (elevation > 89.0f) elevation = 89.0f;
    if (elevation < -89.0f) elevation = -89.0f;

    updateCameraPosition();
}

void Camera::processScroll(double yoffset) {
    distance -= (float)yoffset;
    if (distance < 1.0f) distance = 1.0f; // prevent camera from getting too close
    updateCameraPosition();
}

void Camera::updateCameraPosition() {
    position.x = distance * cos(glm::radians(elevation)) * sin(glm::radians(azimuth));
    position.y = distance * sin(glm::radians(elevation));
    position.z = distance * cos(glm::radians(elevation)) * cos(glm::radians(azimuth));
}