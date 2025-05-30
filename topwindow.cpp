#include "topwindow.h"
#include "ui_topwindow.h"
#include <QAccessibleTextUpdateEvent>


TopWindow::TopWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TopWindow)
{
    ui->setupUi(this);
    ui->headerText->setAccessibleName("Поле вывода");
    ui->headerText->setAccessibleDescription("Здесь отбражается текст игры");
    ui->enterText->setAccessibleName("Поле ввода");
    ui->enterText->setAccessibleDescription("Введите команду для игры");
    announceText(ui->headerText, reference);
    mw = new Manager;
    setWindowTitle("AGL-Manager");
    setMinimumSize(781, 491);
    ui->headerText->installEventFilter(this);
    ui->headerText->setReadOnly(true);

    createActionsName();

    if(!loadPlugin())
    {
        QMessageBox::information(this, "Ошибка!", "Не удалось загрузить плагин!");
    }
    mw->updateLists();
    managerOpen();
    QListWidget* pluginList = mw->getPlugList();
    connect(pluginList, &QListWidget::itemActivated, this, &TopWindow::onPlugSelected);

}

TopWindow::~TopWindow()
{
    delete ui;
    delete mw;
}


void TopWindow::onPlugSelected(QListWidgetItem* item)
{
    if(!item) return;

    QString plugName = item->text();

    for(int i = 0; i < mw->namePlugin.size(); ++i)
    {
        if(mw->namePlugin[i] == plugName)
        {
            gameInterface = pluginsInterface[i];
            if(gameInterface)
            {
                announceSetText(ui->headerText, gameInterface->startMessage());
                disconnect(ui->enterText, &QLineEdit::returnPressed, this, &TopWindow::sendEcho);
                connect(ui->enterText, &QLineEdit::returnPressed, this, &TopWindow::sendEcho);
            }

            else
            {
                QMessageBox::warning(this, "Ошибка", "Плагин не реализует GameInterface");
            }
            break;
        }
    }
}

void TopWindow::createActionsName()
{
    ui->gameManager->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    ui->gameManager->setStatusTip(tr("Открыть менеджер игр"));
    connect(ui->gameManager, &QAction::triggered, this, &TopWindow::managerOpen);

    ui->addGame->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    ui->addGame->setStatusTip(tr("Добавить игру в библиотеку"));

    ui->openSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    ui->openSave->setStatusTip(tr("Открыть сохранение"));

    ui->openSaveDontReload->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    ui->openSaveDontReload->setStatusTip(tr("Открыть сохранение без перезагрузки библиотеки"));

    ui->saveGame->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    ui->saveGame->setStatusTip(tr("Сохранить игру"));


    ui->startGameAgain->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    ui->startGameAgain->setStatusTip(tr("Начать игру заново"));

    ui->resetAllSettings->setStatusTip(tr("Сбросить все настройки"));
    ui->resetAllSettings->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Z);

    ui->quit->setStatusTip(tr("Выход"));
    ui->quit->setShortcut(QKeySequence::Close);
    connect(ui->quit, &QAction::triggered, this, &TopWindow::exit);


    ui->setting->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    ui->setting->setStatusTip("Настройка");


    ui->search->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    ui->search->setStatusTip(tr("Поиск"));

    ui->searchFurther->setShortcut(QKeySequence(Qt::Key_F1));
    ui->searchFurther->setStatusTip(tr("Найти далее"));

    ui->sceneText->setShortcut(QKeySequence(Qt::Key_F2));
    ui->sceneText->setStatusTip(tr("Текст сцены"));

    ui->objectList->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
    ui->objectList->setStatusTip(tr("Перейти к списку объектов"));

    ui->inventList->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_2));
    ui->inventList->setStatusTip(tr("Перейти к списку инвентаря"));

    ui->wayList->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_3));
    ui->wayList->setStatusTip(tr("Перейти к списку путей"));

    ui->debugMode->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    ui->debugMode->setStatusTip(tr("Режим отладки"));

    ui->logOn->setShortcut(QKeySequence::Close);
    ui->logOn->setStatusTip(tr("Включить логирование"));


    ui->musicUp->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F1));
    ui->musicUp->setStatusTip(tr("Музыка громче"));

    ui->musicLow->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F2));
    ui->musicLow->setStatusTip(tr("Музыка тише"));

    ui->switchMusic->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F3));
    ui->switchMusic->setStatusTip(tr("Музыка включена"));

    ui->soundUp->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F1));
    ui->soundUp->setStatusTip(tr("Звуки громче"));

    ui->soundLow->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F2));
    ui->soundLow->setStatusTip(tr("Звуки Тише"));

    ui->switchSound->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F3));
    ui->switchSound->setStatusTip(tr("Звуки включены"));

    ui->alertUp->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_5));
    ui->alertUp->setStatusTip(tr("Уведомления громче"));

    ui->alertLow->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_6));
    ui->alertLow->setStatusTip(tr("Уведомления тише"));

    ui->switchAlert->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_7));
    ui->switchAlert->setStatusTip(tr("Уведомления включены"));

    ui->findOutput->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M));
    ui->findOutput->setStatusTip(tr("Текущий способ вывода"));
}

