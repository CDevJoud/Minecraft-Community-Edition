#include "GLRenderer.hpp"
#include "glad/gl.h"
#include "SFML/Window/Context.hpp"
#include "SFML/Window/Window.hpp"

#include <print>

static const char* VERTEX = R"(
#version 410
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 vertexColor;

void main()
{
	vertexColor = color;
    gl_Position = vec4(position.xyz, 1.0);
}
)";

static const char* FRAGMENT = R"(
#version 410
layout (location = 0) in vec3 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor.xyz, 1.0f);
}
)";

namespace MCE {
	GLRenderer::GLRenderer(sf::Window& window) : window(window) {
		gladLoadGL(sf::Context::getFunction);

		constexpr float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		constexpr float colors[] = {
			 1.0f,  0.0f, 0.0f,
			 0.5f,  1.0f, 0.0f,
			 0.0f,  0.0f, 1.0f
		};

		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &colorVbo);
		glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		const uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &VERTEX, nullptr);
		glCompileShader(vertex);
		logIfFailed(vertex);

		const uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &FRAGMENT, nullptr);
		glCompileShader(fragment);
		logIfFailed(fragment);

		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		glUseProgram(program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	GLRenderer::~GLRenderer() {
		glDeleteProgram(program);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &colorVbo);
		glDeleteVertexArrays(1, &vao);
	}

	void GLRenderer::renderFrame() {
		const auto viewportSize = sf::Vector2f(window.getSize());
		glViewport(0, 0, viewportSize.x, viewportSize.y);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void GLRenderer::logIfFailed(const uint32_t shader) {
		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success)
			return;

		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::println(stderr, "{}", infoLog);
	}
}
