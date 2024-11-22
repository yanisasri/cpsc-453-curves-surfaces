// yanisa srisa-ard
// 30150196

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>
#include <cmath>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Panel.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;


bool isBezier = true; // bezier=true and b-spline=false
std::vector<glm::vec2> controlPoints;
int selectedPointIndex = -1; // to track which point is selected

bool cameraControlActive = false; // to track if moving camera or not
bool isWireframe = true; // wireframe=true and solid-surface=false

bool isOrtho = false; // toggle between perspective and orthographic cameras
float orthoScale = 1.0f; // scale for orthographic projection

bool isSurface1 = true; // for which surface is being displayed (tensor product surfaces)

// first surface (tensor product surfaces)
glm::vec3 controlPoints1[5][5] = {
    {{-2.0f, 0.0f, -2.0f}, {-1.0f, 0.0f, -2.0f}, {0.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -2.0f}, {2.0f, 0.0f, -2.0f}},
    {{-2.0f, 0.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {2.0f, 0.0f, -1.0f}},
    {{-2.0f, 0.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {2.0f, 0.0f, 0.0f}},
    {{-2.0f, 0.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {2.0f, 0.0f, 1.0f}},
    {{-2.0f, 0.0f, 2.0f}, {-1.0f, 0.0f, 2.0f}, {0.0f, 0.0f, 2.0f}, {1.0f, 0.0f, 2.0f}, {2.0f, 0.0f, 2.0f}}
};

// second surface (non-square) (tensor product surfaces)
glm::vec3 controlPoints2[3][5] = {
	{{-2.0f, 0.0f, -2.0f}, {-1.0f, 0.0f, -2.0f}, {0.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -2.0f}, {2.0f, 0.0f, -2.0f}},
    {{-2.0f, 0.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {2.0f, 0.0f, -1.0f}},
    {{-2.0f, 0.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {2.0f, 0.0f, 0.0f}}
};


// type 1 = bezier & b-spline curves
// type 2 = viewing pipeline
// type 3 = surface of revolution
// type 4 = tensor product surfaces
int type = 1;


class Camera {
public:
    Camera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f)) 
	: target(target), up(0.0f, 1.0f, 0.0f), distance(2.5f), azimuth(0.0f), elevation(glm::radians(30.0f)) {
        updateCameraPosition();
    }

    glm::mat4 getViewMatrix() const {
		return glm::lookAt(position, target, up);
	}

	glm::mat4 getProjectionMatrix(float aspectRatio) const {
		if (isOrtho) {
			float orthoWidth = orthoScale * aspectRatio;
			float orthoHeight = orthoScale;
			return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, nearClip, farClip);
		} else {
			return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		}
	}

	void updateCameraPosition() {
		position.x = distance * cos(elevation) * sin(azimuth);
		position.y = distance * sin(elevation);
		position.z = distance * cos(elevation) * cos(azimuth);
	}

	void resetCamera() {
		distance = 2.5f;
		azimuth = 0.0f;
		elevation = glm::radians(30.0f);
		fov = 45.0f;
        nearClip = 0.1f;
        farClip = 100.0f;
        orthoScale = 1.0f;
		updateCameraPosition();
	}

	void setDistance(float d) { distance = d; updateCameraPosition(); }
    float getDistance() const { return distance; }

    void setAzimuth(float a) { azimuth = a; updateCameraPosition(); }
    float getAzimuth() const { return azimuth; }

	void setElevation(float e) {
        elevation = glm::clamp(e, -glm::half_pi<float>(), glm::half_pi<float>());
        updateCameraPosition();
    }
    float getElevation() const { return elevation; }

	void setFOV(float f) { fov = f; }
    float getFOV() const { return fov; }

    void setNearClip(float near) { nearClip = near; }
    float getNearClip() const { return nearClip; }

    void setFarClip(float far) { farClip = far; }
    float getFarClip() const { return farClip; }

    void setOrthoScale(float scale) { orthoScale = scale; }
    float getOrthoScale() const { return orthoScale; }

private:
	glm::vec3 target;
	glm::vec3 up;
    float distance = 2.5f;
    float azimuth = 0.0f; // horizontal angle
    float elevation = glm::radians(30.0f); // vertical angle
    glm::vec3 position;
	float fov = 45.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;
};


class CurveEditorCallBack : public CallbackInterface {
public:
	CurveEditorCallBack(GLFWwindow* window, Camera& camera) : window(window), camera(camera) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		if (type == 1) {
			// press R key to reset control points
			if (key == GLFW_KEY_R && action == GLFW_PRESS) {
				cout << "Control points have reset." << endl;
				controlPoints.clear(); // reset control points
			}
			
			// when a control point is selected, press BACKSPACE key to delete control point
			if (selectedPointIndex != -1 && key == GLFW_KEY_BACKSPACE) {
                controlPoints.erase(controlPoints.begin() + selectedPointIndex); // remove selected point
                cout << "Deleted control point at index: " << selectedPointIndex << endl; // debugging
                selectedPointIndex = -1; // deselect
            }
		}

		if ((type == 2) || (type == 3) || (type == 4)) {
			// press R key to reset camera
			if (key == GLFW_KEY_R && action == GLFW_PRESS) {
				cout << "Camera has reset." << endl;
				camera.resetCamera();
			}

			// press O key to toggle between orthographic and perspective cameras
			if (key == GLFW_KEY_O && action == GLFW_PRESS) {
                isOrtho = !isOrtho;
                if (isOrtho) {
                    cout << "Switched to Orthographic Camera." << endl;
                } else {
                    cout << "Switched to Perspective Camera." << endl;
                }
            }

			// modify FOV for perspective camera
			// perspective camera: press F key repetitively to increase the FOV or press SHIFT + F key repetitively to decrease the FOV
			if (!isOrtho && key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
                if (mods & GLFW_MOD_SHIFT) {
                    camera.setFOV(camera.getFOV() - 1.0f);
                } else {
                    camera.setFOV(camera.getFOV() + 1.0f);
                }
                cout << "FOV: " << camera.getFOV() << endl;
            }

			// modify near clip
            if (key == GLFW_KEY_N && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
                if (mods & GLFW_MOD_SHIFT) {
                    camera.setNearClip(camera.getNearClip() - 0.1f);
                } else {
                    camera.setNearClip(camera.getNearClip() + 0.1f);
                }
                cout << "Near Clip: " << camera.getNearClip() << endl;
            }

			// modify far clip
			if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
                if (mods & GLFW_MOD_SHIFT) {
                    camera.setFarClip(camera.getFarClip() - 1.0f);
                } else {
                    camera.setFarClip(camera.getFarClip() + 1.0f);
                }
                cout << "Far Clip: " << camera.getFarClip() << endl;
            }

			// modify ortho scale
            if (isOrtho && key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
                if (mods & GLFW_MOD_SHIFT) {
                    camera.setOrthoScale(camera.getOrthoScale() - 0.1f);
                } else {
                    camera.setOrthoScale(camera.getOrthoScale() + 0.1f);
                }
                cout << "Ortho Scale: " << camera.getOrthoScale() << endl;
            }
		}

		if ((type == 1) || (type == 2)) {
			// press UP key to toggle between bezier and b-spline curves
			if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
				isBezier = !isBezier; // change curve type
				if (isBezier) { cout << "Changed to BEZIER CURVE." << endl; }
				else { cout << "Changed to B-SPLINE CURVE." << endl; }
			}
		}

		if ((type == 3) || (type == 4)) {
			// press UP key to toggle between wireframe and solid surface view
			if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                isWireframe = !isWireframe;
                if (isWireframe) {
                    cout << "Wireframe mode enabled." << endl;
                } else {
                    cout << "Solid surface mode enabled." << endl;
                }
            }
		}

		if (type == 4) {
            // change surface
            if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
                isSurface1 = !isSurface1; 
                if (isSurface1) { cout << "Switched to surface 1 (5x5)" << endl; }
				else { cout << "Switched to surface 2 (3x5)" << endl; }
            }
        }

		// press RIGHT arrow key to change to the next exercise/assignment part
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			type += 1;
			if (type == 5) { type = 1; }

			cout << "Changed to: ";

			if (type == 1) { cout << "1. BEZIER & B-SPLINE CURVES" << endl; }
			else if (type == 2) { cout << "2. VIEWING PIPELINE" << endl; }
			else if (type == 3) { cout << "3. SURFACE OF REVOLUTION" << endl;}
			else if (type == 4) { cout << "4. TENSOR PRODUCT SURFACES" << endl; }
        }

		// press LEFT arrow key to change to the previous exercise/assignment part
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			type -= 1;
			if (type == 0) { type = 4; }

			cout << "Changed to: ";

			if (type == 1) { cout << "1. BEZIER & B-SPLINE CURVES" << endl; }
			else if (type == 2) { cout << "2. VIEWING PIPELINE" << endl; }
			else if (type == 3) { cout << "3. SURFACE OF REVOLUTION" << endl;}
			else if (type == 4) { cout << "4. TENSOR PRODUCT SURFACES" << endl; }
        }
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override {
		if (type == 1) {
			// LEFT mouse click to add control points or select an existing control point
			// ** a selected control point can be moved (move cursor & press left click again) or deleted (presss BACKSPACE key)
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				int width, height;
				glfwGetWindowSize(window, &width, &height);
				float normX = (xpos / width) * 2.0f - 1.0f;
				float normY = 1.0f - (ypos / height) * 2.0f;

				// check if a control point is clicked
				for (size_t i = 0; i < controlPoints.size(); i++) {
					if (glm::distance(controlPoints[i], glm::vec2(normX, normY)) < 0.05f) {
						if (selectedPointIndex == i) {
							// deselect the point if it's already selected
							selectedPointIndex = -1;
							// cout << "Deselected control point: (" << normX << ", " << normY << ")" << endl; // debugging
						} else {
							selectedPointIndex = i;
							// cout << "Selected control point: (" << normX << ", " << normY << ")" << endl; // debugging
						}
						return;
					}
				}

				// if no point is selected, add a new control point
				controlPoints.push_back(glm::vec2(normX, normY));
				// cout << "Added control point: (" << normX << ", " << normY << ")" << endl; // debugging
				// cout << "Number of control points: " << controlPoints.size() << endl; // debugging
			}
		} else if ((type == 2) || (type == 3) || (type == 4)) {
			// move/stop moving camera
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				cameraControlActive = !cameraControlActive;
			}
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) override {
		if (type == 1) {
			// move the selected point to follow cursor
			if (selectedPointIndex != -1) {
				int width, height;
				glfwGetWindowSize(window, &width, &height);
				float normX = (xpos / width) * 2.0f - 1.0f;
				float normY = 1.0f - (ypos / height) * 2.0f;
				controlPoints[selectedPointIndex] = glm::vec2(normX, normY); // update position
			}
		} else if (((type == 2) || (type == 3) || (type == 4)) && cameraControlActive) {
			// move camera to follow cursor
			static double lastX = xpos, lastY = ypos;
			double deltaX = xpos - lastX;
			double deltaY = ypos - lastY;

			camera.setAzimuth(camera.getAzimuth() - static_cast<float>(deltaX) * 0.01f);
        	camera.setElevation(camera.getElevation() - static_cast<float>(deltaY) * 0.01f);

			camera.updateCameraPosition();
			lastX = xpos;
			lastY = ypos;
		}
	}

	virtual void scrollCallback(double xoffset, double yoffset) override {
		if ((type == 2) || (type == 3) || (type == 4)) { 
			// scroll to zoom in/out
			camera.setDistance(camera.getDistance() - static_cast<float>(yoffset) * 0.5f);
			camera.setDistance(glm::clamp(camera.getDistance(), 1.0f, 20.0f));
			camera.updateCameraPosition();
		}
	}

