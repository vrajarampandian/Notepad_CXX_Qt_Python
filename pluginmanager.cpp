// ── pybind_include.h MUST be the very first include ──────────────────────
#include "pybind_include.h"
namespace py = pybind11;
// ─────────────────────────────────────────────────────────────────────────

#include "pluginmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

static py::scoped_interpreter *s_interpreter = nullptr;
static int                     s_refCount    = 0;

PluginManager::PluginManager() : pythonInitialized(false)
{
    if (s_refCount == 0) {
        try {
            s_interpreter     = new py::scoped_interpreter();
            pythonInitialized = true;
        } catch (const std::exception &e) {
            qWarning() << "Python init failed:" << e.what();
        }
    } else {
        pythonInitialized = true;
    }
    ++s_refCount;
}

PluginManager::~PluginManager()
{
    if (--s_refCount == 0) {
        delete s_interpreter;
        s_interpreter = nullptr;
    }
}

QStringList PluginManager::discoverPlugins(const QString &pluginDir)
{
    QStringList list;
    QDir dir(pluginDir);
    if (!dir.exists()) { dir.mkpath("."); return list; }
    for (const auto &fi : dir.entryInfoList({"*.py"}, QDir::Files))
        list << fi.absoluteFilePath();
    return list;
}

PluginResult PluginManager::runPlugin(const QString &scriptPath, const QString &inputText)
{
    PluginResult result{false, "", ""};
    if (!pythonInitialized) {
        result.error = "Python interpreter not available.";
        return result;
    }
    QFile file(scriptPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        result.error = "Cannot open: " + scriptPath;
        return result;
    }
    QString src = QTextStream(&file).readAll();
    try {
        py::gil_scoped_acquire acquire;
        py::dict ns;
        ns["input_text"]  = inputText.toStdString();
        ns["output_text"] = inputText.toStdString();
        py::exec(src.toStdString(), py::globals(), ns);
        result.output  = QString::fromStdString(ns["output_text"].cast<std::string>());
        result.success = true;
    } catch (const py::error_already_set &e) {
        result.error = QString("Python error:\n%1").arg(e.what());
    } catch (const std::exception &e) {
        result.error = QString("Error:\n%1").arg(e.what());
    }
    return result;
}
