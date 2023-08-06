
#include <iostream>
#include <fstream>
#include "GLow.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLow::GLow() {
    shader_dir = std::filesystem::path("glsl");
}



int GLow::compileShaders()
{
    std::cout << "Compiling shaders in: " << shader_dir.generic_string() << std::endl;
    for (auto const& file : std::filesystem::directory_iterator{ shader_dir }) {
        shaders_t id = file_name_to_shader_id(file.path());
        if (id != ERROR)
        {
            std::cout << "Compiling shader: " << file.path() << std::endl;

            int shader_bytes = std::filesystem::file_size(file.path());
            std::ifstream glsl_file;
            std::vector<char> shader_vec;
            glsl_file.open(file.path());

            char* shader_str;
            shader_str = new char[shader_bytes + 1];

            glsl_file.read(shader_str, shader_bytes);
            //needed as iftsream will convert windows to linux line endings, need to reterminate in correct spot
            int actual_bytes = glsl_file.gcount();
            shader_str[actual_bytes] = NULL;

            std::cout << shader_str << std::endl;
            
            unsigned int glid;
            glid = glCreateShader(shader_id_to_gl_type(id));
            glShaderSource(glid, 1, &shader_str, NULL);
            glCompileShader(glid);
            
            int  success;
            char infoLog[512];
            glGetShaderiv(glid, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(glid, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            else
            {
                shaders.insert(std::pair(id, glid));
            }
            std::cout << std::endl;
        }

    }
    glCheckError();

    //now link all the programs we'll need
    red_prog = glCreateProgram();
    glAttachShader(red_prog, shaders[default_vtx]);
    glAttachShader(red_prog, shaders[default_frag]);
    glLinkProgram(red_prog);
    glCheckError();

    green_prog = glCreateProgram();
    glAttachShader(green_prog, shaders[default_vtx]);
    glAttachShader(green_prog, shaders[green_frag]);
    glLinkProgram(green_prog);

    colour_prog = glCreateProgram();
    glAttachShader(colour_prog, shaders[moving_vtx]);
    glAttachShader(colour_prog, shaders[colour_frag]);
    glLinkProgram(colour_prog);
    time_uniform_loc = glGetUniformLocation(colour_prog, "time");

    glCheckError();

    return 0;
}

int GLow::loadTextures()
{   
    glGenTextures(1, &froge_tex);
    glBindTexture(GL_TEXTURE_2D, froge_tex);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glCheckError();

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/good-froge.png", &width, &height, &nrChannels, 3);
    if (data)
    {
        //froge is height: 1891, width 1207 crash
        //manually setting height: 1890 and width 1206 gets something correct looking
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    stbi_image_free(data);

    return 0;
}



int GLow::setRenderProgram()
{
    
    return 0;
}

GLow::shaders_t GLow::file_name_to_shader_id(std::filesystem::path p)
{   
    shaders_t s = ERROR;
    if (p == "glsl/default_vtx.vert")
    {
        s = default_vtx;
    }
    else if (p == "glsl/default.frag") {
        s = default_frag;
    }
    else if (p == "glsl/default_2.frag") {
        s = green_frag;
    }
    else if (p == "glsl/colour_interp.frag") {
        s = colour_frag;
    }
    else if (p == "glsl/sin_translation.vert") {
        s = moving_vtx;
    }
    return s;
}

GLenum GLow::shader_id_to_gl_type(shaders_t s)
{
    if (s == default_vtx || s == moving_vtx)
    {
        return GL_VERTEX_SHADER;
    }
    else if (s == default_frag || s == green_frag || s == colour_frag)
    {
        return GL_FRAGMENT_SHADER;
    }
    return 0;
}


int GLow::render()
{
    float time = glfwGetTime();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    int dwords_per_vert = 8; //3 pos + 3 colour + 2 tex
    float vertices[] = {
         -0.3, -0.5, 0.0, 1.0f, 0.0f, 0.0f, 0.0, 0.0,
            0,  0.5, 0.0, 0.0f, 1.0f, 0.0f, 1.0, 0.0,
         -0.6, -0.5, 0.0, 0.0f, 0.0f, 1.0f, 0.0, 1.0,

         0.3, 0.5, 0.0, 1.0f, 0.0f, 0.0f, 0.0, 0.0,
         0,  -0.5, 0.0, 0.0f, 1.0f, 0.0f, 1.0, 0.0,
         0.6, 0.5, 0.0, 0.0f, 0.0f, 0.7f, 0.0, 1.0
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        3, 4, 5    // second triangle
    };

    unsigned int indices2[] = {  // note that we start from 0!
        0,3,4   // first triangle
    };

    glCheckError();
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glCheckError();

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, dwords_per_vert * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, dwords_per_vert * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); //accessed using layout=1

    //tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, dwords_per_vert * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); //accessed using layout=1
    glCheckError();

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    unsigned int EBO2;
    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    glCheckError();

    //final setup of shaders
    glBindVertexArray(VAO);




    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glUseProgram(green_prog);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glCheckError();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glUseProgram(colour_prog);
    glUniform1f(time_uniform_loc, time);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    return 0;
}
