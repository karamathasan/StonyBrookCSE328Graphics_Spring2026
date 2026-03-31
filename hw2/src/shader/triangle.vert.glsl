#version 410 core

// The "a" prefix stands for "attribute".
layout (location = 0) in vec2 aPosition;

uniform mat3 model;

uniform float windowWidth;
uniform float windowHeight;

void main()
{
    vec3 transformedPosition = model * vec3(2.0f * aPosition.x / windowWidth - 1.0f,
                                            2.0f * aPosition.y / windowHeight - 1.0f,
                                            1.0f);

    gl_Position = vec4(transformedPosition.xy, 0.0f, 1.0f);
}
