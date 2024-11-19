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

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Panel.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

bool isBezier = true; // bezier=true and b-spline=false
std::vector<glm::vec2> controlPoints;
int selectedPointIndex = -1; // track which point is selected

// type 1 = bezier & b-spline curves
// type 2 = viewing pipeline
// type 3 = surface of revolution
// type 4 = tensor product surfaces
// type 5 = extension???
int type = 1;


class CurveEditorCallBack : public CallbackInterface {
public:
	CurveEditorCallBack(GLFWwindow* window, Camera& camera) : window(window), camera(camera) {}

	// virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

		// press UP key to toggle between bezier and b-spline curves
		if ((key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) && action == GLFW_PRESS) {
			isBezier = !isBezier; // change curve type
			if (isBezier) { cout << "Changed to: BEZIER CURVE." << endl; }
			else { cout << "Changed to: B-SPLINE CURVE." << endl; }
		}

		// if (type == 2) {
		// 	// press R key to reset camera
		// 	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		// 		// reset camera HERE
		// 	}
		// }

		// press RIGHT arrow key to change to the next exercise/assignment part
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			type += 1;
			if (type == 6) { type = 1; }

			cout << "Changed to: ";

			if (type == 1) { cout << "1. BEZIER & B-SPLINE CURVES" << endl; }
			else if (type == 2) { cout << "2. VIEWING PIPELINE" << endl; }
			else if (type == 3) { cout << "3. SURFACE OF REVOLUTION" << endl;}
			else if (type == 4) { cout << "4. TENSOR PRODUCT SURFACES" << endl; }
			else if (type == 5) { cout << "EXTENSION???" << endl; }
        }

		// press LEFT arrow key to change to the previous exercise/assignment part
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			type -= 1;
			if (type == 0) { type = 5; }

			cout << "Changed to: ";

			if (type == 1) { cout << "1. BEZIER & B-SPLINE CURVES" << endl; }
			else if (type == 2) { cout << "2. VIEWING PIPELINE" << endl; }
			else if (type == 3) { cout << "3. SURFACE OF REVOLUTION" << endl;}
			else if (type == 4) { cout << "4. TENSOR PRODUCT SURFACES" << endl; }
			else if (type == 5) { cout << "EXTENSION???" << endl; }
        }
	}

	// virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	virtual void mouseButtonCallback(int button, int action, int mods) override {
		// cout << "Mouse button pressed: " << button << ", action: " << action << endl; // debugging
		
		// if (action == GLFW_PRESS) {
		// 	cout << "Mouse button action detected." << endl; // debugging
		// }

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float normX = (xpos / width) * 2.0f - 1.0f;
		float normY = 1.0f - (ypos / height) * 2.0f;

		if (type == 1) {
			// LEFT mouse click to add control points or select an existing control point
			// ** a selected control point can be moved (move cursor & press left click again) or deleted (presss BACKSPACE key)
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				// check if a control point is clicked
				for (size_t i = 0; i < controlPoints.size(); i++) {
					if (glm::distance(controlPoints[i], glm::vec2(normX, normY)) < 0.05f) {
						if (selectedPointIndex == i) {
							// deselect the point if it's already selected
							selectedPointIndex = -1;
							cout << "Deselected control point: (" << normX << ", " << normY << ")" << endl; // debugging
						} else {
							selectedPointIndex = i;
							cout << "Selected control point: (" << normX << ", " << normY << ")" << endl; // debugging
						}
						return;
					}
				}

				// if no point is selected, add a new control point
				controlPoints.push_back(glm::vec2(normX, normY));
				cout << "Added control point: (" << normX << ", " << normY << ")" << endl; // debugging
				cout << "Number of control points: " << controlPoints.size() << endl; // debugging
			}
		} else if (type == 2) {
			camera.processMouseMovement(xpos, ypos);
		}
	}

	// virtual void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	virtual void cursorPosCallback(double xpos, double ypos) override {
		if (selectedPointIndex != -1) {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			float normX = (xpos / width) * 2.0f - 1.0f;
			float normY = 1.0f - (ypos / height) * 2.0f;
			controlPoints[selectedPointIndex] = glm::vec2(normX, normY); // update position
		}
	}

	virtual void scrollCallback(double xoffset, double yoffset) override {
		if (type == 2) { camera.processScroll(yoffset); }
	}

	// virtual void keyCallback(int key, int scancode, int action, int mods) override {
	// 	Log::info("KeyCallback: key={}, action={}", key, action);
	// }

	// virtual void mouseButtonCallback(int button, int action, int mods) override {
	// 	Log::info("MouseButtonCallback: button={}, action={}", button, action);
	// }

	// virtual void cursorPosCallback(double xpos, double ypos) override {
	// 	Log::info("CursorPosCallback: xpos={}, ypos={}", xpos, ypos);
	// }

	// virtual void scrollCallback(double xoffset, double yoffset) override {
	// 	Log::info("ScrollCallback: xoffset={}, yoffset={}", xoffset, yoffset);
	// }

	// virtual void windowSizeCallback(int width, int height) override {
	// 	Log::info("WindowSizeCallback: width={}, height={}", width, height);
	// 	CallbackInterface::windowSizeCallback(width, height); // Important, calls glViewport(0, 0, width, height);
	// }

