uniform mat4 P;
uniform mat4 V;

attribute vec3 pos;
attribute vec2 tex;

attribute mat4 M;
attribute vec4 tint;
attribute vec2 texOffset;

varying vec2 texCoord;
varying vec4 tintCol;

void kore() {
	gl_Position = P * V * M * vec4(pos, 1.0);
	texCoord = (tex + texOffset) / 2.0;
	tintCol = tint;
}