private:
	GLFWwindow* window;
	Camera& camera;
};

void drawPoint(const glm::vec2& point, float r, float g, float b, GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    float vertices[] = {
        point.x, point.y, 0.0f, r, g, b
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void drawBezierCurve(const std::vector<glm::vec2>& points, GLuint shaderProgram) {
    if (points.size() < 2) return;

    std::vector<float> vertices;
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        std::vector<glm::vec2> temp = points;
        while (temp.size() > 1) {
            std::vector<glm::vec2> next;
            for (size_t i = 0; i < temp.size() - 1; ++i) {
                next.push_back((1 - t) * temp[i] + t * temp[i + 1]);
            }
            temp = next;
        }
        vertices.push_back(temp[0].x);
        vertices.push_back(temp[0].y);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f); // r
        vertices.push_back(0.0f); // g
        vertices.push_back(0.0f); // b
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 6);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void chaikinAlgorithm(const std::vector<glm::vec2>& controlPoints, std::vector<glm::vec2>& outputPoints, int iterations) {
    outputPoints = controlPoints;

    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<glm::vec2> newPoints;

        for (size_t i = 0; i < outputPoints.size() - 1; ++i) {
            glm::vec2 p0 = outputPoints[i];
            glm::vec2 p1 = outputPoints[i + 1];

            glm::vec2 q = 0.75f * p0 + 0.25f * p1; // Q point
            glm::vec2 r = 0.25f * p0 + 0.75f * p1; // R point

            newPoints.push_back(q);
            newPoints.push_back(r);
        }
        outputPoints = newPoints; // update to new points for the next iteration
    }
}

