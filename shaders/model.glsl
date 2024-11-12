#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aJoints;
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

out vec2 fragTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

uniform mat4 u_Joints[MAX_BONES];

out vec3 fragNormal;

void main() {
    fragNormal = aNormal;
    fragTexCoords = aTexCoords;

    // vec4 totalLocalPos = vec4(0.0);

    // vec4 vertexPos = vec4(aPos, 1.0);
    // for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
    //     mat4 jointTransform = u_Joints[aJoints[i]];
    //     vec4 posePosition = jointTransform * vertexPos;
    //     totalLocalPos += posePosition * aWeights[i];
    // }
    
    mat4 skinMat = aWeights.x * u_Joints[aJoints.x] + 
                   aWeights.y * u_Joints[aJoints.y] +    
                   aWeights.z * u_Joints[aJoints.z] +
                   aWeights.w * u_Joints[aJoints.w];

    vec4 worldPos = skinMat * vec4(aPos, 1.0);
                    
    gl_Position = u_ViewProjection * u_Model * worldPos;
}
#SPLIT
#version 330 core
out vec4 fragColor;

in vec2 fragTexCoords;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 u_baseColor;

void main() {
    vec4 textureColor = texture(texture0, fragTexCoords);    
    if (textureColor.xyz != vec3(0.0)) {
        fragColor = textureColor * u_baseColor;
    } else {
        fragColor = u_baseColor;
    }
}
