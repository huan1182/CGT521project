#include <windows.h>

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "InitShader.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "imgui_impl_glut.h"
#include "VideoMux.h"

//names of the shader files to load
static const std::string vertex_shader("vertex_shader.glsl");
static const std::string fragment_shader("fragment_shader.glsl");
static const std::string compute_shader("compute_shader.glsl");
GLuint compute_program = -1;
GLuint shader_program = -1;
GLuint texture_id = -1; //Texture map for fish

//names of the mesh and texture files to load
static const std::string mesh_name = "Amago0.obj";
static const std::string texture_name = "AmagoT.bmp";

unsigned int vbo;
unsigned int scorevbo;
unsigned int vao;
unsigned int tbo;

float time_sec = 0.0f;
float angle = 0.0f;
bool recording = false;
int inputx = 0;
int inputy = 0;
void reload_shader();
struct object 
{
	glm::vec4 pos;
	float size;
	float score;
	int collision;
	
	float speed;
};

//Draw the user interface using ImGui
void draw_gui()
{
   ImGui_ImplGlut_NewFrame();

   const int filename_len = 256;
   static char video_filename[filename_len] = "capture.mp4";

   if (ImGui::Button("Reload Shader"))
   {
	   reload_shader();
   }

   ImGui::InputText("Video filename", video_filename, filename_len);
   ImGui::SameLine();
   if (recording == false)
   {
      if (ImGui::Button("Start Recording"))
      {
         const int w = glutGet(GLUT_WINDOW_WIDTH);
         const int h = glutGet(GLUT_WINDOW_HEIGHT);
         recording = true;
         start_encoding(video_filename, w, h); //Uses ffmpeg
      }
      
   }
   else
   {
      if (ImGui::Button("Stop Recording"))
      {
         recording = false;
         finish_encoding(); //Uses ffmpeg
      }
   }

   //create a slider to change the angle variables
   ImGui::SliderFloat("View angle", &angle, -3.141592f, +3.141592f);

   ImGui::Image((void*)texture_id, ImVec2(128,128));

   static bool show = false;
   ImGui::ShowTestWindow();
   ImGui::Render();
 }

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
   //clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //***********************************************************
  // glUseProgram(compute_program);
  // glBindImageTexture(0, tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);// access to the player's postion data
  // glDispatchCompute(4, 1, 1);
  // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   //***********************************************************
   glUseProgram(shader_program);

   const int w = glutGet(GLUT_WINDOW_WIDTH);
   const int h = glutGet(GLUT_WINDOW_HEIGHT);
   const float aspect_ratio = float(w) / float(h);

   glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 V = glm::lookAt(glm::vec3(0.3f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 P = glm::perspective(3.141592f / 4.0f, aspect_ratio, 0.1f, 100.0f);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   int tex_loc = 1;
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   int PVM_loc = 0;
   if (PVM_loc != -1)
   {
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   glBindVertexArray(vao);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDrawArrays(GL_POINTS, 0, 10000);
   //glDrawElements(GL_TRIANGLES, mesh_data.mNumIndices, GL_UNSIGNED_INT, 0);
         
   draw_gui();

   if (recording == true)
   {
      glFinish();

      glReadBuffer(GL_BACK);
      read_frame_to_encode(&rgb, &pixels, w, h);
      encode_frame(rgb);
   }

   glutSwapBuffers();
   glUseProgram(compute_program);
  // glBindImageTexture(0, tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);// access to the player's postion data
   glDispatchCompute(100, 1, 1);
   //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

// glut idle callback.
//This function gets called between frames
void idle()
{
	glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   time_sec = 0.001f*time_ms;
   glUseProgram(compute_program);
   GLuint timeLoc = 3;
   glUniform1f(timeLoc, time_sec);
}

void reload_shader()
{
   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

   if(new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
   }
   else
   {
      glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

      if(shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;

   }
}

// Display info about the OpenGL implementation provided by the graphics driver.
// Your version should be > 4.0 for CGT 521 
void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
void initOpenGl()
{
   //Initialize glew so that new OpenGL function names can be used
   glewInit();
   compute_program = InitShader(compute_shader.c_str());
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_POINT_SPRITE);
   glEnable(GL_PROGRAM_POINT_SIZE);
   reload_shader();

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   //initialize VBO
   std::vector<object> point_verts(10000);
   for (int n = 0; n < 10000; n++)
   {
	   point_verts[n] .pos= glm::vec4(0.0001*(float)n,0.0f, 0.0f, 1.0f);
	   point_verts[n].collision = 0;
	   point_verts[n].score = 0;
	   point_verts[n].size = 0;
	   point_verts[n].speed = 0;

   }
   point_verts[0].pos = point_verts[0].pos+glm::vec4(2.0f, 0.4f, 0.0f, 0.0f);
   point_verts[0].score = 1;
   point_verts[0].size = 20;

   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, point_verts.size() * sizeof(object), &point_verts[0], GL_DYNAMIC_COPY);

   GLint pos_loc = 1;
   GLint size_loc = 2;
   GLint score_loc = 3;
   glEnableVertexAttribArray(pos_loc);
   int stride = 6 * sizeof(float) + 2 * sizeof(int);
   //score
   glVertexAttribPointer(pos_loc, 4, GL_FLOAT, false, stride, 0);
   glEnableVertexAttribArray(size_loc);
   glVertexAttribPointer(size_loc, 1, GL_FLOAT, false, stride, BUFFER_OFFSET(4 * sizeof(float)));
   glEnableVertexAttribArray(score_loc);
   glVertexAttribPointer(score_loc, 1, GL_FLOAT, false, stride, BUFFER_OFFSET(5 * sizeof(float)));
   //score
   //glGenBuffers(1, &scorevbo);
  // glBindBuffer(GL_ARRAY_BUFFER, scorevbo);
  // glBufferData(GL_ARRAY_BUFFER, point_verts.size() * sizeof(object), &point_verts[0], GL_DYNAMIC_COPY);//
 //  glVertexAttribPointer(pos_loc, 1, GL_INT, false, stride, BUFFER_OFFSET(4 * sizeof(float) + sizeof(int)));
  // glBindVertexArray(0);

   glEnable(GL_PROGRAM_POINT_SIZE);
   //glPointSize(10.0f);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
   //***************************test for SBO
   //glUseProgram(compute_program);
   //glGenTextures(1, &tbo);
   //glBindTexture(GL_TEXTURE_BUFFER, tbo);
   //glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo);
   //Load a mesh and a texture
   texture_id = LoadTexture(texture_name.c_str()); //Helper function: Uses FreeImage library
}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);
   std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();
		 break;
	  case 'w':
	  case 'W':
		  inputy = 1;
		  break;
	  case 's':
	  case 'S':
		  inputy = -1;
		  break;
	  case 'a':
	  case 'A':
		  inputx = -1;
		  break;
	  case 'd':
	  case 'D':
		  inputx = 1;
          break;
   }
   glUseProgram(compute_program);
   glUniform1i(1,inputy);
   glUniform1i(2, inputx);
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);

   switch (key)
   {
 
   case 'w':
   case 'W':
   case 's':
   case 'S':
	   inputy = 0;
	   break;
   case 'a':
   case 'A':
   case 'd':
   case 'D':
	   inputx = 0;
	   break;
   }
   glUseProgram(compute_program);
   glUniform1i(1, inputy);
   glUniform1i(2, inputx);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x,y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}


int main (int argc, char **argv)
{
   //Configure initial window state using freeglut
   glutInit(&argc, argv); 
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (1280, 720);
   int win = glutCreateWindow ("Huyunting Huang Final Project");

   printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special);
   glutKeyboardUpFunc(keyboard_up);
   glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);

   glutIdleFunc(idle);

   initOpenGl();
   ImGui_ImplGlut_Init(); // initialize the imgui system

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;		
}


