#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// NO pybind11 here — only Qt and our own headers
#include <QMainWindow>
#include <QTabWidget>
#include <QTextEdit>
#include <QLabel>
#include <QMenu>
#include "pluginmanager.h"
#include "spellchecker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void closeTab(int index);
    void onTabChanged(int index);
    void findReplace();
    void setFont();
    void toggleWordWrap();
    void aboutApp();
    void updateStatusBar();
    void documentModified();
    void loadPluginMenu();
    void runPlugin(const QString &scriptPath);
    void toggleSpellCheck(bool enabled);
    void showContextMenu(const QPoint &pos);
    void replaceWordUnderCursor(const QString &replacement);

private:
    void          addNewTab(const QString &fileName = QString());
    QTextEdit    *currentEditor();
    bool          maybeSaveTab(int index);
    bool          saveTabToFile(int index, const QString &fileName);
    void          updateTabTitle(int index);
    QString       tabFilePath(int index);
    void          setTabFilePath(int index, const QString &path);
    void          setupUI();
    void          setupMenuBar();
    void          setupStatusBar();
    void          loadFile(const QString &fileName);
    QString       wordUnderCursor(QTextCursor &cursor);

    QTabWidget    *tabWidget;
    QLabel        *statusLabel;
    QMenu         *pluginMenu;
    QString        pluginDir;
    PluginManager  pluginManager;
    QAction       *spellCheckAction;
    bool           m_spellEnabled;
};

#endif // MAINWINDOW_H
