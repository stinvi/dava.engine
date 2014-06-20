#include "BeastDialog.h"

#include <QEventLoop>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "Classes/Qt/Scene/SceneEditor2.h"


BeastDialog::BeastDialog( QWidget *parent )
    : QWidget( parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint )
    , ui(new Ui::BeastDialog())
    , scene(NULL)
    , result(false)
    , beastMode(BeastProxy::MODE_LIGHTMAPS)
{
    ui->setupUi(this);

    setWindowModality( Qt::WindowModal );

    connect( ui->start, SIGNAL( clicked() ), SLOT( OnStart() ) );
    connect( ui->cancel, SIGNAL( clicked() ), SLOT( OnCancel() ) );
    connect( ui->browse, SIGNAL( clicked() ), SLOT( OnBrowse() ) );
    connect( ui->output, SIGNAL( textChanged( const QString& ) ), SLOT( OnTextChanged() ) );
    connect( ui->lightmapBeastModeButton, SIGNAL( clicked( bool ) ), SLOT( OnLightmapMode( bool ) ) );
    connect( ui->shBeastModeButton, SIGNAL( clicked( bool ) ), SLOT( OnSHMode( bool ) ) );
}

BeastDialog::~BeastDialog()
{
}

void BeastDialog::SetScene(SceneEditor2 *_scene)
{
    scene = _scene;
}

bool BeastDialog::Exec(QWidget *parent)
{
    DVASSERT(scene);

    ui->scenePath->setText( QDir::toNativeSeparators( GetDefaultPath() ) );
    ui->output->setText( "lightmaps" );

    show();

    loop = new QEventLoop( this );
    loop->exec();

    hide();
    loop->deleteLater();

    return result;
}

void BeastDialog::OnStart()
{
    const QDir dir( ui->scenePath->text() );
    dir.mkpath( ui->output->text() );
    if ( !dir.exists( ui->output->text() ) )
    {
        result = false;
        QMessageBox::warning( this, QString(), "Specified path is invalid. Couldn't create output directory." );
        return;
    }

    result = true;
    loop->quit();
}

void BeastDialog::OnCancel()
{
    result = false;
    loop->quit();
}

void BeastDialog::OnBrowse()
{
    const QString path = QFileDialog::getExistingDirectory( this, QString(), GetPath(), QFileDialog::ShowDirsOnly );
    if ( !path.isEmpty() )
    {
        SetPath( path );
    }
}

void BeastDialog::OnTextChanged()
{
    const QString text = QString("Output path: %1").arg( QDir::toNativeSeparators( GetPath() ) );
    ui->output->setToolTip( text );
}

void BeastDialog::OnLightmapMode(bool checked)
{
    if(checked)
    {
        ui->foldersWidget->setDisabled(false);
        beastMode = BeastProxy::MODE_LIGHTMAPS;
    }
}

void BeastDialog::OnSHMode(bool checked)
{
    if(checked)
    {
        ui->foldersWidget->setDisabled(true);
        beastMode = BeastProxy::MODE_SPHERICAL_HARMONICS;
    }
}

void BeastDialog::closeEvent( QCloseEvent * event )
{
    Q_UNUSED(event);
    ui->cancel->click();
}

QString BeastDialog::GetPath() const
{
    const QString root = ui->scenePath->text();
    const QString output = ui->output->text();
    if (output.isEmpty())
        return QString();

    QString path = QString("%1/%2/").arg(root).arg(output);
    return path;
}

BeastProxy::eBeastMode BeastDialog::GetMode() const
{
    return beastMode;
}

QString BeastDialog::GetDefaultPath() const
{
    if ( !scene )
        return QString();

    const QString absoluteFilePath = scene->GetScenePath().GetAbsolutePathname().c_str();
    const QFileInfo fileInfo( absoluteFilePath );
    const QString dir = fileInfo.absolutePath();

    return dir;
}

void BeastDialog::SetPath( const QString& path )
{
    const QString mandatory = QDir::fromNativeSeparators( GetDefaultPath() );
    const QString pathRoot = path.left(mandatory.length());
    if ( QDir( mandatory + '/' ) != QDir( pathRoot + '/' ) )
    {
        ui->output->setText( QString() );
        return;
    }

    const QString relative = path.mid(mandatory.length() + 1);
    ui->output->setText( QDir::toNativeSeparators( relative ) );
}
