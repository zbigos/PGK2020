#include <fstream>
#include <iostream>

using std::string;

std::vector<string> ReadShader(string shadername) {
    std::vector<string> shader;
    std::fstream shaderfile;
    shaderfile.open(shadername, std::ios::in);
    if (!shaderfile) {
        std::cout << "couldn't read " << shadername << std::endl;
        std::cout << "terminating";
        exit(1);
    } else {
        string line;
        while(std::getline(shaderfile, line)) {
            line += '\n';
            shader.push_back(line);
        }
    }

    shaderfile.close();
    return shader;
}

void CompileShader(GLuint shader_handle, std::vector<string> shader_source, string shadername) {
    std::cout << "-----------------------------" << std::endl;
    char compiler_errors[1024];
    int coompilation_status;

    int *sourcelens = (int *)malloc(sizeof(int) * shader_source.size());
    const char** sources = (const char **)malloc(sizeof(char *) * shader_source.size());

    for(int i = 0; i < shader_source.size(); i++) {
        sourcelens[i] = shader_source[i].length();
        sources[i] = shader_source[i].c_str();
    }


    glShaderSource(shader_handle, shader_source.size(), sources, sourcelens);
    glCompileShader(shader_handle);
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &coompilation_status);
    glGetShaderInfoLog(shader_handle, sizeof(compiler_errors), 0, compiler_errors);
    AGLErrors("failed when compiling");
    
    for(int i = 0; i < shader_source.size(); i++)
        std::cout << shader_source[i];
    std::cout << compiler_errors << std::endl;

    if (coompilation_status != 1) {
        std::cout << "compilation of shader " << shadername << " failed" << std::endl;
        std::cout << compiler_errors << std::endl;
    }
}

std::pair<GLuint, GLuint> BuildPipeline() {
    printf("starting to build the shader pipeline!\n");
    
    std::cout << "compiling vertex shader" << std::endl;
    std::vector<string> VertexShaderSource = ReadShader("shaders/VertexShader.txt");
    GLuint VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(VertexShaderHandle, VertexShaderSource, "shaders/VertexShader.txt");

    std::cout << "finished compiling vertex shader" << std::endl << "compiling tesselation control shader" << std::endl;
    std::vector<string> TesselationControlShaderSource = ReadShader("shaders/TesselationControlShader.txt");
    GLuint TesselationControlShaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
    CompileShader(TesselationControlShaderHandle, TesselationControlShaderSource, "shaders/TesselationControlShader.txt");

    std::cout << "finished compiling tesselation control shader" << std::endl << "compiling tesselation evaluation shader" << std::endl;
    std::vector<string> TesselationEvaluationShaderSource = ReadShader("shaders/TesselationEvaluationShader.txt");
    GLuint TesselationEvaluationShaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
    CompileShader(TesselationEvaluationShaderHandle, TesselationEvaluationShaderSource, "shaders/TesselationEvaluationShader.txt");

    std::cout << "finished compiling evaluation control shader" << std::endl << "compiling geometry shader" << std::endl;
    std::vector<string> GeometryShaderSource = ReadShader("shaders/GeometryShader.txt");
    GLuint GeometryShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
    CompileShader(GeometryShaderHandle, GeometryShaderSource, "shaders/GeometryShader.txt");

    std::cout << "finished compiling geometry shader" << std::endl << "compiling fragment shader" << std::endl;
    std::vector<string> FragmentShaderSource = ReadShader("shaders/FragmentShader.txt");
    GLuint FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(FragmentShaderHandle, FragmentShaderSource, "shaders/FragmentShader.txt");
    std::cout << "finished compiling fragment shader" << std::endl;

    GLuint ShaderHandle = glCreateProgram();
    glAttachShader(ShaderHandle, VertexShaderHandle);
    glAttachShader(ShaderHandle, TesselationControlShaderHandle);
    glAttachShader(ShaderHandle, TesselationEvaluationShaderHandle);
    //glAttachShader(ShaderHandle, GeometryShaderHandle);
    glAttachShader(ShaderHandle, FragmentShaderHandle);

    /* no fucking clue as to what this is*/
    GLuint PositionSlot = 0;
    glBindAttribLocation(ShaderHandle, PositionSlot, "Position");

    std::cout << "linking shader" << std::endl;
    glLinkProgram(ShaderHandle);

    char linker_errors[256];
    int linking_status;
    int t;
    glGetProgramiv(ShaderHandle, GL_LINK_STATUS, &linking_status);
    glGetProgramInfoLog(ShaderHandle, sizeof(linker_errors), &t, linker_errors);

    std::cout << linking_status << std::endl;
    std::cout << linker_errors << std::endl;


    if(linking_status != 1) {
        std::cout << "linker failed with" << std::endl;
        std::cout << linker_errors << std::endl;
        std::cout << "terminating";
        exit(1);
    }

    glUseProgram(ShaderHandle);
    AGLErrors("failed when loading program");

    std::cout << "compilation finished" << std::endl;

    return std::make_pair<int, int>(ShaderHandle, PositionSlot);
}