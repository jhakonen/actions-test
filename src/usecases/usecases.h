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

#pragma once

#include "../interfaces/storages.h"
#include "../interfaces/adapters.h"

#include <QObject>

namespace Entity
{
class Vector;
}

namespace UseCase
{

class UseCases : public QObject
{
	Q_OBJECT

public:
	typedef std::function<void(QVariant)> Callback;

	void applicationInitialize();
	void positionUser( quint16 id, const Entity::Vector& position );
	void positionCamera( const Entity::Vector& position, const Entity::Vector& direction );
	void addGameUser( quint16 id );
	void removeGameUser( quint16 id );
	void addChatUser( quint16 id );
	void removeChatUser( quint16 id );
	void changePlaybackDevice();
	void changePlaybackVolume();
	void showSettingsUi( QWidget *parent );
	void saveSettings( const Entity::Settings &settings );
	void playTestAudioWithSettings(const Entity::Settings &settings, Callback callback );
	void showPluginHelp();

private:
	void positionUserToAudioBackends( const Entity::User &user );
	void removeUserFromAudioBackends( const Entity::User &user );
	void updatePlaybackDeviceToBackends();
	void updatePlaybackVolumeToBackends();

public:
	Interfaces::UserStorage* userStorage;
	Interfaces::CameraStorage* cameraStorage;
	Interfaces::SettingsStorage* settingsStorage;
	Interfaces::AdapterStorage* adapterStorage;
};

}
