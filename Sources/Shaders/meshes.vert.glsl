attribute vec3 pos;
attribute vec2 tex;
attribute vec3 nor;

attribute mat4 MVP;
varying vec2 texCoord;

void kore() {
	gl_Position = MVP * vec4(pos, 1.0);
	texCoord = tex;
}