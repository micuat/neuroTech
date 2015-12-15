#version 120

#pragma include "Shaders/Common/ShaderHelpers.glslinc"

uniform sampler2D u_particleDataTexture;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjectionMatrix;

uniform float u_particleDiameter;
uniform float u_screenWidth;

uniform float u_particleMaxAge;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

uniform float u_stringTheory;

uniform int u_meshMode;
uniform int u_stringDirection;

varying vec4 v_particleColor;

float oneOverTexDim = 1.0f / 128.0f;

uniform int u_griding;

// ----------------------------
void main ()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec2 texCoord = gl_TexCoord[0].st;

	vec4 particleData = texture2D( u_particleDataTexture, texCoord );
	vec4 particleDataNext;
	if(u_stringDirection == 0) // East
	{
		if(mod(texCoord.s * 200, 2) <= 0.5f)
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord + vec2(oneOverTexDim, 0) );
		}
		else
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord - vec2(oneOverTexDim, 0) );
		}
	}
	else if(u_stringDirection == 1) // South
	{
		if(mod(texCoord.t * 200, 2) <= 0.5f)
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord + vec2(0, oneOverTexDim) );
		}
		else
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord - vec2(0, oneOverTexDim) );
		}
	}
	else if(u_stringDirection == 2) // West
	{
		if(mod(texCoord.s * 200, 2) <= 0.5f)
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord - vec2(oneOverTexDim, 0) );
		}
		else
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord + vec2(oneOverTexDim, 0) );
		}
	}
	else if(u_stringDirection == 3) // North
	{
		if(mod(texCoord.t * 200, 2) <= 0.5f)
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord - vec2(0, oneOverTexDim) );
		}
		else
		{
			particleDataNext = texture2D( u_particleDataTexture, texCoord + vec2(0, oneOverTexDim) );
		}
	}

	vec3 pos = particleData.xyz;
	vec3 posNext = particleDataNext.xyz;
	//pos = vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ));

	float frac = particleData.w / u_particleMaxAge;
	float fracNext = particleDataNext.w / u_particleMaxAge;

	if(frac >= 1.0f || fracNext >= 1.0f)
	{
		pos = posNext;//vec3(1000000, 1000000, 1000000);
		gl_Position = u_modelViewProjectionMatrix * vec4(pos, 1.0);

		v_particleColor = vec4(0, 0, 0, 0);
		return;
	}
	else
	{
		v_particleColor = mix(u_particleStartColor, u_particleEndColor, frac );
		//v_particleColor = vec4(0,1,1,1);
	}

	if(mod(texCoord.s * 200, 2) <= 0.5f)
	{
		//pos = pos * 0.0f + texCoord * 1.0f;
		//pos = posNext - vec3(0.01f, 0, 0);

	}
	else
	{
		//pos = pos * 0.0f + texCoord * 1.0f;// + vec2(0, 0.1f / 100.0f);
		//pos = pos * 0.0f + vec2(-texCoord.s, texCoord.t) * 1.0f;
	}
	if(u_meshMode == 3)
	{
		float dist = distance(pos, posNext);
		float alpha = 0;
		if(u_stringTheory - dist > 0)
		{
			alpha = (u_stringTheory - dist) / u_stringTheory;
			if(alpha > 0.5f)
			{
				alpha = 1;
			}
			else
			{
				alpha *= 2;
			}
			//alpha *= alpha;
		}
		else
		{
			pos = posNext;
		}
		v_particleColor.a *= alpha;
	}

	vec3 viewSpacePosition = vec3(u_viewMatrix * vec4(pos, 1.0));

	vec4 corner = vec4(u_particleDiameter * 0.5, u_particleDiameter * 0.5, viewSpacePosition.z, 1.0);
	float projectedCornerX = dot(vec4(u_projectionMatrix[0][0], u_projectionMatrix[1][0], u_projectionMatrix[2][0], u_projectionMatrix[3][0]), corner);
	float projectedCornerW = dot(vec4(u_projectionMatrix[0][3], u_projectionMatrix[1][3], u_projectionMatrix[2][3], u_projectionMatrix[3][3]), corner);

	gl_PointSize = u_screenWidth * 0.5 * projectedCornerX * 2.0 / projectedCornerW;

	gl_Position = u_modelViewProjectionMatrix * vec4(pos, 1.0);
}
