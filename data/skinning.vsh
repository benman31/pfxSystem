uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform mat4 worldIT;

const int MAX_BONES = 100;
uniform mat4 u_bones[MAX_BONES];

in vec4 a_position;
in vec4 a_color;
in vec3 a_normal;
in vec4 a_boneIndices;
in vec4 a_boneWeights;

out vec4 v_normal;
out vec4 v_color;
out vec3 v_pos;

void main()
{
    mat4 BoneTransform = u_bones[int(a_boneIndices.x)] * a_boneWeights.x;
    BoneTransform += u_bones[int(a_boneIndices.y)] * a_boneWeights.y;
    BoneTransform += u_bones[int(a_boneIndices.z)] * a_boneWeights.z;
    BoneTransform += u_bones[int(a_boneIndices.w)] * a_boneWeights.w;

    mat4 finalWorld = world * BoneTransform;

    gl_Position = projection * view * finalWorld * vec4(a_position.xyz, 1.0);
	v_pos = (finalWorld * a_position).xyz;
    v_normal.xyz = mat3(finalWorld) * a_normal;
    v_color = a_color;
}