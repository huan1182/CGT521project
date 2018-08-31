#version 430            
layout (location = 0) uniform mat4 PVM;
layout (location = 3) uniform float time;
layout (location=1) in vec4 pos;
layout (location=2) in float size; 
layout (location=3)in float Score;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;
out vec2 tex_coord; //varying variable  
out float score;
void main(void)
{
	//vec3 d = vec3(0.0, 0.0, .05 * sin(time + 10 * pos_attrib.x));
    gl_Position = PVM*pos;
	score=size;
    
	int id = gl_VertexID;
	int special=id%500;
    tex_coord = tex_coord_attrib;
	
	    gl_PointSize = 9.0+float(id*id)*0.0001*sin(time*float(id)/500*0.001);
		if(special==0&&id!=0)
		{
		    gl_PointSize=35;
		}
		else if (id==0)
		{
		    gl_PointSize=25+size;
		}    
}