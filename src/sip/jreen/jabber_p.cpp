/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Dominik Schmidt <dev@dominik-schmidt.de>
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
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

#include "jabber_p.h"
#include "tomahawksipmessage.h"
#include "tomahawksipmessagefactory.h"

#include "config.h"
#include "utils/tomahawkutils.h"

#include <jreen/capabilities.h>
#include <jreen/vcardupdate.h>
#include <jreen/vcard.h>

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QRegExp>
#include <QThread>
#include <QVariant>
#include <QMap>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QPixmap>

//remove
#include <QLabel>
#include <QtGui/QLabel>



Jabber_p::Jabber_p( const QString& jid, const QString& password, const QString& server, const int port )
    : QObject()
    , m_server()
{

}


Jabber_p::~Jabber_p()
{
    delete m_client;
}

void
Jabber_p::setProxy( QNetworkProxy* proxy )
{
}

void
Jabber_p::disconnect()
{
    if ( m_client )
    {
        m_client->disconnect();
    }
}


void
Jabber_p::sendMsg( const QString& to, const QString& msg )
{
}


void
Jabber_p::broadcastMsg( const QString &msg )
{
}


void
Jabber_p::addContact( const QString& jid, const QString& msg )
{

}

void
Jabber_p::onConnect()
{
}


void
Jabber_p::onDisconnect(  )
{


void
Jabber_p::onNewMessage( const Jreen::Message& m )
{
}

void
Jabber_p::onNewIq( const Jreen::IQ &iq, int context )
{
    if( context == RequestDisco )
    {
        qDebug() << Q_FUNC_INFO << "Received disco IQ...";
        Jreen::Disco::Info *discoInfo = iq.findExtension<Jreen::Disco::Info>().data();
        if(!discoInfo)
            return;
        iq.accept();

        QString fulljid = iq.from().full();
        Jreen::DataForm::Ptr form = discoInfo->form();

        if(discoInfo->features().contains( TOMAHAWK_FEATURE ))
        {
            qDebug() << Q_FUNC_INFO << fulljid << "Running tomahawk/feature enabled: yes";

            // the actual presence doesn't matter, it just needs to be "online"
            handlePeerStatus( fulljid, Jreen::Presence::Available );
        }
        else
        {
            qDebug() << Q_FUNC_INFO << fulljid << "Running tomahawk/feature enabled: no";

            //LEGACY: accept resources starting with tomahawk too
            if( iq.from().resource().startsWith("tomahawk") )
            {
                qDebug() << Q_FUNC_INFO << fulljid << "Detected legacy tomahawk..";

                // add to legacy peers, so we can send text messages instead of iqs
                m_legacy_peers.append( fulljid );

                handlePeerStatus( fulljid, Jreen::Presence::Available );
            }
        }
    }
    else if(context == RequestedDisco)
    {
        qDebug() << "Sent IQ(Set), what should be happening here?";
    }
    else if(context == SipMessageSent )
    {
        qDebug() << "Sent SipMessage... what now?!";
    }
    /*else if(context == RequestedVCard )
    {
        qDebug() << "Requested VCard... what now?!";
    }*/
    else
    {

        TomahawkSipMessage *sipMessage = iq.findExtension<TomahawkSipMessage>().data();
        if(sipMessage)
        {
            iq.accept();

            qDebug() << Q_FUNC_INFO << "Got SipMessage ...";
            qDebug() << "ip" << sipMessage->ip();
            qDebug() << "port" << sipMessage->port();
            qDebug() << "uniqname" << sipMessage->uniqname();
            qDebug() << "key" << sipMessage->key();
            qDebug() << "visible" << sipMessage->visible();


            QVariantMap m;
            if( sipMessage->visible() )
            {
                m["visible"] = true;
                m["ip"] = sipMessage->ip();
                m["port"] = sipMessage->port();
                m["key"] = sipMessage->key();
                m["uniqname"] = sipMessage->uniqname();
            }
            else
            {
                m["visible"] = false;
            }


            QJson::Serializer ser;
            QByteArray ba = ser.serialize( m );
            QString msg = QString::fromAscii( ba );

            QString from = iq.from().full();
            qDebug() << Q_FUNC_INFO << "From:" << from << ":" << msg;
            emit msgReceived( from, msg );
        }
    }
}

bool
Jabber_p::presenceMeansOnline( Jreen::Presence::Type p )
{
    switch(p)
    {
        case Jreen::Presence::Invalid:
        case Jreen::Presence::Unavailable:
        case Jreen::Presence::Error:
            return false;
            break;
        default:
            return true;
    }
}

void
Jabber_p::handlePeerStatus( const Jreen::JID& jid, Jreen::Presence::Type presenceType )
{
    QString fulljid = jid.full();

    // "going offline" event
    if ( !presenceMeansOnline( presenceType ) &&
         ( !m_peers.contains( fulljid ) ||
           presenceMeansOnline( m_peers.value( fulljid ) )
         )
       )
    {
        m_peers[ fulljid ] = presenceType;
        qDebug() << Q_FUNC_INFO << "* Peer goes offline:" << fulljid;

        // remove peer from legacy peers
        if( m_legacy_peers.contains( fulljid ) )
        {
            m_legacy_peers.removeAll( fulljid );
        }

        emit peerOffline( fulljid );
        return;
    }

    // "coming online" event
    if( presenceMeansOnline( presenceType ) &&
        ( !m_peers.contains( fulljid ) ||
          !presenceMeansOnline( m_peers.value( fulljid ) )
        )
       )
    {
        m_peers[ fulljid ] = presenceType;
        qDebug() << Q_FUNC_INFO << "* Peer goes online:" << fulljid;

        emit peerOnline( fulljid );

        if(!m_avatarManager->avatar(jid.bare()).isNull())
            onNewAvatar( jid.bare() );

        return;
    }

    //qDebug() << "Updating presence data for" << fulljid;
    m_peers[ fulljid ] = presenceType;
}

void Jabber_p::onNewAvatar(const QString& jid)
{
    qDebug() << Q_FUNC_INFO << jid;
    Q_ASSERT(!m_avatarManager->avatar( jid ).isNull());

    // find peers for the jid
    QStringList peers =  m_peers.keys();
    foreach(const QString &peer, peers)
    {
        if( peer.startsWith(jid) )
        {
            emit avatarReceived ( peer,  m_avatarManager->avatar( jid ) );
        }
    }

    if( jid == m_client->jid().bare() )
        // own avatar
        emit avatarReceived ( m_avatarManager->avatar( jid ) );
    else
        // someone else's avatar
        emit avatarReceived ( jid,  m_avatarManager->avatar( jid ) );
}
