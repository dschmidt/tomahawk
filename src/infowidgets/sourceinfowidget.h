#ifndef SOURCEINFOWIDGET_H
#define SOURCEINFOWIDGET_H

#include <QWidget>

#include "album.h"
#include "result.h"
#include "playlistinterface.h"

class AlbumModel;
class CollectionFlatModel;

namespace Ui
{
    class SourceInfoWidget;
}

class SourceInfoWidget : public QWidget
{
Q_OBJECT

public:
    SourceInfoWidget( const Tomahawk::source_ptr& source, QWidget* parent = 0 );
    ~SourceInfoWidget();

protected:
    void changeEvent( QEvent* e );

private:
    Ui::SourceInfoWidget *ui;

    CollectionFlatModel* m_recentCollectionModel;
    AlbumModel* m_recentAlbumModel;
};

#endif // SOURCEINFOWIDGET_H
