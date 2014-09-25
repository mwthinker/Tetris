#version 100

precision mediump float;

uniform mat4 uMat;

attribute vec2 aPos;
attribute vec4 aColor;

varying vec4 vColor;

void main() {
	gl_Position = uMat * vec4(aPos, 0, 1);
	vColor = aColor;
}
