#version 100

precision mediump float;

uniform mat4 uMat;

attribute float aAngle;
attribute vec2 aPos;
attribute vec2 aTex;
attribute vec2 aLocalPos;

varying vec2 vTex;

void main() {
	vec4 localPos = vec4(aLocalPos, 0, 0);
	vec4 pos = vec4(aPos, 0, 1);
	mat4 rot = mat4(cos(aAngle), sin(aAngle), 0, 0,
				   -sin(aAngle), cos(aAngle), 0, 0,
					          0,           0, 1, 0,
  					          0,           0, 0, 1);
	
	gl_Position = uMat * (pos + rot * localPos);
	vTex = aTex;
}
