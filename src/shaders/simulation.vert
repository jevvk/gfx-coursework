#version 330

in vec3 position;

uniform mat4 modelview;
uniform mat4 projection;
uniform vec2 screenSize;
uniform float spriteSize;

void main(void) {
  // vec4 eyePos = modelview * vec4(position, 1.0f);
  // vec4 projVoxel = projection * vec4(spriteSize, spriteSize, eyePos.z, eyePos.w);
  // vec2 projSize = screenSize * projVoxel.xy / projVoxel.w;

  // gl_PointSize = 0.25 * (projSize.x + projSize.y);
  gl_Position = projection * modelview * vec4(position, 1.0f);
}
