/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2011, Dominik Schmidt <dev@dominik-schmidt.de>
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
#include "utils/TomahawkUtilsGui.h"

#include <QString>
#include <QPixmap>

class SipPlugin;

class DLLEXPORT PeerInfo : public QObject
{
Q_OBJECT

public:
    PeerInfo( SipPlugin* parent, const QString& id );
    virtual ~PeerInfo();

    const QString id() const;
    SipPlugin* sipPlugin() const;

    void setSipInfo( const SipInfo& sipInfo );
    const SipInfo sipInfo() const;

    void setFriendlyName( const QString& friendlyName );
    const QString friendlyName() const;

    void setAvatar( const QPixmap& avatar );
    const QPixmap avatar( TomahawkUtils::AvatarStyle style, const QSize& size ) const;

signals:
    void sipInfoChanged();

private:
    QString m_id;

    SipInfo m_sipInfo;
    QString m_friendlyName;

    mutable QPixmap* m_avatar;
    mutable QPixmap* m_fancyAvatar;
    mutable bool m_avatarUpdated;
    mutable QHash< int, QPixmap > m_coverCache;


};

// DLLEXPORT QDebug operator<<( QDebug dbg, const PeerInfo &info );



#endif // PEERINFO_H
