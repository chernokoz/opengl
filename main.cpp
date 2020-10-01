#include <iostream>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_shader.h"

double zoom_value = 1;
double zoom_min_value = 0.001;
double zoom_max_value = 1000;
int window_size_x = 1280;
int window_size_y = 720;
double x_center = 0;
double y_center = 0;
double xpos, ypos;

static void glfw_error_callback(int error, const char *description)
{
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void create_triangle(GLuint &vbo, GLuint &vao, GLuint &ebo, GLuint &tex, float texture1D [])
{
   // create the triangle
   float triangle_vertices[] = {
       -1.0f, 1.0f, 0.0f,	// position vertex 1
       1.0f, 0.0f, 0.0f,	 // color vertex 1

       1.0f, -1.0f, 0.0f,  // position vertex 1
       0.0f, 1.0f, 0.0f,	 // color vertex 1

       -1.0f, -1.0f, 0.0f, // position vertex 1
       0.0f, 0.0f, 1.0f,	 // color vertex 1

       1.0f, 1.0f, 0.0f, // position vertex 1
       0.0f, 0.0f, 1.0f,
   };
   unsigned int triangle_indices[] = {
       0, 1, 2, 0, 1, 3};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_FLOAT, texture1D);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float k;

    if (yoffset > 0) {
        k = 1.1;
    } else {
        k = 1 / 1.1;
    }

    if (zoom_value >= zoom_min_value and zoom_value <= zoom_max_value)
    {
        zoom_value *= k;
        if (zoom_value < zoom_min_value)
        {
            zoom_value = zoom_min_value;
        }
        else if (zoom_value > zoom_max_value)
        {
            zoom_value = zoom_max_value;
        }
        else {
            float x_coord = -1.0f + 2 * (float) xpos / (float) window_size_x;
            float y_coord = -1.0f + 2 * (float) ypos / (float) window_size_y;

            x_center = x_center + (x_coord / k - x_coord) * zoom_value;
            y_center = y_center - (y_coord / k - y_coord) * zoom_value;
        }
    }
}

int main(int, char **)
{
   // Use GLFW to create a simple window
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit())
      return 1;


   // GL 3.3 + GLSL 330
   const char *glsl_version = "#version 330";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

   // Create window with graphics context
   GLFWwindow *window = glfwCreateWindow(window_size_x, window_size_y, "Dear ImGui - Conan", NULL, NULL);
   if (window == NULL)
      return 1;
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
   if (glewInit() != GLEW_OK)
   {
      std::cerr << "Failed to initialize OpenGL loader!\n";
      return 1;
   }

    float texture1D [] = {
           20 / 255.0f, 20 / 255.0f, 20 / 255.0f,
           255 / 255.0f, 100 / 255.0f, 255 / 255.0f,
           100 / 255.0f, 255 / 255.0f, 100 / 255.0f,
           200 / 255.0f, 200 / 255.0f, 200 / 255.0f
    };

   // create our geometries
   GLuint vbo, vao, ebo, tex;
   create_triangle(vbo, vao, ebo, tex, texture1D);

   // init shader
   shader_t triangle_shader("simple-shader.vs", "simple-shader.fs");

   // Setup GUI context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);
   ImGui::StyleColorsDark();

   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Get windows size
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);

      // Set viewport to fill the whole window area
      glViewport(0, 0, display_w, display_h);

      // Fill background with solid color
      glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT);

      // Gui start new frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // GUI
      ImGui::Begin("Fractal settings: radius and iterations");
      static float R = 5;
      ImGui::SliderFloat("R_value", &R, 1, 25);
      static int n = 30;
      ImGui::SliderInt("n", &n, 1, 1000);
      ImGui::End();

       glfwGetCursorPos(window ,&xpos, &ypos);


       ImVec2 size = io.DisplaySize;
       ImVec2 mouse_delta = ImGui::GetMouseDragDelta();

       x_center -= mouse_delta.x / size.x * 2 * zoom_value;
       y_center += mouse_delta.y / size.y * 2 * zoom_value;


       ImGui::ResetMouseDragDelta();

       glfwSetScrollCallback(window, scroll_callback);

      // Pass the parameters to the shader as uniforms
      triangle_shader.set_uniform("R_value", R);
      triangle_shader.set_uniform("n_value", n);
      triangle_shader.set_uniform("center", (float) x_center, (float) y_center);
      triangle_shader.set_uniform("zoom", (float) zoom_value);

      auto model = glm::rotate(glm::mat4(1), glm::radians(0.0f), glm::vec3(0, 1, 0));
      auto view = glm::lookAt<float>(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
      auto projection = glm::perspective<float>(90, 1, 0.1, 100);
      auto mvp = projection * view * model;
      //glm::mat4 identity(1.0); 
      //mvp = identity;
      //triangle_shader.set_uniform("u_mvp", glm::value_ptr(mvp));

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_1D, tex);
      glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_FLOAT, texture1D);
      triangle_shader.set_uniform("tex", 0);

      // Bind triangle shader
      triangle_shader.use();
      // Bind vertex array = buffers + indices
      glBindVertexArray(vao);
      // Execute draw call
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      // Generate gui render commands
      ImGui::Render();

      // Execute gui render commands using OpenGL backend
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Swap the backbuffer with the frontbuffer that is used for screen display
      glfwSwapBuffers(window);
   }

   // Cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}
