#version 430

layout(location = 1) uniform sampler2D diffuse_color;
layout (location = 3) uniform float time;

out vec4 fragcolor;           
in vec2 tex_coord;
in float score;
vec4 make_stripes(vec2);
      
void main(void)
{   
    int id=gl_PrimitiveID;
    int special=id%500;
	float r = length(gl_PointCoord.st-vec2(0.5));
	float s = smoothstep(0.5, 0.3, r);
	if (length(gl_PointCoord.st-vec2(0.5))>=0.5) discard;
	//mix 2 hot colors to get a fire look.
	vec4 color0 = vec4(0.7, 0.5, 0.3, 0.7*s);
	vec4 color1 = vec4(0.7, 0.1, 0.0, 0.3*s);
	fragcolor = mix(color1, color0, float(id)/10000);
	//fragcolor=vec4(gl_PointCoord.s,1.0,0.0,1.0);
	
	if(id==0)
	{
         color0 = vec4(0.3, 0.4, 0.7f, 0.7*s);
	     color1 = vec4(0.0, 0.1, 0.7f, 0.3*s);
	     fragcolor = mix(color1, color0, 1.0)+0.01*vec4(score,0.0,0.0,0.0);
	}
	if(id!=0&&special==0)
	{
         color0 = vec4(0.3, 0.7, 0.4f, 0.7*s);
	     color1 = vec4(0.1, 0.7, 0.1f, 0.3*s);
	     fragcolor = mix(color1, color0, 1.0)+0.01*vec4(score,0.0,0.0,0.0);
	}
}

vec4 c1 = vec4(.5, 0.0, 1.0, 1.0);
vec4 c2 = vec4(0.0, 1.0, 0.0, 1.0);

vec4 make_stripes(vec2 t){
	float a = fract(10.0 * t.s);
	a = step(.5, a);

	return mix(c2, c1, a);
}




















