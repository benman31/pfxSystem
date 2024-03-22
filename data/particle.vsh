uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mWorld;

in vec4 a_position;
in vec4 a_color;

out vec4 v_color;

void main()
{
    gl_Position = mProj * mView * mWorld * a_position;
    v_color = a_color;
}
