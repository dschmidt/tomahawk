#include "jabber.h"

#include "tomahawksettings.h"

#include <QtPlugin>


void
JabberPlugin::setProxy( QNetworkProxy* proxy )
{
    p->setProxy( proxy );
}


bool
JabberPlugin::connect()
{
    if ( !TomahawkSettings::instance()->value( "jabber/autoconnect", true ).toBool() )
        return false;

    QString jid       = TomahawkSettings::instance()->value( "jabber/username"   ).toString();
    QString server    = TomahawkSettings::instance()->value( "jabber/server"     ).toString();
    QString password  = TomahawkSettings::instance()->value( "jabber/password"   ).toString();
    unsigned int port = TomahawkSettings::instance()->value( "jabber/port", 5222 ).toUInt();

    // gtalk check
    if( server.isEmpty() && ( jid.contains( "@gmail.com" ) || jid.contains( "@googlemail.com" ) ) )
    {
        qDebug() << "Setting jabber server to talk.google.com";
        server = "talk.google.com";
    }

    if ( port < 1 || port > 65535 || jid.isEmpty() || password.isEmpty() )
    {
        qDebug() << "Jabber credentials look wrong, not connecting";
        return false;
    }

    delete p;
    p = new Jabber_p( jid, password, server, port );

    QObject::connect( p, SIGNAL( peerOnline( QString ) ), SIGNAL( peerOnline( QString ) ) );
    QObject::connect( p, SIGNAL( peerOffline( QString ) ), SIGNAL( peerOffline( QString ) ) );
    QObject::connect( p, SIGNAL( msgReceived( QString, QString ) ), SIGNAL( msgReceived( QString, QString ) ) );

    QObject::connect( p, SIGNAL( connected() ), SIGNAL( connected() ) );
    QObject::connect( p, SIGNAL( disconnected() ), SIGNAL( disconnected() ) );
    QObject::connect( p, SIGNAL( authError( int, QString ) ), SLOT( onAuthError( int, QString ) ) );

    p->go();

    return true;
}


void
JabberPlugin::onAuthError( int code, const QString& message )
{
    if ( code == gloox::ConnAuthenticationFailed )
        emit error( SipPlugin::AuthError, message );
    else
        emit error( SipPlugin::ConnectionError, message );
}

Q_EXPORT_PLUGIN2( sip, JabberPlugin )
