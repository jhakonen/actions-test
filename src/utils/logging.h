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

#include <QString>

class QFile;

namespace Entity
{
class Vector;
}

namespace Log
{

class Stream;
class Sink;

enum Severity
{
	Debug,
	Info,
	Warning,
	Error
};

Stream debug( const char *channel = "TessuMod Plugin" );
Stream info( const char *channel = "TessuMod Plugin" );
Stream warning( const char *channel = "TessuMod Plugin" );
Stream error( const char *channel = "TessuMod Plugin" );

void setSink( Sink *sink );

void logQtMessages();

class Sink
{
public:
	virtual ~Sink() {}
	virtual void logMessage( const QString &message, const char *channel, Severity severity ) = 0;
};

class FileLogger : public Sink
{
public:
	FileLogger( const QString &filepath );
	~FileLogger();

	void logMessage( const QString &message, const char *channel, Severity severity );

private:
	QFile *logFile;
};

class Stream
{
public:
	~Stream();
	const char *channel;
	QString message;
	Severity severity;
};

Stream& operator<<( Stream &stream, const QString &value );
Stream& operator<<( Stream &stream, quint16 value );
Stream& operator<<( Stream &stream, const Entity::Vector &value );

}
