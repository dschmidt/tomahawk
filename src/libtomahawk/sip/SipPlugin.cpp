/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *             2011, Dominik Schmidt <dev@dominik-schmidt.de>
 *             2010-2011, Leo Franchi <lfranchi@kde.org>
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

#include "sip/SipPlugin.h"

#include "utils/Logger.h"
#include "Source.h"


SipPlugin::SipPlugin() : QObject() {}
SipPlugin::~SipPlugin() {}

SipPlugin::SipPlugin( Tomahawk::Accounts::Account *account, QObject* parent )
    : QObject( parent )
    , m_account( account )
{
    connect( this, SIGNAL( peerOnline( PeerInfo* ) ), this, SLOT( onPeerOnline( PeerInfo* ) ) );
    connect( this, SIGNAL( peerOffline( PeerInfo* ) ), this, SLOT( onPeerOffline( PeerInfo* ) ) );
}


QString
SipPlugin::pluginId() const
{
    return m_account->accountId();
}


const QString
SipPlugin::friendlyName() const
{
    return m_account->accountFriendlyName();
}


const QString
SipPlugin::serviceName() const
{
    return m_account->accountServiceName();
}

QString
SipPlugin::inviteString() const
{
    return QString();
}


#ifndef ENABLE_HEADLESS

QMenu*
SipPlugin::menu()
{
    return 0;
}

#endif


Tomahawk::Accounts::Account*
SipPlugin::account() const
{
    return m_account;
}


const QList< PeerInfo* >
SipPlugin::peersOnline() const
{
    return m_peersOnline;
}


void
addContact( const QString& jid, const QString& msg )
{
}


void
SipPlugin::onPeerOnline( PeerInfo* peerInfo )
{
   if( !m_peersOnline.contains( peerInfo ) )
   {
       m_peersOnline.append( peerInfo );
   }
}


void
SipPlugin::onPeerOffline( PeerInfo* peerInfo )
{
    tLog() << "peer offline";
    m_peersOnline.removeAll( peerInfo );
    delete peerInfo;
}


PeerInfo* SipPlugin::peerInfoForId( const QString& id )
{
   foreach( PeerInfo* peerInfo, m_peersOnline )
    {
//         tLog() << "id: " << id << " peerInfo: " << peerInfo->id();
        if( peerInfo->id() == id )
            return peerInfo;
    }

    return 0;
}
