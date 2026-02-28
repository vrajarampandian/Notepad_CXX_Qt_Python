// pybind_include.h MUST be first — before any Qt headers
#include "pybind_include.h"

#include "mainwindow.h"
#include "finddialog.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QTextStream>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QTextCursor>
#include <QRegularExpression>
#include <QVariant>

static const char *PROP_FILEPATH = "filePath";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_spellEnabled(true)
{
    pluginDir = QCoreApplication::applicationDirPath() + "/plugins";
    setupUI();
    setupMenuBar();
    setupStatusBar();
    addNewTab();
    resize(900, 650);
}

MainWindow::~MainWindow() {}

// ── UI Setup ──────────────────────────────────────────────────────────────

void MainWindow::setupUI()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);
    setCentralWidget(tabWidget);

    connect(tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::closeTab);
    connect(tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onTabChanged);
}

void MainWindow::setupMenuBar()
{
    // File
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New",        this, &MainWindow::newFile,    QKeySequence::New);
    fileMenu->addAction("&Open...",    this, &MainWindow::openFile,   QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("&Save",       this, &MainWindow::saveFile,   QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::saveFileAs, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    QAction *closeTabAct = new QAction("&Close Tab", this);
    closeTabAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    connect(closeTabAct, &QAction::triggered, this,
            [this](){ closeTab(tabWidget->currentIndex()); });
    fileMenu->addAction(closeTabAct);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

    // Edit
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo", this,
                        [this](){ if (currentEditor()) currentEditor()->undo(); }, QKeySequence::Undo);
    editMenu->addAction("&Redo", this,
                        [this](){ if (currentEditor()) currentEditor()->redo(); }, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t",  this,
                        [this](){ if (currentEditor()) currentEditor()->cut();  }, QKeySequence::Cut);
    editMenu->addAction("&Copy", this,
                        [this](){ if (currentEditor()) currentEditor()->copy(); }, QKeySequence::Copy);
    editMenu->addAction("&Paste",this,
                        [this](){ if (currentEditor()) currentEditor()->paste();}, QKeySequence::Paste);
    editMenu->addAction("Select &All", this,
                        [this](){ if (currentEditor()) currentEditor()->selectAll(); }, QKeySequence::SelectAll);
    editMenu->addSeparator();
    editMenu->addAction("&Find / Replace...", this,
                        &MainWindow::findReplace, QKeySequence::Find);

    // Format
    QMenu *formatMenu = menuBar()->addMenu("F&ormat");
    formatMenu->addAction("&Font...", this, &MainWindow::setFont);
    QAction *wwAct = formatMenu->addAction("&Word Wrap",
                                           this, &MainWindow::toggleWordWrap);
    wwAct->setCheckable(true);
    wwAct->setChecked(true);

    // Tools
    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    spellCheckAction = toolsMenu->addAction("&Spell Check");
    spellCheckAction->setCheckable(true);
    spellCheckAction->setChecked(true);
    connect(spellCheckAction, &QAction::toggled,
            this, &MainWindow::toggleSpellCheck);

    // Help
    menuBar()->addMenu("&Help")->addAction("&About", this, &MainWindow::aboutApp);

    // Plugins
    pluginMenu = menuBar()->addMenu("&Plugins");
    pluginMenu->addAction("&Reload Plugins", this, &MainWindow::loadPluginMenu);
    pluginMenu->addSeparator();
    loadPluginMenu();
}

void MainWindow::setupStatusBar()
{
    statusLabel = new QLabel("Line 1, Col 1");
    statusBar()->addPermanentWidget(statusLabel);
}

// ── Tab Helpers ───────────────────────────────────────────────────────────

void MainWindow::addNewTab(const QString &fileName)
{
    QTextEdit *editor = new QTextEdit();
    editor->setFont(QFont("Courier New", 11));
    editor->setContextMenuPolicy(Qt::CustomContextMenu);
    editor->setProperty(PROP_FILEPATH, fileName);

    // Each tab gets its own SpellChecker
    SpellChecker *sc = new SpellChecker(editor->document());
    sc->setEnabled(m_spellEnabled);

    connect(editor, &QTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::documentModified);
    connect(editor, &QTextEdit::customContextMenuRequested,
            this, &MainWindow::showContextMenu);

    QString title = fileName.isEmpty() ? "Untitled"
                                       : QFileInfo(fileName).fileName();
    int idx = tabWidget->addTab(editor, title);
    tabWidget->setCurrentIndex(idx);
    editor->setFocus();
}

QTextEdit *MainWindow::currentEditor()
{
    return qobject_cast<QTextEdit *>(tabWidget->currentWidget());
}

QString MainWindow::tabFilePath(int index)
{
    auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(index));
    return ed ? ed->property(PROP_FILEPATH).toString() : QString();
}

