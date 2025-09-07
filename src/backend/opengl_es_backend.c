#include <prm.h>
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ----------------------
// Global stuff
// ----------------------
static GLuint fbo = 0;
static GLuint tex = 0;
static GLuint shader_program = 0;
static GLuint vbo = 0;

static int fb_width = 0;
static int fb_height = 0;

// logical pixel buffer (CPU side)
static uint32_t *logical_fb = NULL;

// current resolution + scale
static prm_window_resolution_t cur_res;

// remember how we’re running: GPU or CPU
static prm_usage_type current_usage;

// shaders
static const char *vertex_shader_src =
"attribute vec2 a_pos;\n"
"attribute vec2 a_tex;\n"
"varying vec2 v_texcoord;\n"
"void main() {\n"
"    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
"    v_texcoord = a_tex;\n"
"}\n";

static const char *fragment_shader_src =
"precision mediump float;\n"
"uniform sampler2D u_pixels;\n"
"varying vec2 v_texcoord;\n"
"void main() {\n"
"    gl_FragColor = texture2D(u_pixels, v_texcoord);\n"
"}\n";

// shader helpers
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint status;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buf[512];
        glGetShaderInfoLog(s, 512, NULL, buf);
        const char *t = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        printf("%s SHADER COMPILE ERROR: %s\n", t, buf);
    }
    return s;
}

static GLuint create_shader_program() {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);

    // bind attribs before linking
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");

    glLinkProgram(prog);

    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (!status) {
        char buf[512];
        glGetProgramInfoLog(prog, 512, NULL, buf);
        printf("SHADER LINK ERROR: %s\n", buf);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// init PRM backend
int prm_opengl_es_init(prm_window_config_t *config) {
    current_usage = config->usage_type;

    // set resolution
    cur_res = PRM_PRESET_RES_RETRO;
    cur_res.width  = config->width  ? config->width  : cur_res.width;
    cur_res.height = config->height ? config->height : cur_res.height;
    cur_res.scale_x = config->scale_x ? config->scale_x : cur_res.scale_x;
    cur_res.scale_y = config->scale_y ? config->scale_y : cur_res.scale_y;

    fb_width  = cur_res.width * cur_res.scale_x;
    fb_height = cur_res.height * cur_res.scale_y;

    // allocate CPU buffer no matter what
    logical_fb = malloc(cur_res.width * cur_res.height * sizeof(uint32_t));
    if (!logical_fb) return -1;
    memset(logical_fb, 0, cur_res.width * cur_res.height * sizeof(uint32_t));

    // if user wants GPU, try init GPU stuff
    if (current_usage == PRM_GPU) {
        // try GPU init, fallback silently to CPU if fails
        // NOTE: no actual EGL/context creation here, just assume it's ok
        shader_program = create_shader_program();
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cur_res.width, cur_res.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, logical_fb);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        GLfloat vertices[] = {
            -1, -1, 0, 0,
             1, -1, 1, 0,
            -1,  1, 0, 1,
             1,  1, 1, 1,
        };
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }
    
    printf("\x1b[?25l");
    fflush(stdout);

    return 0;
}

// change resolution
void prm_opengl_es_set_resolution(prm_window_resolution_t *res) {
    cur_res = *res;
    fb_width  = cur_res.width * cur_res.scale_x;
    fb_height = cur_res.height * cur_res.scale_y;

    free(logical_fb);
    logical_fb = malloc(cur_res.width * cur_res.height * sizeof(uint32_t));
    memset(logical_fb, 0, cur_res.width * cur_res.height * sizeof(uint32_t));

    if (current_usage == PRM_GPU) {
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cur_res.width, cur_res.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, logical_fb);
    }
}

// draw a pixel (logical)
void prm_opengl_es_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= cur_res.width || y < 0 || y >= cur_res.height) return;
    logical_fb[y * cur_res.width + x] = color;
}

// clear buffer (logical)
void prm_opengl_es_clear() {
  memset(logical_fb, 0, cur_res.width * cur_res.height * sizeof(uint32_t));
}

// present framebuffer
void prm_opengl_es_present() {
    if (current_usage == PRM_GPU) {
        // upload CPU buffer to GPU
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        cur_res.width, cur_res.height,
                        GL_RGBA, GL_UNSIGNED_BYTE, logical_fb);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, fb_width, fb_height);

        glUseProgram(shader_program);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        GLint pos_loc = glGetAttribLocation(shader_program, "a_pos");
        GLint tex_loc = glGetAttribLocation(shader_program, "a_tex");
        glEnableVertexAttribArray(pos_loc);
        glEnableVertexAttribArray(tex_loc);
        glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(pos_loc);
        glDisableVertexAttribArray(tex_loc);
    }
    
    printf("\x1b[H");
    fflush(stdout);

#ifdef TERMINAL_PRM_EMU
    // write to terminal PRM emulator
    uint32_t *readback = malloc(fb_width * fb_height * sizeof(uint32_t));
    if (current_usage == PRM_GPU) {
        glReadPixels(0, 0, fb_width, fb_height, GL_RGBA, GL_UNSIGNED_BYTE, readback);
    } else {
        // CPU just replicate logical pixels for terminal
        for (int y = 0; y < fb_height; y++) {
            for (int x = 0; x < fb_width; x++) {
                int lx = x / cur_res.scale_x;
                int ly = y / cur_res.scale_y;
                readback[y * fb_width + x] = logical_fb[ly * cur_res.width + lx];
            }
        }
    }
    fwrite(readback, sizeof(uint32_t), fb_width * fb_height, stdout);
    free(readback);
#else
    // if not PRM terminal, optional: ANSI output
    for (int y = 0; y < fb_height; y++) {
        for (int x = 0; x < fb_width; x++) {
            int lx = x / cur_res.scale_x;
            int ly = y / cur_res.scale_y;
            uint32_t c = logical_fb[ly * cur_res.width + lx];
            uint8_t r = (c >> 24) & 0xFF;
            uint8_t g = (c >> 16) & 0xFF;
            uint8_t b = (c >> 8)  & 0xFF;
            printf("\x1b[48;2;%d;%d;%dm ", r,g,b);
        }
        printf("\x1b[0m\n");
    }
#endif
    fflush(stdout);
}

// shutdown
void prm_opengl_es_shutdown() {
    printf("\x1b[?25h");
    fflush(stdout);
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (tex) glDeleteTextures(1, &tex);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (shader_program) glDeleteProgram(shader_program);
    free(logical_fb);
    logical_fb = NULL;
}