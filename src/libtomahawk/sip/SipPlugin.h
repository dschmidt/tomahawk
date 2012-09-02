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

#ifndef SIPPLUGIN_H
#define SIPPLUGIN_H

#include "PeerInfo.h"
#include "SipInfo.h"
#include "accounts/Account.h"

#include "DllMacro.h"

#include <QObject>
#include <QString>
#include <QNetworkProxy>

#ifndef ENABLE_HEADLESS
    #include <QMenu>
#endif


class DLLEXPORT SipPlugin : public QObject
{
    Q_OBJECT

public:
    SipPlugin();
    explicit SipPlugin( Tomahawk::Accounts::Account *account, QObject* parent = 0 );
    virtual ~SipPlugin();

    // plugin id is "pluginfactoryname_someuniqueid".  get it from SipPluginFactory::generateId
    QString pluginId() const;

    virtual bool isValid() const = 0;
    virtual const QString friendlyName() const;
    virtual const QString serviceName() const;
    virtual QString inviteString() const;
#ifndef ENABLE_HEADLESS
    virtual QMenu* menu();
#endif
    virtual Tomahawk::Accounts::Account* account() const;

    // peer infos
    virtual const QList< PeerInfo* > peersOnline() const;

public slots:
    virtual void connectPlugin() = 0;
    virtual void disconnectPlugin() = 0;
    virtual void checkSettings() = 0;
    virtual void configurationChanged() = 0;

    virtual void sendSipInfo( PeerInfo* receiver, const SipInfo& info ) = 0;
    virtual void addContact( const QString& jid, const QString& msg = QString() ){};

signals:
    void peerOnline( PeerInfo* );
    void peerOffline( PeerInfo* );

    void dataError( bool );

#ifndef ENABLE_HEADLESS
    // new data for own source
    void avatarReceived ( const QPixmap& avatar );

    void addMenu( QMenu* menu );
    void removeMenu( QMenu* menu );
#endif

protected:
    PeerInfo* peerInfoForId( const QString& id );

private slots:
    void onPeerOnline( PeerInfo* peerInfo );
    void onPeerOffline( PeerInfo* peerInfo );

protected:
    Tomahawk::Accounts::Account *m_account;

private:
    QList< PeerInfo* > m_peersOnline;
};

#endif
