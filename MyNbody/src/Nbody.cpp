#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


#include <utils/shader.h>
#include <utils/camera.h>
#include <utils/model.h>

#include <chrono>
#include <iostream>
#include <math.h>
#include <random>

#define CellSize (1.25f)
#define WORKGROUP_SIZE 256

static float dy_factor = 1.0f;
static float frame_time = 0.0f;
static glm::vec2 m_pos = glm::vec2(0.f);
static glm::vec2 m_delta = glm::vec2(0.f);
static bool m_pressed = false;
static int particle_num = WORKGROUP_SIZE * 200;
static bool paused = false;
const float PI = 3.14159265358979323846;

std::uniform_real_distribution<double> distribution(0.0, 1.0);
std::default_random_engine generator;
mt19937 rng;

#define Render_rs 0

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 40.0f, 65.0f));

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int TextureFromFile(const string path, bool gamma = 0);
//void renderQuad();


// nbody particle data buffer & VBO creation 

struct Cube {
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    void renderCube()
    {
        // initialize (if necessary)
        if (cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    void destroyCube() {
        glDeleteBuffers(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
    }
    Cube() {};
};


class nBody_particles {
public:
    nBody_particles(){
        particle_pos.resize(particle_num);
        particle_vel.resize(particle_num);
        initialize();
    }
    void initialize() {
        populate_buffers();

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &_particle_pos_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _particle_pos_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, particle_pos.size() * sizeof(glm::vec4), &particle_pos[0], GL_DYNAMIC_COPY);//SSBO PACK ALL VEC3 TO VEC4 ???? WTF, FORCED ME TO USE VEC4

        glGenBuffers(1, &_particle_vel_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _particle_vel_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, particle_vel.size() * sizeof(glm::vec4), &particle_vel[0], GL_DYNAMIC_COPY); //SSBO PACK ALL VEC3 TO VEC4 ???? WTF
    }

    void draw() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _particle_pos_buffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glBindBuffer(GL_ARRAY_BUFFER, _particle_vel_buffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glDrawArrays(GL_POINTS, 0, particle_num);
    }
public:
    GLuint _particle_pos_buffer, _particle_vel_buffer, VAO;
    struct galaxy {
        glm::vec3 g_center;
        glm::vec3 g_velocity;
        int num_stars;
        galaxy():g_center(glm::vec3(0)), g_velocity(glm::vec3(0)), num_stars(particle_num) {}
        galaxy(glm::vec3 _center, glm::vec3 _velocity, int _num_stars) : 
            g_center(_center), g_velocity(_velocity), num_stars(_num_stars)
        {}  
        glm::vec4 get_rnd_pos(const int i) {
            float rng1 = distribution(rng) * 2.f * PI;
            float rng2 = distribution(rng);
            float rng3 = distribution(rng);
            float rng4 = distribution(rng);
            rng4 = pow((rng4 + 1.0f) / 2.0f,1.0f);// 0.5 ~ 1.0
            glm::vec4 rnd_pos =  glm::vec4(cos(rng1) * rng2 * 20.0f,rng3 * 1.0f, sin(rng1) * rng2 * 20.0f, rng4);
            rnd_pos += glm::vec4(g_center, 0.f);   //galaxy 1 center
            if (i == 0 || i == particle_num/2) {
                rnd_pos.x = g_center.x;
                rnd_pos.y = g_center.y;
                rnd_pos.z = g_center.z;
                rnd_pos.w = 1000.0f; };
            return rnd_pos;
        }
        glm::vec4 get_rnd_vel( const glm::vec4& _pos, const int i) {
            float r =1 - distribution(rng)/50.0f;
            glm::vec4 tang_vel = glm::vec4(glm::normalize(glm::cross(glm::vec3(0, 1, 0), glm::vec3(_pos) - g_center)), 0.0f);
            float dis = glm::distance(glm::vec3(_pos), g_center);
            glm::vec4 rnd_vel = tang_vel * (dis) * 60.f;

            rnd_vel += glm::vec4(g_velocity, 0.0f);   //galaxy 1 center
            if (i == 0 || i == particle_num / 2) rnd_vel = glm::vec4(0);
            rnd_vel.w = log(_pos.w * 2.0f * 1.0);
            
            return rnd_vel;
            //return glm::vec3(0,0,100);
        }
    };
    
private:

    vector<glm::vec4> particle_pos;
    vector<glm::vec4> particle_vel;
    galaxy g1 = galaxy(glm::vec3(40, 40, 20), glm::vec3(0, 0, 0), particle_num / 2);
    galaxy g2 = galaxy(-glm::vec3(40, 40, 20), -glm::vec3(0, 0, 0), particle_num / 2);



    void populate_buffers() {
        for (int i = 0; i < particle_num; ++i) {
            if (i <= particle_num / 2) {
                particle_pos[i] = g1.get_rnd_pos(i);
                particle_vel[i] = g1.get_rnd_vel(particle_pos[i], i);
            }
            else {
                particle_pos[i] = g2.get_rnd_pos(i);
                particle_vel[i] = g2.get_rnd_vel(particle_pos[i], i);
            }
        }
    }
};

struct GframeBuffer {
    GLuint gBuffer, color_tex, brightness_tex, rboDepth;
    void initialize_fbo() {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glGenTextures(1, &color_tex);
        glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
        glGenTextures(1, &brightness_tex);
        glBindTexture(GL_TEXTURE_2D, brightness_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightness_tex, 0);
        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    GframeBuffer() { initialize_fbo(); }
};

//blur fbos
struct BlurFrameBuffers {
    GLuint pingpongFBO[2], pingpongColorTex[2];
    void initialize_fbos(){
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorTex);
        for (int i = 0; i < 2; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorTex[i], 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    BlurFrameBuffers() { initialize_fbos(); }
};

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
struct Quad {
    bool render_instanced;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad(GLuint instanced_buffer_pos_handle = 0, GLuint instanced_buffer_vel_handle = 0)
    {
        if (quadVAO == 0)
        {
            float size_fac = 0.05;
            if (!render_instanced) {
                size_fac = 1.0f;
            }
            float resolution_fac = 1.0;
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f * size_fac,  1.0f * size_fac, 0.0f * size_fac, 0.0f * resolution_fac, 1.0f * resolution_fac,
                -1.0f * size_fac, -1.0f * size_fac, 0.0f * size_fac, 0.0f * resolution_fac, 0.0f * resolution_fac,
                 1.0f * size_fac,  1.0f * size_fac, 0.0f * size_fac, 1.0f * resolution_fac, 1.0f * resolution_fac,
                 1.0f * size_fac, -1.0f * size_fac, 0.0f * size_fac, 1.0f * resolution_fac, 0.0f * resolution_fac,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            if (render_instanced) {
                glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, instanced_buffer_pos_handle);
                glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glEnableVertexAttribArray(3);
                glBindBuffer(GL_ARRAY_BUFFER, instanced_buffer_vel_handle);
                glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glVertexAttribDivisor(2, 1);
                glVertexAttribDivisor(3, 1);
            }
        }
        glBindVertexArray(quadVAO);
        if (render_instanced)
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_num);
        else
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    void destroyQuad() {
        glDeleteBuffers(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
    Quad(bool _instanced = false) : render_instanced(_instanced ) {};
};



void createFluidTexture(GLuint& _tex) {
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT,
        NULL);

}

void bindTexture_image(GLuint _tex, int slot) {
    glBindImageTexture(slot, _tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}



int main()
{
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Y R WE STILL HERE ???", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------

    Shader shaderNbody("../res/nBody.vs", "../res/nBody.fs");
    Shader shaderBloom("../res/bloom.vs", "../res/bloom.fs");
    Shader shaderBlur("../res/blur.vs", "../res/blur.fs");

    Shader shaderVelocityUpdate("../res/velocityUpdate.comp");
    Shader shaderPositionUpdate("../res/positionUpdate.comp");


    //------------compute shader work group-------------

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
    printf("max global (total) work group counts x:%i y:%i z:%i\n",
        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
        work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    //------------compute shader work group-------------

    // initialize n body particles
    // -----------

    nBody_particles myNbody;
    Quad sprite_quad(true);
    Quad fullscreen_quad;
    Cube myCube;
    GframeBuffer my_fbo;
    BlurFrameBuffers blur_fbo;
   

    // render loop
    // -----------

    auto _pre_timer = std::chrono::steady_clock::now();
    glPointSize(1.5);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
   

        glViewport(0, 0, SCR_WIDTH * dy_factor, SCR_HEIGHT * dy_factor);
        auto _cur_timer = std::chrono::steady_clock::now();
        auto _elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(_cur_timer - _pre_timer).count();
        std::cout << 1000.0f / _elapsed_time << std::endl;
        _pre_timer = _cur_timer;

        // per-frame time logic
        // --------------------
        frame_time = glfwGetTime();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << "GLFW time : " << deltaTime << std::endl;



        // compute particles
        // -----
        shaderVelocityUpdate.use();
        shaderVelocityUpdate.setInt("particle_count", particle_num);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, myNbody._particle_pos_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, myNbody._particle_vel_buffer);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(particle_num / (WORKGROUP_SIZE), 1, 1);

        shaderPositionUpdate.use();
        shaderPositionUpdate.setInt("particle_count", particle_num);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, myNbody._particle_pos_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, myNbody._particle_vel_buffer);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(particle_num / (WORKGROUP_SIZE), 1, 1);



        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        //cout << "x :" << camera.Front.x << "y: " << camera.Front.y << "z: " << camera.Front.z << endl;

     

        // ----visulize here---
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        
        glBindFramebuffer(GL_FRAMEBUFFER, my_fbo.gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shaderNbody.use();
        shaderNbody.setMat4("projection", projection);
        shaderNbody.setMat4("view", view);
        shaderNbody.setVec3("viewPos", camera.Position);
        glm::mat3 camera_axes = glm::mat3({ camera.Right.x, camera.Right.y, camera.Right.z,
           camera.Up.x, camera.Up.y, camera.Up.z,
            camera.Front.x, camera.Front.y, camera.Front.z });
        shaderNbody.setMat3("camera_axes", camera_axes);
        model = glm::mat4(1.0f);
        shaderNbody.setMat4("model", model);
        sprite_quad.renderQuad(myNbody._particle_pos_buffer, myNbody._particle_vel_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bloom blur pass

        bool horizontal = true, first_iter = true;
        unsigned int amount = 10;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo.pingpongFBO[horizontal]);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            shaderBlur.setInt("horizontal", horizontal);
            shaderBlur.setInt("image", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iter ? my_fbo.brightness_tex : blur_fbo.pingpongColorTex[!horizontal]);
            fullscreen_quad.renderQuad();
            horizontal = !horizontal;
            if (first_iter) first_iter = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // bloom pass
        shaderBloom.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, my_fbo.color_tex);
        shaderBloom.setInt("color_tex", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blur_fbo.pingpongColorTex[!horizontal]);
        shaderBloom.setInt("blur_tex", 1);

        fullscreen_quad.renderQuad();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

 
    glfwTerminate();
    //_CrtDumpMemoryLeaks();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        paused = !paused;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        m_pressed = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        m_pressed = false;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int TextureFromFile(const string path, bool gamma )
{

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}