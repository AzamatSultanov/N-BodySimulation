#version 330

in vec4 fragColor;
in vec2 fragTexCoord;

uniform sampler2D texture0;
out vec4 finalColor;

void main()
{
    vec2 texOffset = 1.0 / textureSize(texture0, 0);
    vec4 color = vec4(0.0);

    // A Gaussian Kernel matrix
    // Corners get weight 1, Edges get weight 2, Center gets weight 6
    // This keeps the center sharp while softly blending the edges.
    
    // Top row
    color += texture(texture0, fragTexCoord + vec2(-1.0, -1.0) * texOffset) * 1.0;
    color += texture(texture0, fragTexCoord + vec2( 0.0, -1.0) * texOffset) * 2.0;
    color += texture(texture0, fragTexCoord + vec2( 1.0, -1.0) * texOffset) * 1.0;
    
    // Middle row
    color += texture(texture0, fragTexCoord + vec2(-1.0,  0.0) * texOffset) * 2.0;
    color += texture(texture0, fragTexCoord + vec2( 0.0,  0.0) * texOffset) * 6.0; // <-- High center weight!
    color += texture(texture0, fragTexCoord + vec2( 1.0,  0.0) * texOffset) * 2.0;
    
    // Bottom row
    color += texture(texture0, fragTexCoord + vec2(-1.0,  1.0) * texOffset) * 1.0;
    color += texture(texture0, fragTexCoord + vec2( 0.0,  1.0) * texOffset) * 2.0;
    color += texture(texture0, fragTexCoord + vec2( 1.0,  1.0) * texOffset) * 1.0;

    // The total sum of all the weights above is 18.0
    finalColor = (color / 18.0) * fragColor;
}