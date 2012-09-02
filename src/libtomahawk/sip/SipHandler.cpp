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

#include "SipHandler.h"
#include "sip/SipPlugin.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#ifndef ENABLE_HEADLESS
    #include <QMessageBox>
#endif

#include "FuncTimeout.h"

#include "database/Database.h"
#include "database/DatabaseImpl.h"
#include "network/ControlConnection.h"
#include "network/Servent.h"
#include "SourceList.h"
#include "TomahawkSettings.h"
#include "utils/Logger.h"
#include "accounts/AccountManager.h"

#include "config.h"

SipHandler* SipHandler::s_instance = 0;


SipHandler*
SipHandler::instance()
{
    if ( !s_instance )
        new SipHandler( 0 );

    return s_instance;
}


SipHandler::SipHandler( QObject* parent )
    : QObject( parent )
{
    s_instance = this;
}


SipHandler::~SipHandler()
{
    qDebug() << Q_FUNC_INFO;
    s_instance = 0;
}


void
SipHandler::hookUpPlugin( SipPlugin* sip )
{
    QObject::connect( sip, SIGNAL( peerOnline( PeerInfo* ) ), SLOT( onPeerOnline( PeerInfo* ) ) );
    QObject::connect( sip, SIGNAL( peerOffline( PeerInfo* ) ), SLOT( onPeerOffline( PeerInfo* ) ) );


    QObject::connect( sip, SIGNAL( avatarReceived( QPixmap ) ), SLOT( onAvatarReceived( QPixmap ) ) );

    QObject::connect( sip->account(), SIGNAL( configurationChanged() ), sip, SLOT( configurationChanged() ) );
}


void
SipHandler::onPeerOnline( PeerInfo* peerInfo )
{
    QString peerId = peerInfo->id();

//    qDebug() << Q_FUNC_INFO;
    tDebug() << "SIP online:" << peerId;

    SipPlugin* sip = qobject_cast< SipPlugin* >( sender() );

    SipInfo info;
    if( Servent::instance()->visibleExternally() )
    {
        QString key = uuid();
        ControlConnection* conn = new ControlConnection( Servent::instance() );
        conn->addPeerInfo( peerInfo );

        const QString& nodeid = Database::instance()->impl()->dbid();
        conn->setName( peerId.left( peerId.indexOf( "/" ) ) );
        conn->setId( nodeid );

        Servent::instance()->registerOffer( key, conn );
        info.setVisible( true );
        info.setHost( Servent::instance()->externalAddress() );
        info.setPort( Servent::instance()->externalPort() );
        info.setKey( key );
        info.setUniqname( nodeid );

        tDebug() << "Asking them to connect to us:" << info;
    }
    else
    {
        info.setVisible( false );
        tDebug() << "We are not visible externally:" << info;
    }

    sip->sendSipInfo( peerInfo, info );
    handleSipInfo( peerInfo );
    connect(peerInfo, SIGNAL(sipInfoChanged()), SLOT(onSipInfoChanged()));
}


void
SipHandler::onPeerOffline( PeerInfo* peerInfo )
{
//    qDebug() << Q_FUNC_INFO;
    tDebug() << "SIP offline:" << peerInfo->id();
}


void
SipHandler::onSipInfoChanged()
{
    PeerInfo* peerInfo = qobject_cast< PeerInfo* >( sender() );

    if( !peerInfo )
        return;

    handleSipInfo( peerInfo );
}


void
SipHandler::handleSipInfo( PeerInfo* peerInfo )
{
    SipInfo info = peerInfo->sipInfo();

    if( !info.isValid() )
        return;

//     QString barePeerId = peerId.left( peerId.indexOf( "/" ) );

    //FIXME: We should probably be using barePeerId in the connectToPeer call below.
    //But, verify this doesn't cause any problems (there is still a uniquename after all)

    /*
        If only one party is externally visible, connection is obvious
        If both are, peer with lowest IP address initiates the connection.
        This avoids dupe connections.
        */
    if ( info.isVisible() )
    {
            if( !Servent::instance()->visibleExternally() ||
            Servent::instance()->externalAddress() < info.host() ||
            ( Servent::instance()->externalAddress() == info.host() && Servent::instance()->externalPort() < info.port() ) )
        {
                tDebug() << "Initiate connection to" << peerInfo->id() << "at" << info.host();
            Servent::instance()->connectToPeer( peerInfo );
        }
        else
        {
                tDebug() << Q_FUNC_INFO << "They should be conecting to us...";
        }
    }
    else
    {
            tDebug() << Q_FUNC_INFO << "They are not visible, doing nothing atm";
    }
}


#ifndef ENABLE_HEADLESS
void
SipHandler::onAvatarReceived( const QPixmap& avatar )
{
//    qDebug() << Q_FUNC_INFO << "Set own avatar on MyCollection";
//    SourceList::instance()->getLocal()->setAvatar( avatar );
}
#endif
