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
#include "utils/TomahawkCache.h"
#include "utils/TomahawkUtilsGui.h"

#include <QBuffer>

PeerInfo::PeerInfo( SipPlugin* parent, const QString& id )
    : QObject( parent )
    , m_avatarUpdated( true )
    , m_avatar( 0 )
    , m_fancyAvatar( 0 )
{
    m_id = id;
}



PeerInfo::~PeerInfo()
{
    delete m_avatar;
    delete m_fancyAvatar;
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
    if( m_friendlyName.isNull() )
        return m_id;

    return m_friendlyName;
}


void
PeerInfo::setVersionString( const QString& versionString )
{
    m_versionString = versionString;
}


const QString
PeerInfo::versionString() const
{
    return m_versionString;
}


void
PeerInfo::setAvatar( const QPixmap& avatar )
{
    delete m_avatar;
    m_avatar = new QPixmap( avatar );
    m_fancyAvatar = 0;

    QByteArray ba;
    QBuffer buffer( &ba );
    buffer.open( QIODevice::WriteOnly );
    avatar.save( &buffer, "PNG" );

    TomahawkUtils::Cache::instance()->putData( "Sources", 7776000000 /* 90 days */, m_id, ba );
}


const QPixmap
PeerInfo::avatar( TomahawkUtils::AvatarStyle style, const QSize& size ) const
{
    if ( !m_avatar && m_avatarUpdated )
    {
        m_avatar = new QPixmap();
        QByteArray ba = TomahawkUtils::Cache::instance()->getData( "Sources", m_id ).toByteArray();

        if ( ba.count() )
            m_avatar->loadFromData( ba );
        if ( m_avatar->isNull() )
        {
            delete m_avatar;
            m_avatar = 0;
        }
        m_avatarUpdated = false;
    }

    if ( style == TomahawkUtils::AvatarStyleFancy && m_avatar && !m_fancyAvatar )
        m_fancyAvatar = new QPixmap( TomahawkUtils::createAvatarFrame( QPixmap( *m_avatar ) ) );

    QPixmap pixmap;
    if ( style == TomahawkUtils::AvatarStyleOriginal && m_avatar )
        pixmap = *m_avatar;
    else if ( style == TomahawkUtils::AvatarStyleFancy && m_fancyAvatar )
        pixmap = *m_fancyAvatar;

    if ( !pixmap.isNull() && !size.isEmpty() )
    {
        if ( m_coverCache.contains( size.width() ) )
        {
            return m_coverCache.value( size.width() );
        }

        QPixmap scaledCover;
        scaledCover = pixmap.scaled( size, Qt::KeepAspectRatio, Qt::SmoothTransformation );
        m_coverCache.insert( size.width(), scaledCover );
        return scaledCover;
    }

    return pixmap;
}
