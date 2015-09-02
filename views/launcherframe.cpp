#include "launcherframe.h"
#include "widgets/util.h"
#include "displaymodeframe.h"
#include "apptablewidget.h"
#include "categoryframe.h"
#include "navigationbar.h"
#include "app/global.h"
#include "app/xcb_misc.h"
#include "Logger.h"
#include "searchlineedit.h"
#include "categorytablewidget.h"
#include "background/backgroundlabel.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QApplication>
#include <QCloseEvent>
#include <QDBusConnection>


QButtonGroup LauncherFrame::buttonGroup(qApp);

LauncherFrame::LauncherFrame(QWidget *parent) : QFrame(parent)
{
    LauncherFrame::buttonGroup.setExclusive(true);
    XcbMisc::instance()->setLauncher(winId());
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setGeometry(qApp->desktop()->screenGeometry());
    setObjectName("LauncherFrame");
    computerGrid(160, 60, 24, 160);
    initUI();
    initConnect();
    setStyleSheet(getQssFromFile(":/qss/skin/qss/main.qss"));
    qDebug() << qApp->desktop()->screenGeometry();
}


void LauncherFrame::initUI(){
    LOG_INFO() << "initUI";
    m_backgroundLabel = new BackgroundLabel(true, this);
    m_clearCheckedButton = new QPushButton(this);
    m_clearCheckedButton->setCheckable(true);
    m_clearCheckedButton->hide();
    LauncherFrame::buttonGroup.addButton(m_clearCheckedButton, 0);

    m_categoryFrame = new CategoryFrame(this);
    m_categoryFrame->initUI(m_leftMargin, m_rightMargin, m_column, m_itemWidth, m_gridwidth);

    QFrame* appBox = new QFrame;
    appBox->setObjectName("AppBox");
    m_appTableWidget = new AppTableWidget(this);
    m_appTableWidget->setGridParameter(m_column, m_gridwidth, m_itemWidth);

    QHBoxLayout* appLayout = new QHBoxLayout(appBox);
    appLayout->addWidget(m_appTableWidget);
    appLayout->setContentsMargins(m_leftMargin, m_topMargin, m_rightMargin, m_bottomMargin);
    appBox->setLayout(appLayout);

    m_layout = new QStackedLayout(this);
    m_layout->addWidget(m_categoryFrame);
    m_layout->addWidget(appBox);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
    m_layout->setCurrentIndex(0);
    m_displayModeFrame = new DisplayModeFrame(this);

    m_searchLineEdit = new SearchLineEdit(this);
    m_searchLineEdit->hide();


//    QPoint start(100, 100);
//    QPixmap pixmap = this->grab();
//    qDebug() << start << "===========" << appBox->geometry() << "xxxxxxxxxxxxxxxxxxx";

//    QPainter p;
//    p.begin(&pixmap);
//    QLinearGradient gradient(pixmap.rect().topLeft(), pixmap.rect().bottomLeft());
//    gradient.setColorAt(0, Qt::white);
//    gradient.setColorAt(0.2, Qt::transparent);
//    gradient.setColorAt(0.3, QColor(0, 0, 0, 0));
//    gradient.setColorAt(0.7, QColor(0, 0, 0, 0));
//    gradient.setColorAt(0.8, Qt::transparent);
//    gradient.setColorAt(1, Qt::white);
//    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
//    p.setBrush(gradient);
//    p.end();

//    pixmap.save("/home/djf/xxxxxxx.png");

//    QLabel * mask = new QLabel(this);
//    mask->setAttribute(Qt::WA_TransparentForMouseEvents);
//    mask->resize(pixmap.size());
//    mask->setPixmap(pixmap);
//    mask->move(start);
//    mask->raise();


}

void LauncherFrame::computerGrid(int minimumLeftMargin, int minimumTopMargin, int miniSpacing, int itemWidth){
    int desktopWidth = qApp->desktop()->screenGeometry().width();
    int desktopHeight = qApp->desktop()->screenGeometry().height();
    m_itemWidth = itemWidth;
    m_column = (desktopWidth - minimumLeftMargin * 2) / (itemWidth + miniSpacing);
    m_spacing = (desktopWidth  - minimumLeftMargin * 2) / m_column - itemWidth;
    m_gridwidth = m_spacing + itemWidth;
    m_leftMargin = (desktopWidth - m_column * m_gridwidth)/ 2;
    m_rightMargin = desktopWidth - m_leftMargin - m_column * m_gridwidth;

    m_row = (desktopHeight - minimumTopMargin) / m_gridwidth;
    m_topMargin = (desktopHeight - m_row * m_gridwidth) / 2;
    m_bottomMargin = desktopHeight - m_row * m_gridwidth - m_topMargin;
    LOG_INFO() << m_column << m_itemWidth << m_spacing << m_leftMargin << m_rightMargin;
    LOG_INFO() << m_row << m_topMargin << m_bottomMargin;
}


