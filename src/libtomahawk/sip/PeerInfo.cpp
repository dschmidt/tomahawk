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

#include "PeerInfo.h"
#include "SipPlugin.h"

PeerInfo::PeerInfo( SipPlugin* parent, const QString& id ): QObject( parent )
{
    m_id = id;
}



PeerInfo::~PeerInfo()
{

}


const
QString PeerInfo::id() const
{
    return m_id;
}



SipPlugin*
PeerInfo::sipPlugin() const
{
    return qobject_cast< SipPlugin* >( parent() );
}


void
PeerInfo::setSipInfo( const SipInfo& sipInfo )
{
    m_sipInfo = sipInfo;

    tLog() << "id: " << id() << " info changed" << sipInfo;
    emit sipInfoChanged();
}


const SipInfo
PeerInfo::sipInfo() const
{
    return m_sipInfo;
}


void
PeerInfo::setFriendlyName( const QString& friendlyName )
{
    m_friendlyName = friendlyName;
}


const QString
PeerInfo::friendlyName() const
{
    return m_friendlyName;
}


void
PeerInfo::setAvatar( const QPixmap& avatar )
{
    m_avatar = avatar;
}


const QPixmap
PeerInfo::avatar() const
{
    return m_avatar;
}


void
PeerInfo::setVersionInfo( const QString& versionInfo )
{
    m_versionInfo = versionInfo;
}


const QString
PeerInfo::versionInfo() const
{
    return m_versionInfo;
}

// QDebug
// operator<<( QDebug dbg, const PeerInfo& info )
// {
//
// }
