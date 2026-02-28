#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

// NO pybind11 here — only in pluginmanager.cpp
#include <QString>
#include <QStringList>
#include <QDir>

struct PluginResult {
    bool    success;
    QString output;
    QString error;
};

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    QStringList  discoverPlugins(const QString &pluginDir);
    PluginResult runPlugin(const QString &scriptPath, const QString &inputText);

private:
    bool pythonInitialized;
};

#endif // PLUGINMANAGER_H
