#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 anormal;
in vec3 FragPos;

uniform vec3 lampPos;
uniform vec3 lampColor;
uniform vec3 viewPos;
//uniform vec3 objectColor;

// Ouput data
//out vec3 objectColor;
out vec4 FragColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTexture;

void main(){

	// Output color = color of the texture at the specified UV
    vec3 objectColor = texture( myTexture, UV ).rgb;
    
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lampColor;

    // diffuse
    vec3 norm = normalize(anormal);
    vec3 lightDir = normalize(lampPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.6 * diff * lampColor;

    // specular
    float specularStrength = 2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lampColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
