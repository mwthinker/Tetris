#version 100

precision mediump float;

uniform mat4 uProj;
uniform mat4 uModel;

attribute vec2 aPos;
attribute vec2 aTex;

varying vec2 vTex;

void main() {
	gl_Position = uProj * uModel * vec4(aPos, 0, 1);
	vTex = aTex;
}
