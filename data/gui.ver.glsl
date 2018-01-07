#version 100

precision mediump float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform mat4 uPos;
uniform mat4 uTex;

attribute vec2 aPos;

varying vec2 vTex;

void main() {
	gl_Position = uProj * uModel * uPos * vec4(aPos, 0, 1);
	vec4 tex = uTex * vec4(aPos, 0, 1);
	vTex = tex.xy;
}
