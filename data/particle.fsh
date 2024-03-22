in vec4 v_color;
uniform vec4 u_color;

out vec4 PixelColor;

void main()
{
	
    PixelColor = u_color + v_color;
}
