// Spot Light
uniform vec4 u_lightPosRange; // range is in .w
uniform vec3 u_lightColor;
uniform vec3 u_lightAttenuation;
uniform vec4 u_lightSpot;

// Material properties
uniform sampler2D u_diffuseTex;
uniform vec3 u_specularColor;
uniform float u_shininess;

// Scene properties
uniform vec3 u_viewPos;
uniform vec3 u_ambientLight;

in vec4 v_normal;
in vec4 v_color;
in vec3 v_pos;
out vec4 PixelColor;

// spotlight: xyz = spot direction, w = spot "angle"
vec3 calc_lighting(vec4 lightPosRange, vec3 lightColor, vec3 lightAttenuation, vec4 spotlight, 
				   vec3 samplePos, vec3 n, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess)
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
		// Calculate spot light falloff
		float spot = pow(max(dot(-lightDir,spotlight.xyz), 0.0), spotlight.w);
		// Attenuate the light
		float att = spot / dot(lightAttenuation, vec3(1.0, distance, distance*distance));
		light *= att;

		return light;
	}
}

void main()
{
    vec3 diffuseMat = v_color.rgb;
	vec3 n = normalize( v_normal.xyz );

	vec3 color = vec3(0,0,0);

	vec3 viewDir = normalize(u_viewPos - v_pos);
	color += calc_lighting(u_lightPosRange, u_lightColor, u_lightAttenuation, u_lightSpot, v_pos, n, viewDir, diffuseMat, u_specularColor, u_shininess);
	color += u_ambientLight * diffuseMat;

	PixelColor = vec4(color, 1.0);
}

