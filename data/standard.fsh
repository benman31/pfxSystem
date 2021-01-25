// Point Light
uniform vec4 u_light0PosRange; // range is in .w
uniform vec3 u_light0Color;
uniform vec3 u_light0Attenuation;
uniform vec4 u_light1PosRange; // range is in .w
uniform vec3 u_light1Color;
uniform vec3 u_light1Attenuation;
uniform vec4 u_light2PosRange; // range is in .w
uniform vec3 u_light2Color;
uniform vec3 u_light2Attenuation;
uniform vec4 u_light3PosRange; // range is in .w
uniform vec3 u_light3Color;
uniform vec3 u_light3Attenuation;

// Material properties
uniform sampler2D u_diffuseTex;
uniform vec3 u_specularColor;
uniform float u_shininess;

// Scene properties
uniform vec3 u_viewPos;
uniform vec3 u_ambientLight;

in vec4 v_normal;
in vec2 v_uv1;
in vec3 v_pos;
out vec4 PixelColor;

vec3 calc_lighting_point(vec4 lightPosRange, vec3 lightColor, vec3 lightAttenuation, vec3 samplePos, 
						 vec3 n, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess)
{
	vec3 lightDir = lightPosRange.xyz - samplePos.xyz;
	float distance = length(lightDir);

	if( distance > lightPosRange.w )
	{
		return vec3(0,0,0);	
	}
	else
	{
		// normalize it
		lightDir /= distance;
		// Diffuse
		vec3 diffuse = diffuseColor * lightColor * max(0.0,dot(n, lightDir));
		// Specular
		vec3 R = reflect(-lightDir,n);
		vec3 spec = pow(max(dot(R, viewDir), 0.0), shininess) * lightColor * specularColor;
		// Combine the components
		vec3 light = clamp(diffuse + spec, 0, 1);
		// Attenuate the light
		float att = 1.0 / dot(lightAttenuation, vec3(1.0, distance, distance*distance));
		light *= att;

		return light;
	}
}

void main()
{
    vec3 diffuseMat = texture(u_diffuseTex, v_uv1).rgb;
	vec3 n = normalize( v_normal.xyz );

	vec3 color = vec3(0,0,0);

	vec3 viewDir = normalize(u_viewPos - v_pos);
	color += calc_lighting_point(u_light0PosRange, u_light0Color, u_light0Attenuation, v_pos, n, viewDir, diffuseMat, u_specularColor, u_shininess);
	color += calc_lighting_point(u_light1PosRange, u_light1Color, u_light1Attenuation, v_pos, n, viewDir, diffuseMat, u_specularColor, u_shininess);
	color += calc_lighting_point(u_light2PosRange, u_light2Color, u_light2Attenuation, v_pos, n, viewDir, diffuseMat, u_specularColor, u_shininess);
	color += calc_lighting_point(u_light3PosRange, u_light3Color, u_light3Attenuation, v_pos, n, viewDir, diffuseMat, u_specularColor, u_shininess);
	color += u_ambientLight * diffuseMat;

	PixelColor = vec4(color, 1.0);
}