void MainWindow::setTabFilePath(int index, const QString &path)
{
    auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(index));
    if (ed) ed->setProperty(PROP_FILEPATH, path);
}

void MainWindow::updateTabTitle(int index)
{
    auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(index));
    if (!ed) return;
    QString path     = ed->property(PROP_FILEPATH).toString();
    bool    modified = ed->document()->isModified();
    QString title    = path.isEmpty() ? "Untitled" : QFileInfo(path).fileName();
    if (modified) title.prepend("*");
    tabWidget->setTabText(index, title);
    tabWidget->setTabToolTip(index, path.isEmpty() ? "Untitled" : path);
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index)
    updateStatusBar();
    auto *ed = currentEditor();
    if (ed) {
        QString path  = ed->property(PROP_FILEPATH).toString();
        QString title = path.isEmpty() ? "Untitled" : QFileInfo(path).fileName();
        setWindowTitle(title + " - Notepad");
    }
}

// ── File Operations ───────────────────────────────────────────────────────

void MainWindow::newFile()
{
    addNewTab();
}

void MainWindow::openFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this, "Open File", "",
        "Text Files (*.txt);;XML Files (*.xml);;All Files (*)");
    for (const QString &fn : files)
        loadFile(fn);
}

bool MainWindow::saveFile()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return false;
    QString path = tabFilePath(idx);
    return path.isEmpty() ? saveFileAs() : saveTabToFile(idx, path);
}

bool MainWindow::saveFileAs()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return false;
    QString fn = QFileDialog::getSaveFileName(
        this, "Save File As", "", "Text Files (*.txt);;All Files (*)");
    return fn.isEmpty() ? false : saveTabToFile(idx, fn);
}

void MainWindow::loadFile(const QString &fileName)
{
    // Switch to tab if already open
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabFilePath(i) == fileName) {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Notepad",
                             QString("Cannot read file %1:\n%2").arg(fileName, file.errorString()));
        return;
    }

    QString content = QTextStream(&file).readAll();

    // Reuse current tab if it's empty and unsaved
    auto *ed    = currentEditor();
    int   idx   = tabWidget->currentIndex();
    bool  reuse = ed &&
                 ed->toPlainText().isEmpty() &&
                 !ed->document()->isModified() &&
                 tabFilePath(idx).isEmpty();

    if (reuse) {
        ed->setPlainText(content);
        setTabFilePath(idx, fileName);
        ed->document()->setModified(false);
        updateTabTitle(idx);
    } else {
        addNewTab(fileName);
        currentEditor()->setPlainText(content);
        currentEditor()->document()->setModified(false);
        updateTabTitle(tabWidget->currentIndex());
    }

    statusBar()->showMessage("File loaded", 2000);
}

bool MainWindow::saveTabToFile(int index, const QString &fileName)
{
    auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(index));
    if (!ed) return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Notepad",
                             QString("Cannot write %1:\n%2").arg(fileName, file.errorString()));
        return false;
    }

    QTextStream(&file) << ed->toPlainText();
    setTabFilePath(index, fileName);
    ed->document()->setModified(false);
    updateTabTitle(index);
    statusBar()->showMessage("File saved", 2000);
    return true;
}

bool MainWindow::maybeSaveTab(int index)
{
    auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(index));
    if (!ed || !ed->document()->isModified()) return true;

    QString path  = tabFilePath(index);
    QString title = path.isEmpty() ? "Untitled" : QFileInfo(path).fileName();

    auto ret = QMessageBox::warning(this, "Notepad",
                                    QString("'%1' has unsaved changes.\nSave before closing?").arg(title),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        if (path.isEmpty()) {
            path = QFileDialog::getSaveFileName(this, "Save File As", "",
                                                "Text Files (*.txt);;All Files (*)");
            if (path.isEmpty()) return false;
        }
        return saveTabToFile(index, path);
    }
    return ret != QMessageBox::Cancel;
}

void MainWindow::closeTab(int index)
{
    if (!maybeSaveTab(index)) return;
    tabWidget->removeTab(index);
    if (tabWidget->count() == 0) addNewTab();
}

// ── Edit / Format / Help ──────────────────────────────────────────────────

void MainWindow::findReplace()
{
    auto *ed = currentEditor();
    if (ed) { FindDialog *dlg = new FindDialog(ed, this); dlg->show(); }
}

void MainWindow::setFont()
{
    auto *ed = currentEditor();
    if (!ed) return;
    bool ok;
    QFont f = QFontDialog::getFont(&ok, ed->font(), this, "Select Font");
    if (!ok) return;
    for (int i = 0; i < tabWidget->count(); ++i) {
        auto *e = qobject_cast<QTextEdit *>(tabWidget->widget(i));
        if (e) e->setFont(f);
    }
}

