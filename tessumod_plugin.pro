# TessuMod: Mod for integrating TeamSpeak into World of Tanks
# Copyright (C) 2014  Janne Hakonen
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

TARGET = tessumod_plugin
TEMPLATE = lib
QT += widgets

PLUGIN_NAME        = "TESSUMOD_NAME_HERE"
PLUGIN_DESCRIPTION = "TESSUMOD_DESCRIPTION_HERE"
PLUGIN_AUTHOR      = "TESSUMOD_AUTHOR_HERE"
PLUGIN_VERSION     = "TESSUMOD_VERSION_HERE"

QMAKE_CFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

INCLUDEPATH += include $$OUT_PWD/src

SOURCES += \
    src/ui/settingsdialog.cpp \
    src/entities/settings.cpp \
    src/entities/user.cpp \
    src/entities/vector.cpp \
    src/entities/camera.cpp \
    src/usecases/usecasefactory.cpp \
    src/usecases/usecases.cpp \
    src/storages/userstorage.cpp \
    src/storages/camerastorage.cpp \
    src/adapters/audioadapter.cpp \
    src/adapters/voicechatadapter.cpp \
    src/adapters/gamedataadapter.cpp \
    src/storages/adapterstorage.cpp \
    src/storages/settingsstorage.cpp \
    src/drivers/inisettingsfile.cpp \
    src/drivers/openalbackend.cpp \
    src/drivers/teamspeakplugin.cpp \
    src/drivers/wotconnector.cpp \
    src/adapters/uiadapter.cpp \
    src/main.cpp \
    src/utils/logging.cpp \
    src/utils/positionrotator.cpp \
    src/utils/wavfile.cpp \
	src/utils/async.cpp \
    src/entities/failures.cpp \
	src/openal/proxies.cpp \
	src/openal/openal.cpp \
    src/openal/structures.cpp \
    src/openal/privateimpl.cpp

HEADERS +=\
    src/ui/settingsdialog.h \
    src/entities/settings.h \
    src/entities/user.h \
    src/entities/vector.h \
    src/entities/camera.h \
    src/usecases/usecasefactory.h \
    src/usecases/usecases.h \
    src/interfaces/storages.h \
    src/interfaces/usecasefactory.h \
    src/interfaces/adapters.h \
    src/storages/userstorage.h \
    src/storages/camerastorage.h \
    src/interfaces/drivers.h \
    src/adapters/audioadapter.h \
    src/adapters/voicechatadapter.h \
    src/adapters/gamedataadapter.h \
    src/storages/adapterstorage.h \
    src/storages/settingsstorage.h \
    src/drivers/inisettingsfile.h \
    src/drivers/openalbackend.h \
    src/drivers/teamspeakplugin.h \
    src/drivers/wotconnector.h \
    src/adapters/uiadapter.h \
    src/entities/enums.h \
    src/utils/logging.h \
    src/utils/positionrotator.h \
    src/utils/wavfile.h \
	src/utils/async.h \
	src/entities/failures.h \
	src/openal/proxies.h \
	src/openal/openal.h \
    src/openal/structures.h \
    src/openal/privateimpl.h

FORMS += \
    src/ui/settingsdialog.ui

QMAKE_SUBSTITUTES += src/config.h.in

DISTFILES += \
    src/config.h.in
