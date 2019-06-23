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

#include "usecases.h"
#include "../entities/user.h"
#include "../entities/camera.h"
#include "../entities/settings.h"
#include "../entities/failures.h"
#include "../utils/logging.h"
#include <QList>
#include <QString>
#include <QVariant>
#include <QSet>
#include <QDesktopServices>
#include <QUrl>

namespace {

bool notInGameOrChat( const Entity::User &user )
{
	return !user.inGame && !user.inChat;
}

}

namespace UseCase
{

void UseCases::applicationInitialize()
{
	Entity::Settings settings = settingsStorage->get();
	if( settings.positioningEnabled )
	{
		updatePlaybackDeviceToBackends();
		updatePlaybackVolumeToBackends();

		foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
		{
			backend->setHrtfEnabled( settings.hrtfEnabled );
			backend->setHrtfDataSet( settings.hrtfDataSet );
			backend->setLoggingLevel( settings.audioLoggingLevel );
		}

		adapterStorage->getAudio( settings.audioBackend )->setEnabled( true );
	}
	deleteLater();
}

void UseCases::positionUser( quint16 id, const Entity::Vector& position )
{
	if( userStorage->has( id ) )
	{
		Entity::User user = userStorage->get( id );
		user.position = position;
		userStorage->set( user );
		positionUserToAudioBackends( user );
	}
	deleteLater();
}

void UseCases::positionCamera( const Entity::Vector& position, const Entity::Vector& direction )
{
	Entity::Camera camera = cameraStorage->get();
	camera.position = position;
	camera.direction = direction;
	cameraStorage->set( camera );
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
	{
		backend->positionCamera( camera );
	}
	deleteLater();
}

void UseCases::addGameUser( quint16 id )
{
	if( adapterStorage->getVoiceChat()->getMyUserId() == id )
	{
		return;
	}
	Entity::User user;
	if( userStorage->has( id ) )
	{
		user = userStorage->get( id );
	}
	else
	{
		user.id = id;
	}
	user.inGame = true;
	userStorage->set( user );
	positionUserToAudioBackends( user );
	deleteLater();
}

void UseCases::removeGameUser( quint16 id )
{
	if( !userStorage->has( id ) )
	{
		return;
	}
	Entity::User user = userStorage->get( id );
	removeUserFromAudioBackends( user );
	user.inGame = false;
	if( !user.exists() )
	{
		userStorage->remove( id );
	}
	deleteLater();
}

void UseCases::addChatUser( quint16 id )
{
	Entity::User user;
	Entity::Settings settings = settingsStorage->get();
	if( userStorage->has( id ) )
	{
		user = userStorage->get( id );
	}
	else
	{
		user.id = id;
	}
	user.inChat = true;
	userStorage->set( user );
	positionUserToAudioBackends( user );
	deleteLater();
}

void UseCases::removeChatUser( quint16 id )
{
	if( !userStorage->has( id ) )
	{
		return;
	}
	Entity::User user = userStorage->get( id );
	removeUserFromAudioBackends( user );
	user.inChat = false;
	if( !user.exists() )
	{
		userStorage->remove( id );
	}
	deleteLater();
}

void UseCases::changePlaybackDevice()
{
	updatePlaybackDeviceToBackends();
	deleteLater();
}

void UseCases::changePlaybackVolume()
{
	updatePlaybackVolumeToBackends();
	deleteLater();
}

void UseCases::showSettingsUi( QWidget *parent )
{
	Entity::Settings settings = settingsStorage->get();
	QStringList hrtfDataNames;
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
	{
		hrtfDataNames.append( backend->getHrtfDataFileNames() );
	}
	adapterStorage->getUi()->showSettingsUi( settings, hrtfDataNames, parent );
	deleteLater();
}

void UseCases::saveSettings( const Entity::Settings &settings )
{
	Entity::Settings originalSettings = settingsStorage->get();
	settingsStorage->set( settings );
	if( settings.positioningEnabled )
	{
		if( originalSettings.audioBackend != settings.audioBackend )
		{
			adapterStorage->getAudio( originalSettings.audioBackend )->setEnabled( false );
		}
		foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
		{
			backend->setHrtfEnabled( settings.hrtfEnabled );
			backend->setHrtfDataSet( settings.hrtfDataSet );
			backend->setLoggingLevel( settings.audioLoggingLevel );
		}
		adapterStorage->getAudio( settings.audioBackend )->setEnabled( true );
	}
	else
	{
		// positioningEnabled: true --> false ?
		if( originalSettings.positioningEnabled )
		{
			adapterStorage->getAudio( originalSettings.audioBackend )->setEnabled( false );
		}
	}
	deleteLater();
}

void UseCases::playTestAudioWithSettings(const Entity::Settings &settings, Callback callback )
{
	Interfaces::AudioAdapter *backend = adapterStorage->getTestAudio( settings.audioBackend );
	backend->setHrtfEnabled( settings.hrtfEnabled );
	backend->setHrtfDataSet( settings.hrtfDataSet );
	backend->setLoggingLevel( settings.audioLoggingLevel );
	backend->setEnabled( settings.positioningEnabled );
	backend->playTestSound( settings.testRotateMode, [=]( QVariant result ) {
		deleteLater();
		callback( result );
	} );
}

void UseCase::UseCases::showPluginHelp()
{
	QDesktopServices::openUrl( QUrl( "https://github.com/jhakonen/wot-teamspeak-mod/wiki/TeamSpeak-Plugins#tessumod-plugin" ) );
}

void UseCases::positionUserToAudioBackends( const Entity::User &user )
{
	if( user.paired() )
	{
		foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
		{
			backend->positionUser( user );
		}
	}
}

void UseCases::removeUserFromAudioBackends( const Entity::User &user )
{
	if( user.paired() )
	{
		foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
		{
			backend->removeUser( user );
		}
	}
}

void UseCases::updatePlaybackDeviceToBackends()
{
	auto deviceName = adapterStorage->getVoiceChat()->getPlaybackDeviceName();
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
	{
		backend->setPlaybackDeviceName( deviceName );
	}
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getTestAudios() )
	{
		backend->setPlaybackDeviceName( deviceName );
	}
}

void UseCases::updatePlaybackVolumeToBackends()
{
	auto volume = adapterStorage->getVoiceChat()->getPlaybackVolume();
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getAudios() )
	{
		backend->setPlaybackVolume( volume );
	}
	foreach( Interfaces::AudioAdapter *backend, adapterStorage->getTestAudios() )
	{
		backend->setPlaybackVolume( volume );
	}
}

}
