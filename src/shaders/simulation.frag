#version 330

out vec4 outColor;

uniform sampler2D tex;

void main(void) {
	// outColor = texture(tex, gl_PointCoord) * vec4(251f / 255f, 172f / 255f, 71f / 255f, 1);
	outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
