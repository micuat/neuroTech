#version 120

#extension GL_ARB_draw_buffers : enable

// Hmm, do we really need to give the path to the shader if it's in the same folder?
#pragma include "Shaders/Common/ShaderHelpers.glslinc"
//#pragma include "Shaders/Common/SimplexNoiseDerivatives4D.glslinc"
#pragma include "Shaders/Common/Noise2D.glslinc"
#pragma include "Shaders/Common/Noise4D.glslinc"

uniform sampler2D u_positionAndAgeTex;

uniform float u_time;
uniform float u_timeStep;

uniform float u_particleMaxAge;
uniform float u_particleSpawnFreq;

uniform float u_noisePositionScale = 1.5;
uniform float u_noiseMagnitude = 0.075;
uniform float u_noiseTimeScale = 1.0 / 4000.0;
uniform float u_noisePersistence = 0.2;
uniform vec3 u_wind = vec3( 0.5, 0.0, 0.0 );

uniform int u_spawnParticles;

uniform float u_griding;
uniform float u_cylindering;

uniform float u_scaling;

const int OCTAVES = 3;

vec3 gridPosition(vec2 texCoord, float scale)
{
	vec3 gridPos = vec3(texCoord - vec2(0.5f, 0.5f), 0);
	//gridPos.z = gridPos.x * gridPos.x;
	vec3 cylinderPos = vec3(cos((texCoord.x - 0.5f) * 2 * 3.1415f), texCoord.y - 0.5f, sin((texCoord.x - 0.5f) * 2 * 3.1415f));
	gridPos = gridPos * (1 - u_cylindering) + cylinderPos * u_cylindering;
	if(texCoord.x > 0.5f)
	{
		gridPos.x = gridPos.x * u_scaling + 0.5f * (1 - u_scaling);
	}
	else
	{
		gridPos.x = gridPos.x * u_scaling + -0.5f * (1 - u_scaling);
	}
	if(texCoord.y > 0.5f)
	{
		gridPos.y = gridPos.y * u_scaling + 0.5f * (1 - u_scaling);
	}
	else
	{
		gridPos.y = gridPos.y * u_scaling + -0.5f * (1 - u_scaling);
	}
	return gridPos;
}

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = gl_TexCoord[0].st;

	vec4 posAndAge = texture2D( u_positionAndAgeTex, texCoord );
	vec4 posAndAgeNext = texture2D( u_positionAndAgeTex, texCoord + vec2(1, 0) );

	vec3 pos = posAndAge.xyz;
	vec3 posNext = posAndAgeNext.xyz;

	float age = posAndAge.w;

	age += u_timeStep;

	if( age > u_particleMaxAge )
	{
		if(u_spawnParticles != 0 && mod(snoise(texCoord * u_particleSpawnFreq) + u_time / u_particleMaxAge, 1) < 0.3f)
		{
			age = mod(age, 1) - u_particleMaxAge;

			if(u_griding > 0)
			{
				pos = gridPosition(texCoord, u_griding);
			}
			else
			{
				float spawnRadius = 0.1;
				pos = randomPointOnSphere( vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ))) * spawnRadius;
			}
		}
		else
		{
			//gl_FragData[0] = vec4( vec3(1000000, 1000000, 1000000), age );
			//return;
		}
	}

	vec3 noisePosition = pos  * u_noisePositionScale;
	float noiseTime    = u_time * u_noiseTimeScale;

	vec3 noiseVelocity = fbmvec3( vec4(noisePosition, noiseTime), OCTAVES, 2.0, u_noisePersistence ) * u_noiseMagnitude;
	//vec3 noiseVelocity = curlNoise( noisePosition, noiseTime, OCTAVES, u_noisePersistence ) * u_noiseMagnitude;

	vec2 posDir = noisePosition.xz / length(noisePosition.xz);
	vec3 totalVelocity = vec3(posDir.x * u_wind.x, u_wind.y, posDir.y * u_wind.z) + noiseVelocity;

	vec3 newPos = pos + totalVelocity * u_timeStep;

	if(u_griding > 0)
	{
		vec3 gridPos = gridPosition(texCoord, u_griding);;
		newPos.xyz = newPos * (1 - u_griding) + gridPos * u_griding;
	}

	pos = newPos;

	gl_FragData[0] = vec4( pos, age );
}
