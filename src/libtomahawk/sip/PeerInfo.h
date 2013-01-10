/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012, Dominik Schmidt <dev@dominik-schmidt.de>
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

#ifndef PEERINFO_H
#define PEERINFO_H



#include "DllMacro.h"

#include "SipInfo.h"
#include "accounts/Account.h"
#include "utils/TomahawkUtils.h"

#include <QString>
#include <QPixmap>


#define peerInfoDebugNumber(peerInfo,n) tLog() << "PEERINFO" << n <<":" << ( !peerInfo.isNull() ? peerInfo->debugName() : "Invalid PeerInfo" ).toLatin1().constData()
#define peerInfoDebug(peerInfo) peerInfoDebugNumber(peerInfo,1)
#define peerInfoDebug2(peerInfo) peerInfoDebugNumber(peerInfo,2)

class SipPlugin;


namespace Tomahawk
{

class DLLEXPORT PeerInfo : public QObject
{
Q_OBJECT

public:
    enum Status
    {
        Online,
        Offline
    };

    enum GetOptions
    {
        None,
        AutoCreate
    };

    static Tomahawk::peerinfo_ptr get( SipPlugin* parent, const QString& id, GetOptions options = None);
    virtual ~PeerInfo();

    QWeakPointer< Tomahawk::PeerInfo > weakRef();
    void setWeakRef( QWeakPointer< Tomahawk::PeerInfo > weakRef );

    const QString id() const;
    SipPlugin* sipPlugin() const;
    const QString debugName() const;

    void setStatus( Status status );
    Status status() const;

    void setSipInfo( const SipInfo& sipInfo );
    const SipInfo sipInfo() const;

    void setFriendlyName( const QString& friendlyName );
    const QString friendlyName() const;

    void setAvatar( const QPixmap& avatar );
    const QPixmap avatar( TomahawkUtils::ImageMode style = TomahawkUtils::Original, const QSize& size = QSize() ) const;

    void setVersionString( const QString& versionString );
    const QString versionString() const;

signals:
    void sipInfoChanged();

private:
    PeerInfo( SipPlugin* parent, const QString& id );

    static QHash< QString, peerinfo_ptr > s_peersByCacheKey;
    QWeakPointer< Tomahawk::PeerInfo > m_ownRef;

    QString m_id;
    Status  m_status;
    SipInfo m_sipInfo;
    QString m_friendlyName;
    QString m_versionString;

    mutable QPixmap* m_avatar;
    mutable QPixmap* m_fancyAvatar;
    mutable QByteArray m_avatarHash;
    mutable bool m_avatarUpdated;
    mutable QHash< TomahawkUtils::ImageMode, QHash< int, QPixmap > > m_coverCache;
};


} // ns


#endif // PEERINFO_H
