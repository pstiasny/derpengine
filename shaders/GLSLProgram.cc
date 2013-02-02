#include <fstream>
#include <string>
#include <iterator>
#include <glm/gtc/type_ptr.hpp>

#include "GLSLProgram.h"
#include "GLSLProgramPool.h"

GLSLProgram::GLSLProgram(
		const char *vertex_shader_path, 
		const char *fragment_shader_path)
{
	program_id = 0;

	printf("Compiling shaders: %s, %s\n",
			vertex_shader_path, fragment_shader_path);

	GLuint vertex_shader_id = compileFromFile(
			GL_VERTEX_SHADER, vertex_shader_path);
	if (!vertex_shader_id)
		printf("Shader %s failed to compile\n", vertex_shader_path);

	GLuint fragment_shader_id = compileFromFile(
			GL_FRAGMENT_SHADER, fragment_shader_path);
	if (!fragment_shader_id)
		printf("Shader %s failed to compile\n", fragment_shader_path);

	char log_buff[1024];
	program_id = glCreateProgram();
	glBindAttribLocation(program_id, ATTR_POSITION, "v");
	glBindAttribLocation(program_id, ATTR_NORMAL, "n");
	glBindAttribLocation(program_id, ATTR_UV, "uv");
	
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);
	glGetShaderInfoLog(vertex_shader_id, 1023, NULL, log_buff);
	puts(log_buff);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	uni_mvp = getUniformLocation("MVP");
	uni_m = getUniformLocation("M");
	uni_normal = getUniformLocation("NormalMx");
	uni_cam_pos = getUniformLocation("cam_pos");
	uni_tex_sampler = getUniformLocation("tex_sampler");

	printf("v = #%d\n", glGetAttribLocation(program_id, "v"));
	printf("n = #%d\n", glGetAttribLocation(program_id, "n"));
	printf("uv = #%d\n", glGetAttribLocation(program_id, "uv"));
}

GLint GLSLProgram::getUniformLocation(const char *name)
{
	GLuint r = -1;

	r = glGetUniformLocation(program_id, name);
	if (r == -1)
		printf("Failed to get location of the %s uniform.\n", name);

	return r;
}

GLuint GLSLProgram::compileFromFile(GLenum type, const char *path)
{
	std::ifstream source_stream(path);
	if (!source_stream) {
		printf("Could not open shader %s\n", path);
		return 0;
	}

	std::string source(
			(std::istreambuf_iterator<char>(source_stream)),
			std::istreambuf_iterator<char>());

	return compileShader(type, source.c_str());
}

GLuint GLSLProgram::compileShader(GLenum type, const char* src)
{
	int log_len = 0;
	GLint compile_success = GL_TRUE;
	char *log_buff = NULL;

	GLuint shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &src, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);

	if (compile_success == GL_TRUE)
		return shader_id;

	// If compilation failed, print the error log
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);
	log_buff = (char*)malloc(log_len);
	glGetShaderInfoLog(shader_id, log_len, NULL, log_buff);
	puts(log_buff);
	free(log_buff);
	return 0;
}

GLSLProgram::~GLSLProgram()
{
	glDeleteProgram(program_id);
}

void GLSLProgram::use()
{
	glUseProgram(program_id);
}

void GLSLProgram::setUniformMVP(
		const glm::mat4& model,
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& cam_pos)
{
	if (program_id == 0) return;

	// use() must be called before setting an uniform
	GLint current_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
	assert(current_program == program_id);

	//glm::mat4 mv = view * model;
	//glm::mat4 mvp = projection * mv;
	glm::mat4 mvp = projection * view * model;
	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(model)));

	if (uni_mvp != -1)
		glUniformMatrix4fv(uni_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	if (uni_m != -1)
		glUniformMatrix4fv(uni_m, 1, GL_FALSE, glm::value_ptr(model));
	if (uni_normal != -1)
		glUniformMatrix3fv(uni_normal, 1, GL_FALSE, glm::value_ptr(normal));

	if (uni_cam_pos != -1)
		glUniform3fv(uni_cam_pos, 1, glm::value_ptr(cam_pos));
}

