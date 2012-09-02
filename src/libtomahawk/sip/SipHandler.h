/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *   Copyright 2010-2011, Jeff Mitchell <jeff@tomahawk-player.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIPHANDLER_H
#define SIPHANDLER_H

#include "sip/SipPlugin.h"
#include "DllMacro.h"

#include <QObject>
#include <QHash>
#include <QString>

#ifndef ENABLE_HEADLESS
    #include <QPixmap>
#endif

/**
 * Manages SIP plugins for connecting to friends. External interface to SIP plugins is
 * through AccountManager, this is an internal class.
 */

class DLLEXPORT SipHandler : public QObject
{
    Q_OBJECT

public:
    static SipHandler* instance();

    SipHandler( QObject* parent );
    ~SipHandler();

    void loadFromAccountManager();

    void hookUpPlugin( SipPlugin* p );
private:
    void handleSipInfo( PeerInfo* );

private slots:
    void onPeerOffline( PeerInfo* );
    void onPeerOnline( PeerInfo* );

    void onSipInfoChanged();

#ifndef ENABLE_HEADLESS
    // set data for local source
    void onAvatarReceived( const QPixmap& avatar );
#endif

private:
    static SipHandler *s_instance;
};

#endif