bool TopWindow::loadPlugin()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    pluginsInterface.clear();
    pluginsLoad.clear();
    pluginsDir.cd("plugins");
    const QStringList entries = pluginsDir.entryList(QDir::Files);
    for (const QString& fileName : entries)
    {
        qDebug() << "Файл в папке plugins:" << fileName;
        if(!fileName.endsWith(".dll", Qt::CaseInsensitive))
            continue;
        qDebug() << "Пробуем загрузить: " << fileName;
        QString fullPath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader* pluginLoad = new QPluginLoader(fullPath, this);
        QObject* plugin = pluginLoad->instance();

        QJsonObject meta = pluginLoad->metaData().value("MetaData").toObject();
        QString displayName = meta.value("name").toString();
        if(displayName.isEmpty())
            displayName = fileName;

        if(!plugin)
        {
            qDebug() << "Ошибка загрузки: " << pluginLoad->errorString();
            delete pluginLoad;
            continue;
        }

        mw->namePlugin.push_back(displayName);

        gameInterface = qobject_cast<GameInterface*>(plugin);
        if(gameInterface)
        {
            pluginsLoad.push_back(pluginLoad);
            pluginsInterface.push_back(gameInterface);
            qDebug() << "Плагин успешно загружен: " << fileName;
        }
        else
        {
            delete pluginLoad;
            continue;
        }
    }

    return !pluginsLoad.isEmpty();
}

void TopWindow::announceText(QWidget *widget, const QString &text)
{
    int oldLength = 0;

    if (auto* plain = qobject_cast<QPlainTextEdit*>(widget))
    {
        plain->appendPlainText(text);
        oldLength = plain->toPlainText().length() - text.length();
        plain->moveCursor(QTextCursor::StartOfBlock);
    }
    else if (auto* edit = qobject_cast<QLineEdit*>(widget))
    {
        edit->setText(text);
    }

    QAccessibleTextUpdateEvent ev(widget, oldLength, "", text);
    QAccessible::updateAccessibility(&ev);

    widget->setFocus();
    QAccessibleEvent focusEvent(widget, QAccessible::Focus);
    QAccessible::updateAccessibility(&focusEvent);
}

void TopWindow::announceSetText(QWidget *widget, const QString &text)
{
    if (auto* plain = qobject_cast<QPlainTextEdit*>(widget))
    {
        plain->setPlainText(text);
        plain->moveCursor(QTextCursor::StartOfBlock);
    }
    else if (auto* edit = qobject_cast<QLineEdit*>(widget))
    {
        edit->setText(text);
    }

    QAccessibleTextUpdateEvent ev(widget, 0, "", text);
    QAccessible::updateAccessibility(&ev);

    widget->setFocus();
    QAccessibleEvent focusEvent(widget, QAccessible::Focus);
    QAccessible::updateAccessibility(&focusEvent);
}

void TopWindow::managerOpen()
{
    ui->headerText->setPlainText(reference);
    disconnect(ui->enterText, &QLineEdit::returnPressed, this, &TopWindow::sendEcho);
    ui->enterText->clear();
    mw->show();
}

void TopWindow::exit()
{
    QWidget::close();
}

void TopWindow::sendEcho()
{
    if(!gameInterface) return;

    announceText(ui->enterText, ui->enterText->text());
    QString text = ">" + gameInterface->gameInput(ui->enterText->text());
    ui->enterText->clear();

    if(gameInterface->isOver())
    {
        QMessageBox::information(this, "End game", tr("Спасибо за игру!"));
        ui->enterText->clear();
        announceSetText(ui->headerText, reference);
        disconnect(ui->enterText, &QLineEdit::returnPressed, this, &TopWindow::sendEcho);
        return;
    }


    ui->headerText->clear();
    announceText(ui->headerText, text);
}

void TopWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter)
    {
        ui->headerText->setFocus();
        return;
    }

    if (ev->key() == Qt::Key_Tab || ev->key() == Qt::Key_Backtab)
    {
        QMainWindow::keyPressEvent(ev);
        return;
    }

    if(ev->text().isEmpty() &&
        (ev->key() == Qt::Key_Shift ||
        ev->key() == Qt::Key_Control ||
        ev->key() == Qt::Key_Alt ||
        ev->key() == Qt::Key_CapsLock))
    {
        return;
    }

    ui->enterText->setFocus();
    QAccessibleEvent aEvent(ui->enterText, QAccessible::Focus);
    QAccessible::updateAccessibility(&aEvent);

    if(!ev->text().isEmpty()) ui->enterText->insert(ev->text());
}

bool TopWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->headerText)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            switch(keyEvent->key())
            {
            case Qt::Key_Up:
                return false;
                break;
            case Qt::Key_Down:
                return false;
                break;
            case Qt::Key_Left:
                return false;
                break;
            case Qt::Key_Right:
                return false;
                break;
            case Qt::CTRL:
                return false;
                break;
            case Qt::Key_Home:
                return false;
                break;
            default:
                TopWindow::keyPressEvent(keyEvent);
                return true;
                break;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