private:
	GLFWwindow* window;
	Camera& camera;
};


void drawPoint(const glm::vec2& point, float r, float g, float b, GLuint shaderProgram) {
    glUseProgram(shaderProgram); // ensure the shader program is active

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
    // cout << "Drawing Bezier Curve..." << endl; // debugging

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
        vertices.push_back(1.0f); // red
        vertices.push_back(0.0f); // green
        vertices.push_back(0.0f); // blue
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

            glm::vec2 q = 0.75f * p0 + 0.25f * p1;
            glm::vec2 r = 0.25f * p0 + 0.75f * p1;

            newPoints.push_back(q);
            newPoints.push_back(r);
        }
        outputPoints = newPoints;
    }
}

void drawBSplineCurve(const std::vector<glm::vec2>& points, GLuint shaderProgram) {
    // cout << "Drawing B-Spline Curve..." << endl; // debugging
	
	if (points.size() < 2) return;

	int iterations = 5;
    std::vector<glm::vec2> refinedPoints;
    chaikinAlgorithm(points, refinedPoints, iterations);

    std::vector<float> vertices;
    for (const auto& point : refinedPoints) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f); // red
        vertices.push_back(0.0f); // green
        vertices.push_back(1.0f); // blue
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


int main() {
	Log::debug("Starting main");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// WINDOW
	glfwInit();
	
	 // debugging
	if (!glfwInit()) {
    	cout << "Failed to initialize GLFW" << endl;
    	return -1;
	}

	Window window(800, 800, "CPSC 453: Assignment 3");
	Camera camera;

	// CALLBACKS
	// auto curve_editor_callback = std::make_shared<CurveEditorCallBack>(window);
	auto curve_editor_callback = std::make_shared<CurveEditorCallBack>(window.getGLFWwindow(), camera);
	
	//Set callback to window
	window.setCallbacks(curve_editor_callback);
	
	ShaderProgram shader_program_default(
		"shaders/test.vert",
		"shaders/test.frag"
	);

	shader_program_default.use();
	std::cout << "Using shader program ID: " << shader_program_default.getID() << std::endl;

	glViewport(0, 0, 800, 800);

	cout << "1. BEZIER & B-SPLINE CURVES" << endl;

	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();
		// cout << "Polling events..." << endl; // debugging
		// cout << "Number of control points: " << controlPoints.size() << endl; // debugging

		if (type == 1) {
			// bezier & b-spline curve
			// glUseProgram(shader_program_default.getID());
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

			glm::mat4 view = glm::lookAt(camera.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader_program_default.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader_program_default.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		} else if (type == 3) {
			// implement surface of revolution
		} else if (type == 4) {
			// implement tensor product surfaces
		}

		window.swapBuffers();

		
	}

	glfwTerminate();
	return 0;
}