void LauncherFrame::initConnect(){
    connect(m_displayModeFrame, SIGNAL(visibleChanged(bool)), this, SLOT(toggleDisableNavgationBar(bool)));
    connect(m_displayModeFrame, SIGNAL(sortModeChanged(int)), this, SLOT(showSortedMode(int)));
    connect(m_displayModeFrame, SIGNAL(categoryModeChanged(int)), this, SLOT(showCategoryMode(int)));
    connect(m_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleSearch(QString)));
    connect(signalManager, SIGNAL(startSearched(QString)), m_searchLineEdit, SLOT(setText(QString)));
    connect(signalManager, SIGNAL(showSearchResult()), this, SLOT(showAppTableWidget()));
    connect(signalManager, SIGNAL(mouseReleased()), this, SLOT(handleMouseReleased()));
    connect(signalManager, SIGNAL(Hide()), this, SLOT(Hide()));
    connect(signalManager, SIGNAL(appOpened(QString)), this, SLOT(handleAppOpened(QString)));
    connect(qApp, SIGNAL(aboutToQuit()), this, SIGNAL(Closed()));
}


void LauncherFrame::toggleDisableNavgationBar(bool flag){
    m_categoryFrame->getNavigationBar()->setDisabled(flag);
}

void LauncherFrame::showSortedMode(int mode){
    LOG_INFO() << mode;
    if (mode == 1){
        showNavigationBarByMode();
    }else{
        showAppTableWidgetByMode(mode);
    }
}

void LauncherFrame::showCategoryMode(int mode){
   m_categoryFrame->getNavigationBar()->setCurrentIndex(mode);
   m_categoryFrame->getNavigationBar();
   m_categoryFrame->getCategoryTabelWidget()->show();
}

void LauncherFrame::showAppTableWidget(){
    m_layout->setCurrentIndex(1);
    m_appTableWidget->clear();
    m_displayModeFrame->hide();
}

void LauncherFrame::showAppTableWidgetByMode(int mode){
    m_layout->setCurrentIndex(1);
    m_displayModeFrame->show();
    m_displayModeFrame->raise();
    m_appTableWidget->showBySortedMode(mode);
}

void LauncherFrame::showNavigationBarByMode(){
    m_layout->setCurrentIndex(0);
    m_displayModeFrame->show();
    m_displayModeFrame->raise();
}

void LauncherFrame::mouseReleaseEvent(QMouseEvent *event){
    qDebug() << event;
    emit signalManager->mouseReleased();
    Hide();
    QFrame::mouseReleaseEvent(event);
}

void LauncherFrame::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        if (m_searchLineEdit->isVisible()){
            hideSearchEdit();
        }else{
            hide();
        }
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Up){
        emit signalManager->keyDirectionPressed(Qt::Key_Up);
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Down) {
        emit signalManager->keyDirectionPressed(Qt::Key_Down);
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Left){
        emit signalManager->keyDirectionPressed(Qt::Key_Left);
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Right){
        emit signalManager->keyDirectionPressed(Qt::Key_Right);
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Return){
        qDebug() << "Enter Pressed";
        if (m_layout->currentIndex() == 0){
            emit signalManager->appOpenedInCategoryMode();
        }else{
            emit signalManager->appOpenedInAppMode();
        }
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Backspace){
        if (m_searchLineEdit->isVisible()){
            m_searchText.remove(m_searchText.length() - 1, 1);
            emit signalManager->startSearched(m_searchText);
        }
    }else if (event->text().trimmed().length() > 0){
        if (!m_searchLineEdit->isVisible()){
            m_searchText.clear();
            m_searchLineEdit->raise();
            m_searchLineEdit->show();
        }
        m_searchText.append(event->text());
        emit signalManager->startSearched(m_searchText);
    }
    QFrame::keyPressEvent(event);
}

void LauncherFrame::closeEvent(QCloseEvent *event){
    qDebug() << event;
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.unregisterObject("/com/deepin/dde/Launcher");
    conn.unregisterService("com.deepin.dde.Launcher");
    qDebug() << "~LauncherFrame";
    QFrame::closeEvent(event);
}

void LauncherFrame::Exit(){
    qApp->quit();
}

void LauncherFrame::Hide(){
    hide();
    m_searchLineEdit->hide();
    m_searchLineEdit->clearFocus();
    m_searchLineEdit->setText("");
    setFocus();
    if (m_layout->currentIndex() == 0){

    }
}

void LauncherFrame::Show(){
    emit signalManager->launcheRefreshed();
    LOG_INFO() << "=============launcheRefreshed";
    show();
    setFocus();
    emit Shown();
}

void LauncherFrame::Toggle(){
    if (isVisible()){
        Hide();
    }else{
        Show();
    }
}

void LauncherFrame::handleMouseReleased(){
    m_clearCheckedButton->click();
}

void LauncherFrame::handleSearch(const QString &text){
    if (text.length() == 0){
        hideSearchEdit();
    }else{
        emit signalManager->search(text);
    }
}

void LauncherFrame::hideSearchEdit(){
    if(m_searchLineEdit->isVisible()){
        m_searchLineEdit->hide();
        m_searchLineEdit->clearFocus();
        setFocus();
        emit signalManager->launcheRefreshed();
    }
}

void LauncherFrame::handleAppOpened(const QString &appUrl){
    Q_UNUSED(appUrl)
    Hide();
}

LauncherFrame::~LauncherFrame()
{
}

