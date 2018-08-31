#version 430 core

struct particle
{
   vec4 pos;
   float size;
   float score;
   int collision;
   
   
   float speed;
};

layout(std430, binding=0) buffer particles
{
  particle point[];
};

layout(local_size_x = 100,local_size_y = 1,local_size_z = 1) in;
layout(location=3) uniform float time;
layout(location=1) uniform int inputy;
layout(location=2) uniform int inputx;


int collisiondetect(vec4 p0, vec4 p1,float size);
vec4 velocityplayer(int statusx,int statusy);
vec4 velocityblock(uint id, vec4 pos, vec4 pos1,float size,int special);
float rand(vec2 co);

void main()
{
    int interval= int(time)%50;
    uint gid = gl_GlobalInvocationID.x;
	//behavior of player
    int statusy=inputy;
	int statusx=inputx;
	int special= int (gid)%500;
	vec4 playervelocity=velocityplayer(statusx,statusy);
	point[0].speed=point[0].score*0.004;
	point[0].pos=point[0].pos+playervelocity*(1+point[0].speed);
	if (interval==0) point[0].score=point[0].score+1;
	if(point[0].size<=0)point[0].size=0.01; 
	if(length(point[0].pos)>=8.0)
	{
         point[0].pos=vec4(0.0,0.0,0.0,1.0f);
		 point[0].score=0;
		 point[0].size=20;
    }
	if(gid!=0)// behavior of the block
		{
		 int a = collisiondetect (point[0].pos,point[gid].pos,point[0].size);
		 vec2 seed = vec2(float(gid), time);
		 float boundary=length(point[gid].pos);
		 if(a==1)
		   {
		      point[gid].collision=1;
		   }
		 if(point[gid].collision==1||boundary>=8.0)
		   {
		      
			  
			      //point[gid].score=point[gid].score-1;
				  if(point[gid].collision==1&&special==0)
				  {
				         point[0].size=point[0].size-1;
						 point[0].score=point[0].score-1;
						 
			      }
				  point[gid].collision=0;
		          point[gid].pos =point[gid].pos*0.003+vec4(rand(seed.xy), 0.12*(rand(seed.yx)+2*(int(gid)/1000-4.5)+2*(int(gid)/500-9.5)), 0.5f,0.0f)-vec4(0.5);
		          point[gid].pos.a=1.0f;
		          point[gid].pos.x=point[gid].pos.x+7.4;
			      
		   }
		  else
		  {
		          vec4 objectpos=point[gid].pos;
		          vec4 blockvelocity=velocityblock(gid,objectpos,point[0].pos,point[0].size,special);
		          point[gid].pos = objectpos+0.4*(1+0.003*point[0].score)*blockvelocity;
		   }	 
	}
}

int collisiondetect(vec4 p0, vec4 p1,float size)
{
   vec4 pr = p0-p1;
   float dis = length (pr);
   if (dis<0.1+0.02*size) return 1;
   return 0;
}
vec4 velocityplayer (int statusx,int statusy)
{
	return vec4 (0.05*statusx,0.05*statusy,0.0,0.0);
}

vec4 velocityblock(uint id, vec4 pos,vec4 pos1,float size,int special)
{
     vec4 delta = pos-pos1;
	 float x=-0.00002*int(id)-0.07;
	 float y=0.0000003*sin(time*(int(id+1)-5000)*0.0001)*int(id)*(int(id)/1000-4.5)+0.00001*(int(id+1)/500-10);
	 float detect = 40-size;
	 if (special==0)
	 {
	    if (length(delta)<1.2+0.01*detect)
		return -delta*0.05;
	 }
	 if(length(delta)<1.0+0.02*size)
	 {
	   return delta*0.2;
	 }
     return vec4(x,y,0.0,0.0);
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}













