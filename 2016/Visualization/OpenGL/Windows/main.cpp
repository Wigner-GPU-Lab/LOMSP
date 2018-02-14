#include <sstream>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glext.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


auto cross( sf::Vector3f const& u, sf::Vector3f const& v )
{
	return sf::Vector3f( u.y*v.z - u.z*v.y,
		                 u.z*v.x - u.x*v.z,
		                 u.x*v.y - u.y*v.x );
}

auto length( sf::Vector3f const& u ){ return std::sqrt(u.x*u.x+u.y*u.y+u.z*u.z); }

typedef void (*F_GenBuffers) (GLsizei, GLuint*);
typedef void (*F_BindBuffer) (GLenum target, GLuint buffer);
typedef void (*F_BufferData) (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
typedef void (*F_GenVertexArrays) (GLsizei n, GLuint *arrays);
typedef void (*F_BindVertexArray) (GLuint array);
typedef void (*F_VertexAttribPointer) (	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
typedef void (*F_EnableVertexAttribArray) (GLuint index);
typedef GLuint (*F_CreateShader) (GLenum shaderType);

typedef void (*F_ShaderSource) (GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void (*F_CompileShader) (GLuint shader);
typedef GLuint (*F_CreateProgram) (void);
typedef void (*F_AttachShader) (GLuint program, GLuint shader);
typedef void (*F_LinkProgram) (GLuint program);
typedef GLuint (*F_UseProgram) (GLuint program);

typedef void (*F_BindAttribLocation) (GLuint program, GLuint index, const GLchar *name);
typedef void (*F_GetShaderiv) (GLuint shader, GLenum pname, GLint *params);
typedef void (*F_GetShaderInfoLog) (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

typedef GLint (*F_GetUniformLocation) (GLuint program, const char *name);
typedef void (*F_ProgramUniformMatrix4fv) (GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat *v);

typedef void (*F_ActiveTexture) (GLenum texture);

typedef void (*F_GenFramebuffers) (GLsizei n, GLuint *ids);
typedef void (*F_BindFramebuffer) (GLenum target, GLuint framebuffer);
typedef void (*F_FramebufferTexture) (GLenum target, GLenum attachment, GLuint texture, GLint level);

typedef void (*F_ClearBufferfv) (GLenum buffer, GLint drawbuffer, const GLfloat * value);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	static const int w = 640;
	static const int h = 480;
	sf::ContextSettings context(24, 8, 2, 3, 3);
    sf::RenderWindow window(sf::VideoMode(w, h), "OpenGL 2D Function", sf::Style::Default, context);

	//sf::ContextSettings settings = window.getSettings();
	//std::stringstream ss;
	//ss << "OpenGL version:"<< settings.majorVersion << "." << settings.minorVersion << std::endl;
	//MessageBoxA(0, ss.str().c_str(), "", 0);

	auto glGenBuffers      = (F_GenBuffers)     wglGetProcAddress("glGenBuffers");
	auto glBindBuffer      = (F_BindBuffer)     wglGetProcAddress("glBindBuffer");
	auto glBufferData      = (F_BufferData)     wglGetProcAddress("glBufferData");
	auto glGenVertexArrays = (F_GenVertexArrays)wglGetProcAddress("glGenVertexArrays");
	auto glBindVertexArray = (F_BindVertexArray)wglGetProcAddress("glBindVertexArray");
	auto glVertexAttribPointer = (F_VertexAttribPointer)wglGetProcAddress("glVertexAttribPointer");
	auto glEnableVertexAttribArray = (F_EnableVertexAttribArray)wglGetProcAddress("glEnableVertexAttribArray");
	auto glCreateShader = (F_CreateShader)wglGetProcAddress("glCreateShader");
	auto glShaderSource = (F_ShaderSource)wglGetProcAddress("glShaderSource");
	auto glCompileShader = (F_CompileShader)wglGetProcAddress("glCompileShader");
	auto glCreateProgram = (F_CreateProgram)wglGetProcAddress("glCreateProgram");
	auto glAttachShader = (F_AttachShader)wglGetProcAddress("glAttachShader");
	auto glLinkProgram = (F_LinkProgram)wglGetProcAddress("glLinkProgram");
	auto glUseProgram = (F_UseProgram)wglGetProcAddress("glUseProgram");
	auto glBindAttribLocation = (F_BindAttribLocation)wglGetProcAddress("glBindAttribLocation");
	auto glGetShaderiv = (F_GetShaderiv)wglGetProcAddress("glGetShaderiv");
	auto glGetShaderInfoLog = (F_GetShaderInfoLog)wglGetProcAddress("glGetShaderInfoLog");

	auto glGetUniformLocation = (F_GetUniformLocation)wglGetProcAddress("glGetUniformLocation");
	auto glProgramUniformMatrix4fv = (F_ProgramUniformMatrix4fv)wglGetProcAddress("glProgramUniformMatrix4fv");

	auto glActiveTexture = (F_ActiveTexture)wglGetProcAddress("glActiveTexture");
	auto glGenFramebuffers = (F_GenFramebuffers)wglGetProcAddress("glGenFramebuffers");
	auto glBindFramebuffer = (F_BindFramebuffer)wglGetProcAddress("glBindFramebuffer");
	auto glFramebufferTexture = (F_FramebufferTexture)wglGetProcAddress("glFramebufferTexture");

	auto glClearBufferfv = (F_ClearBufferfv)wglGetProcAddress("glClearBufferfv");

	auto f = [](auto x, auto z){ return exp(-x*x*6.-z*z*4.)*_y0(1+(x*x+z*z)*20);/*cos(x*x*20+z*z*20)*/; };

	static const int n = 500;

	std::vector<sf::Vector3f> points(n*n);
	std::vector<sf::Vector3f> normals(n*n);
	std::vector<unsigned int> indices((n-1)*(n-1)*6);
	for(int z=0; z<n; ++z)
	{
		for(int x=0; x<n; ++x)
		{
			auto pz = 1.5*2.0*(z-0.5*n)/ (n*1.0);
			auto px = 1.5*2.0*(x-0.5*n)/ (n*1.0);
			auto h = f(px, pz);
			points[z*n+x].x = px;
			points[z*n+x].y = h;
			points[z*n+x].z = pz;
		}
	}

	for(int z=0; z<n; ++z)
	{
		for(int x=0; x<n; ++x)
		{
			sf::Vector3f dx, dy;
			if( z < n-1 ){ dx = points[(z+1)*n+x] - points[(z+0)*n+x]; }
			else         { dx = points[(z+0)*n+x] - points[(z-1)*n+x]; }
			if( x < n-1 ){ dy = points[z*n+(x+1)] - points[z*n+(x+0)]; }
			else         { dy = points[z*n+(x+0)] - points[z*n+(x-1)]; }
			auto cr = cross(dx, dy);
			normals[z*n+x] = cr / length(cr);
        }
    }

	for(int z=0; z<n-1; ++z)
	{
		for(int x=0; x<n-1; ++x)
		{
			auto ow = 6*(z*(n-1)+x);
			auto or =   (z*(n  )+x);
            indices[ow+0] = or;
			indices[ow+1] = or + 1;
			indices[ow+2] = or + n + 1; 
			indices[ow+3] = or;
			indices[ow+4] = or + n + 1;
			indices[ow+5] = or + n;
        }
    }

	GLuint index_vbo;
	glGenBuffers(1, &index_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(sf::Vector3f), points.data(), GL_STATIC_DRAW);

	GLuint normals_vbo = 0;
	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(sf::Vector3f), normals.data(), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	std::ifstream vs_file("vs_light.glsl");
	std::string vs_code{std::istreambuf_iterator<char>(vs_file), std::istreambuf_iterator<char>()};
	
	std::ifstream fs_file("fs_light.glsl");
	std::string fs_code{std::istreambuf_iterator<char>(fs_file), std::istreambuf_iterator<char>()};

	GLint result = GL_FALSE;
    int logLength;

	auto pvs_code = vs_code.c_str();
	auto pfs_code = fs_code.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &pvs_code, NULL);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);
	{
		std::vector<char> ShaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(vs, logLength, NULL, &ShaderError[0]);
		//MessageBoxA(0, &ShaderError[0], "", 0);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &pfs_code, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logLength);
	{
		std::vector<char> ShaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(fs, logLength, NULL, &ShaderError[0]);
		//MessageBoxA(0, &ShaderError[0], "", 0);
	}

	GLuint shader = glCreateProgram();
	glAttachShader(shader, fs);
	glAttachShader(shader, vs);

	glBindAttribLocation(shader, 0, "vertex_position");
	glBindAttribLocation(shader, 1, "vertex_normal");
	glLinkProgram(shader);

	//set up matrices:
	glm::vec3 camera_position(1, 1, 1);
	glm::vec3 look_at_position(0, 0, 0);
	glm::vec3 up_vct(0, 1, 0);

	glm::mat4x4 world = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4x4 view = glm::lookAt(camera_position, look_at_position, up_vct);
	glm::mat4x4 proj = glm::perspective(glm::radians(90.), 4./3., 0.1, 10.);
	auto vp_matrix = proj * view;

	auto vp_location = glGetUniformLocation(shader, "vp");
	auto w_location = glGetUniformLocation(shader, "w");

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
			if     (event.type == sf::Event::Closed  ){ window.close(); }
			else if(event.type == sf::Event::Resized )
			{
				window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				glViewport(0, 0, event.size.width, event.size.height);
			}
			else if(event.type == sf::Event::KeyPressed )
			{
				if     ( event.key.code == sf::Keyboard::Up    ){ camera_position.z += 0.1; }
				else if( event.key.code == sf::Keyboard::Down  ){ camera_position.z -= 0.1; }
				else if( event.key.code == sf::Keyboard::Left  ){ camera_position.x -= 0.1; }
				else if( event.key.code == sf::Keyboard::Right ){ camera_position.x += 0.1; }

				view = glm::lookAt(camera_position, look_at_position, up_vct);
				proj = glm::perspective(glm::radians(90.), 4./3., 0.1, 10.);
				vp_matrix = proj * view;
			}
        }

        window.clear();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0);
		glUseProgram(shader);
		glProgramUniformMatrix4fv(shader, vp_location, 1, GL_TRUE, &vp_matrix[0][0]);
		glProgramUniformMatrix4fv(shader, w_location, 1, GL_FALSE, &world[0][0]);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
		glDrawElements(GL_TRIANGLES, (n-1)*(n-1)*6, GL_UNSIGNED_INT, (void*)0);
		
        window.display();
    }

    return 0;
}