/*
 * TessuMod: Mod for integrating TeamSpeak into World of Tanks
 * Copyright (C) 2015  Janne Hakonen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "proxies.h"
#include "structures.h"
#include "../utils/logging.h"

#include <Windows.h>
#include <iostream>

namespace
{

LPALBUFFERDATA           g_alBufferData;
LPALDELETEBUFFERS        g_alDeleteBuffers;
LPALDELETESOURCES        g_alDeleteSources;
LPALGENBUFFERS           g_alGenBuffers;
LPALGENSOURCES           g_alGenSources;
LPALGETERROR             g_alGetError;
LPALGETSOURCEI           g_alGetSourcei;
LPALGETSTRING            g_alGetString;
LPALSOURCEPLAY           g_alSourcePlay;
LPALSOURCESTOP           g_alSourceStop;
LPALSOURCEQUEUEBUFFERS   g_alSourceQueueBuffers;
LPALLISTENER3F           g_alListener3f;
LPALLISTENERF            g_alListenerf;
LPALLISTENERFV           g_alListenerfv;
LPALSOURCE3F             g_alSource3f;
LPALSOURCEF              g_alSourcef;
LPALSOURCEI              g_alSourcei;
LPALSOURCEUNQUEUEBUFFERS g_alSourceUnqueueBuffers;

LPALCOPENDEVICE          g_alcOpenDevice;
LPALCCREATECONTEXT       g_alcCreateContext;
LPALCMAKECONTEXTCURRENT  g_alcSetThreadContext;
LPALCDESTROYCONTEXT      g_alcDestroyContext;
LPALCGETCURRENTCONTEXT   g_alcGetCurrentContext;
LPALCCLOSEDEVICE         g_alcCloseDevice;
LPALCGETINTEGERV         g_alcGetIntegerv;
LPALCGETERROR            g_alcGetError;
LPALCGETSTRING           g_alcGetString;

HMODULE g_openALLib = NULL;

template <typename TFunction>
TFunction resolveSymbol( const char *symbol )
{
	TFunction result = (TFunction) GetProcAddress( g_openALLib, symbol );
	if( !result )
	{
		throw OpenAL::Failure( "Failed to load OpenAL library, reason: " + getWin32ErrorMessage() );
	}
	return result;
}

inline void throwIfNotLoaded()
{
	if( !g_openALLib )
	{
		throw OpenAL::Failure( "OpenAL library not loaded" );
	}
}

QString getWin32ErrorMessage()
{
	wchar_t *string = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL,
				  GetLastError(),
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPWSTR)&string,
				  0,
				  NULL);
	QString message = QString::fromWCharArray( string );
	LocalFree( (HLOCAL)string );
	return message;
}

void testForALError( const char *funcName )
{
	ALenum err = OpenAL::Proxies::alGetError();
	if( err != AL_NO_ERROR )
	{
		throw OpenAL::Failure( QString( "%1() failed, err=%2, text=%3" ).arg( funcName ).arg( err ).arg( OpenAL::Proxies::alGetString( err ) ) );
	}
}

void testForALCError( ALCdevice *device, const char *funcName )
{
	ALCenum err = OpenAL::Proxies::alcGetError( device );
	if( err != ALC_NO_ERROR )
	{
		throw OpenAL::Failure( QString( "%1() failed, err=%2, text=%3" ).arg( funcName ).arg( err ).arg( OpenAL::Proxies::alcGetString( device, err ) ) );
	}
}

}

namespace OpenAL
{

namespace Proxies
{

void loadLib()
{
	if( !g_openALLib )
	{
		Log::info() << "Loading OpenAL library";
		#if defined( _WIN64 )
		QString libName = "OpenAL64";
		#else
		QString libName = "OpenAL32";
		#endif
		g_openALLib = LoadLibraryEx( (wchar_t*)libName.utf16(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS );
		if( !g_openALLib )
		{
			throw OpenAL::Failure( "Failed to load OpenAL library, reason: " + getWin32ErrorMessage() );
		}

		g_alBufferData           = resolveSymbol<LPALBUFFERDATA>( "alBufferData" );
		g_alDeleteBuffers        = resolveSymbol<LPALDELETEBUFFERS>( "alDeleteBuffers" );
		g_alDeleteSources        = resolveSymbol<LPALDELETESOURCES>( "alDeleteSources" );
		g_alGenBuffers           = resolveSymbol<LPALGENBUFFERS>( "alGenBuffers" );
		g_alGenSources           = resolveSymbol<LPALGENSOURCES>( "alGenSources" );
		g_alGetError             = resolveSymbol<LPALGETERROR>( "alGetError" );
		g_alGetSourcei           = resolveSymbol<LPALGETSOURCEI>( "alGetSourcei" );
		g_alGetString            = resolveSymbol<LPALGETSTRING>( "alGetString" );
		g_alSourcePlay           = resolveSymbol<LPALSOURCEPLAY>( "alSourcePlay" );
		g_alSourceStop           = resolveSymbol<LPALSOURCESTOP>( "alSourceStop" );
		g_alSourceQueueBuffers   = resolveSymbol<LPALSOURCEQUEUEBUFFERS>( "alSourceQueueBuffers" );
		g_alListener3f           = resolveSymbol<LPALLISTENER3F>( "alListener3f" );
		g_alListenerf            = resolveSymbol<LPALLISTENERF>( "alListenerf" );
		g_alListenerfv           = resolveSymbol<LPALLISTENERFV>( "alListenerfv" );
		g_alSource3f             = resolveSymbol<LPALSOURCE3F>( "alSource3f" );
		g_alSourcef              = resolveSymbol<LPALSOURCEF>( "alSourcef" );
		g_alSourcei              = resolveSymbol<LPALSOURCEI>( "alSourcei" );
		g_alSourceUnqueueBuffers = resolveSymbol<LPALSOURCEUNQUEUEBUFFERS>( "alSourceUnqueueBuffers" );
		g_alcOpenDevice          = resolveSymbol<LPALCOPENDEVICE>( "alcOpenDevice" );
		g_alcCreateContext       = resolveSymbol<LPALCCREATECONTEXT>( "alcCreateContext" );
		g_alcSetThreadContext    = resolveSymbol<LPALCMAKECONTEXTCURRENT>( "alcSetThreadContext" );
		g_alcDestroyContext      = resolveSymbol<LPALCDESTROYCONTEXT>( "alcDestroyContext" );
		g_alcGetCurrentContext   = resolveSymbol<LPALCGETCURRENTCONTEXT>( "alcGetCurrentContext" );
		g_alcCloseDevice         = resolveSymbol<LPALCCLOSEDEVICE>( "alcCloseDevice" );
		g_alcGetIntegerv         = resolveSymbol<LPALCGETINTEGERV>( "alcGetIntegerv" );
		g_alcGetError            = resolveSymbol<LPALCGETERROR>( "alcGetError" );
		g_alcGetString           = resolveSymbol<LPALCGETSTRING>( "alcGetString" );
	}
}

void unloadLib()
{
	if( g_openALLib )
	{
		Log::info() << "Unloading OpenAL library";
		if( FreeLibrary( g_openALLib ) == FALSE )
		{
			throw OpenAL::Failure( "Failed to unload OpenAL library, reason: " + getWin32ErrorMessage() );
		}
		g_openALLib = NULL;
	}
}

const ALchar *alGetString( ALenum param )
{
	throwIfNotLoaded();
	return g_alGetString( param );
}

ALenum alGetError()
{
	throwIfNotLoaded();
	return g_alGetError();
}

void alListenerf( ALenum param, ALfloat value )
{
	throwIfNotLoaded();
	g_alListenerf( param, value );
	testForALError( "alListenerf" );
}

void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 )
{
	throwIfNotLoaded();
	g_alListener3f( param, value1, value2, value3 );
	testForALError( "alListener3f" );
}

void alListenerfv( ALenum param, const ALfloat *values )
{
	throwIfNotLoaded();
	g_alListenerfv( param, values );
	testForALError( "alListenerfv" );
}

void alGenSources( ALsizei n, ALuint *sources )
{
	throwIfNotLoaded();
	g_alGenSources( n, sources );
	testForALError( "alGenSources" );
}

void alDeleteSources( ALsizei n, const ALuint *sources )
{
	throwIfNotLoaded();
	g_alDeleteSources( n, sources );
	testForALError( "alDeleteSources" );
}

void alSourcef( ALuint source, ALenum param, ALfloat value )
{
	throwIfNotLoaded();
	g_alSourcef( source, param, value );
	testForALError( "alSourcef" );
}

void alSource3f( ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 )
{
	throwIfNotLoaded();
	g_alSource3f( source, param, value1, value2, value3 );
	testForALError( "alSource3f" );
}

void alSourcei( ALuint source, ALenum param, ALint value )
{
	throwIfNotLoaded();
	g_alSourcei( source, param, value );
	testForALError( "alSourcei" );
}

void alGetSourcei( ALuint source, ALenum param, ALint *value )
{
	throwIfNotLoaded();
	g_alGetSourcei( source, param, value );
}

void alBufferData( ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq )
{
	throwIfNotLoaded();
	g_alBufferData( buffer, format, data, size, freq );
	testForALError( "alBufferData" );
}

void alDeleteBuffers( ALsizei n, const ALuint *buffers )
{
	throwIfNotLoaded();
	g_alDeleteBuffers( n, buffers );
	testForALError( "alDeleteBuffers" );
}

void alGenBuffers( ALsizei n, ALuint *buffers )
{
	throwIfNotLoaded();
	g_alGenBuffers( n, buffers );
	testForALError( "alGenBuffers" );
}

void alSourceUnqueueBuffers( ALuint source, ALsizei nb, ALuint *buffers )
{
	throwIfNotLoaded();
	g_alSourceUnqueueBuffers( source, nb, buffers );
	testForALError( "alSourceUnqueueBuffers" );
}

void alSourceQueueBuffers( ALuint source, ALsizei nb, const ALuint *buffers )
{
	throwIfNotLoaded();
	g_alSourceQueueBuffers( source, nb, buffers );
	testForALError( "alSourceQueueBuffers" );
}

void alSourcePlay( ALuint source )
{
	throwIfNotLoaded();
	g_alSourcePlay( source );
	testForALError( "alSourcePlay" );
}

void alSourceStop( ALuint source )
{
	throwIfNotLoaded();
	g_alSourceStop( source );
	testForALError( "alSourceStop" );
}

ALCdevice *alcOpenDevice( const ALCchar *devicename )
{
	throwIfNotLoaded();
	ALCdevice* device = g_alcOpenDevice( devicename );
	testForALCError( device, "alcOpenDevice" );
	return device;
}

ALCboolean alcCloseDevice( ALCdevice *device )
{
	throwIfNotLoaded();
	return g_alcCloseDevice( device );
}

ALCcontext *alcCreateContext( ALCdevice *device, const ALCint *attrlist )
{
	throwIfNotLoaded();
	ALCcontext *context = g_alcCreateContext( device, attrlist );
	testForALCError( device, "alcCreateContext" );
	return context;
}

void alcDestroyContext( ALCcontext *context )
{
	throwIfNotLoaded();
	g_alcDestroyContext( context );
}

ALCcontext *alcGetCurrentContext()
{
	throwIfNotLoaded();
	return g_alcGetCurrentContext();
}

ALCboolean alcSetThreadContext( ALCcontext *context )
{
	throwIfNotLoaded();
	return g_alcSetThreadContext( context );
}

void alcGetIntegerv( ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values )
{
	throwIfNotLoaded();
	g_alcGetIntegerv( device, param, size, values );
	testForALCError( device, "alcGetIntegerv" );
}

ALCenum alcGetError( ALCdevice *device )
{
	throwIfNotLoaded();
	return g_alcGetError( device );
}

const ALCchar *alcGetString( ALCdevice *device, ALCenum param )
{
	throwIfNotLoaded();
	return g_alcGetString( device, param );
}

}

}