void MainWindow::toggleWordWrap()
{
    auto *a = qobject_cast<QAction *>(sender());
    if (!a) return;
    auto mode = a->isChecked() ? QTextEdit::WidgetWidth : QTextEdit::NoWrap;
    for (int i = 0; i < tabWidget->count(); ++i) {
        auto *e = qobject_cast<QTextEdit *>(tabWidget->widget(i));
        if (e) e->setLineWrapMode(mode);
    }
}

void MainWindow::aboutApp()
{
    QMessageBox::about(this, "About Notepad",
                       "<b>Qt Notepad</b><br>Multi-tab editor with Python plugins and spell check.");
}

// ── Status Bar ────────────────────────────────────────────────────────────

void MainWindow::updateStatusBar()
{
    auto *ed = currentEditor();
    if (!ed) return;
    QTextCursor c = ed->textCursor();
    statusLabel->setText(QString("Line %1, Col %2")
                             .arg(c.blockNumber() + 1)
                             .arg(c.columnNumber() + 1));
}

void MainWindow::documentModified()
{
    updateTabTitle(tabWidget->currentIndex());
}

// ── Spell Check ───────────────────────────────────────────────────────────

void MainWindow::toggleSpellCheck(bool enabled)
{
    m_spellEnabled = enabled;
    for (int i = 0; i < tabWidget->count(); ++i) {
        auto *ed = qobject_cast<QTextEdit *>(tabWidget->widget(i));
        if (ed) {
            auto *sc = ed->findChild<SpellChecker *>();
            if (sc) sc->setEnabled(enabled);
        }
    }
    statusBar()->showMessage(enabled ? "Spell check ON" : "Spell check OFF", 2000);
}

QString MainWindow::wordUnderCursor(QTextCursor &cursor)
{
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    auto *ed = qobject_cast<QTextEdit *>(sender());
    if (!ed) return;

    QMenu *menu = ed->createStandardContextMenu();
    auto  *sc   = ed->findChild<SpellChecker *>();

    if (sc && sc->isEnabled()) {
        QTextCursor cursor = ed->cursorForPosition(pos);
        QString word = wordUnderCursor(cursor);

        if (!word.isEmpty() && sc->isMisspelled(word)) {
            QStringList suggs = sc->suggestions(word);
            if (!suggs.isEmpty()) {
                menu->insertSeparator(menu->actions().first());
                auto *header = new QAction(
                    QString("Suggestions for \"%1\":").arg(word), menu);
                header->setEnabled(false);
                menu->insertAction(menu->actions().first(), header);
                for (const QString &s : suggs) {
                    auto *act = new QAction(s, menu);
                    connect(act, &QAction::triggered, this, [this, s]() {
                        replaceWordUnderCursor(s);
                    });
                    menu->insertAction(menu->actions().first(), act);
                }
                menu->insertSeparator(menu->actions().first());
            }
        }
    }

    menu->exec(ed->viewport()->mapToGlobal(pos));
    delete menu;
}

void MainWindow::replaceWordUnderCursor(const QString &replacement)
{
    auto *ed = currentEditor();
    if (!ed) return;
    QTextCursor cursor = ed->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(replacement);
}

// ── Plugin System ─────────────────────────────────────────────────────────

void MainWindow::loadPluginMenu()
{
    auto acts = pluginMenu->actions();
    for (int i = 2; i < acts.size(); ++i)
        pluginMenu->removeAction(acts[i]);

    QStringList plugins = pluginManager.discoverPlugins(pluginDir);
    if (plugins.isEmpty()) {
        auto *none = pluginMenu->addAction("No plugins found");
        none->setEnabled(false);
        return;
    }
    for (const QString &path : plugins) {
        QString name = QFileInfo(path).baseName().replace('_', ' ');
        auto *act = pluginMenu->addAction(name);
        connect(act, &QAction::triggered, this, [this, path]() {
            runPlugin(path);
        });
    }
}

void MainWindow::runPlugin(const QString &scriptPath)
{
    auto *ed = currentEditor();
    if (!ed) return;
    PluginResult result = pluginManager.runPlugin(scriptPath, ed->toPlainText());
    if (result.success) {
        ed->setPlainText(result.output);
        statusBar()->showMessage(
            QString("Plugin '%1' applied.")
                .arg(QFileInfo(scriptPath).baseName()), 3000);
    } else {
        QMessageBox::critical(this, "Plugin Error", result.error);
    }
}

// ── Close Event ───────────────────────────────────────────────────────────

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        tabWidget->setCurrentIndex(i);
        if (!maybeSaveTab(i)) { event->ignore(); return; }
    }
    event->accept();
}