void drawBSplineCurve(const std::vector<glm::vec2>& points, GLuint shaderProgram) {
    if (points.size() < 2) return;

	int iterations = 5;
    std::vector<glm::vec2> refinedPoints;
    chaikinAlgorithm(points, refinedPoints, iterations);

    std::vector<float> vertices;
    for (const auto& point : refinedPoints) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f); // r
        vertices.push_back(0.0f); // g
        vertices.push_back(1.0f); // b
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 6);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

std::vector<glm::vec3> convertToVec3(const std::vector<glm::vec2>& points) {
    std::vector<glm::vec3> points3D;
    for (const auto& point : points) {
        points3D.push_back(glm::vec3(point.x, point.y, 0.0f)); // z = 0
    }
    return points3D;
}

void drawSurfaceOfRevolution(const std::vector<glm::vec2>& controlPoints, GLuint shaderProgram, bool isWireframe) {
    if (controlPoints.size() < 2) return;

	std::vector<glm::vec3> curvePoints = convertToVec3(controlPoints);

	const int segments = 100; // number of segments around the revolution
    std::vector<glm::vec3> surfacePoints;
	float angleStep = 2.0f * glm::pi<float>() / segments;
	
	for (int i = 0; i < segments; ++i) {
        float angle = i * angleStep;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        for (const auto& point : curvePoints) {
            glm::vec4 rotatedPoint = rotationMatrix * glm::vec4(point, 1.0f);
            surfacePoints.emplace_back(rotatedPoint.x, rotatedPoint.y, rotatedPoint.z);
        }
    }
	
	std::vector<unsigned int> vertices;
    for (int i = 0; i < segments; ++i) {
        int nextSegment = (i + 1) % segments; // wrap around to the first segment (for the last one)

        for (size_t j = 0; j < curvePoints.size() - 1; ++j) {
            int current = i * curvePoints.size() + j;
            int next = nextSegment * curvePoints.size() + j;

            // for two triangles that make up a quad
            vertices.push_back(current);
            vertices.push_back(next);
            vertices.push_back(next + 1);

            vertices.push_back(current);
            vertices.push_back(next + 1);
            vertices.push_back(current + 1);
        }
    }

	GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, surfacePoints.size() * sizeof(glm::vec3), surfacePoints.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices.size() * sizeof(unsigned int), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

	glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

int binomialCoefficient(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;

    int coeff = 1;
    for (int i = 1; i <= k; ++i) {
        coeff = coeff * (n - i + 1) / i;
    }
    return coeff;
}

float bernsteinPolynomial(int i, int n, float t) {
    return binomialCoefficient(n, i) * pow(t, i) * pow(1 - t, n - i);
}

template <size_t Rows, size_t Cols>
void drawTensorProductSurface(glm::vec3 (&controlPoints)[Rows][Cols], int numRows, int numCols, int uSegments, int vSegments, GLuint shaderProgram, bool isWireframe) {
    std::vector<glm::vec3> surfacePoints;
    std::vector<unsigned int> indices;
	glm::vec3 redcolor(1.0f, 0.0f, 0.0f); // red
	std::vector<glm::vec3> surfaceColors; 

    // surface points
    for (int u = 0; u <= uSegments; ++u) {
        float uParam = static_cast<float>(u) / uSegments;
        for (int v = 0; v <= vSegments; ++v) {
            float vParam = static_cast<float>(v) / vSegments;

            glm::vec3 point(0.0f);
            for (int i = 0; i < numRows; ++i) {
                for (int j = 0; j < numCols; ++j) {
                    float bu = bernsteinPolynomial(i, numRows - 1, uParam);
                    float bv = bernsteinPolynomial(j, numCols - 1, vParam);
                    point += bu * bv * controlPoints[i][j];
                }
            }
            surfacePoints.push_back(point);
			surfaceColors.push_back(redcolor);
        }
    }

    for (int u = 0; u < uSegments; ++u) {
        for (int v = 0; v < vSegments; ++v) {
            int current = u * (vSegments + 1) + v;
            int next = current + 1;
            int above = current + (vSegments + 1);

            // two triangles per quad
            indices.push_back(current);
            indices.push_back(above);
            indices.push_back(next);

            indices.push_back(next);
            indices.push_back(above);
            indices.push_back(above + 1);
        }
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, surfacePoints.size() * sizeof(glm::vec3), surfacePoints.data(), GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, surfacePoints.size() * sizeof(glm::vec3) + surfaceColors.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, surfacePoints.size() * sizeof(glm::vec3), surfacePoints.data());
    glBufferSubData(GL_ARRAY_BUFFER, surfacePoints.size() * sizeof(glm::vec3), surfaceColors.size() * sizeof(glm::vec3), surfaceColors.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(surfacePoints.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


int main() {
	Log::debug("Starting main");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwInit();
	
	Window window(800, 800, "CPSC 453: Assignment 3");
	Camera camera;

	auto curve_editor_callback = std::make_shared<CurveEditorCallBack>(window.getGLFWwindow(), camera);
	window.setCallbacks(curve_editor_callback);
	
	ShaderProgram shader_program_default(
		"shaders/test.vert",
		"shaders/test.frag"
	);

	glUseProgram(shader_program_default);

	int width, height;
	glfwGetWindowSize(window.getGLFWwindow(), &width, &height);

	float aspectRatio = static_cast<float>(width) / height;

	glm::mat4 projection, view;
	std::vector<glm::vec3> surfacePoints;

	cout << "Started program: 1. BEZIER & B-SPLINE CURVES" << endl;
	
	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();
		
		if (type == 1) {
			// bezier & b-spline curve
			projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			view = glm::mat4(1.0f);

			glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "projection"), 1, GL_FALSE, &projection[0][0]);

			for (const auto& point : controlPoints) {
				drawPoint(point, 0.0f, 1.0f, 0.0f, shader_program_default); // green points
			}

			if (isBezier) {
				drawBezierCurve(controlPoints, shader_program_default);
			} else {
				drawBSplineCurve(controlPoints, shader_program_default);
			}
		} else if (type == 2) {
			// viewing pipeline
			projection = camera.getProjectionMatrix(aspectRatio);
			view = camera.getViewMatrix();

            glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            
			for (const auto& point : controlPoints) {
				drawPoint(glm::vec2(point.x, point.y), 0.0f, 1.0f, 0.0f, shader_program_default);
			}

            if (isBezier) {
                drawBezierCurve(controlPoints, shader_program_default);
            } else {
                drawBSplineCurve(controlPoints, shader_program_default);
            }
		} else if (type == 3) {
			// surface of revolution
			projection = camera.getProjectionMatrix(aspectRatio);
			view = camera.getViewMatrix();

            glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "view"), 1, GL_FALSE, glm::value_ptr(view));
    		glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);
			
			drawSurfaceOfRevolution(controlPoints, shader_program_default, isWireframe);
		} else if (type == 4) {
			// tensor product surfaces
			projection = camera.getProjectionMatrix(aspectRatio);
			view = camera.getViewMatrix();

            glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shader_program_default, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);

			if (isSurface1) {
				drawTensorProductSurface(controlPoints1, 5, 5, 20, 20, shader_program_default, isWireframe);
			} else {
				drawTensorProductSurface(controlPoints2, 3, 5, 20, 20, shader_program_default, isWireframe);
			}
		}

		window.swapBuffers();
	}

	glfwTerminate();
	return 0;
}