// pybind_include.h MUST be the very first include
#include "pybind_include.h"
namespace py = pybind11;

#include "spellchecker.h"
#include <QDebug>
#include <QRegularExpression>

// How long to wait after last keystroke before spell checking (milliseconds)
// Increase this if still slow, decrease for faster feedback
static const int DEBOUNCE_MS = 600;

SpellChecker::SpellChecker(QTextDocument *parent)
    : QSyntaxHighlighter(parent),
      m_enabled(true),
      m_pythonReady(false)
{
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    m_errorFormat.setUnderlineColor(Qt::red);

    // Debounce timer — single shot, fires after user stops typing
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(DEBOUNCE_MS);
    connect(m_debounceTimer, &QTimer::timeout,
            this, &SpellChecker::onRehighlightTimer);

    // Delay Python init slightly so app opens faster
    QTimer::singleShot(500, this, [this]() {
        m_pythonReady = initPython();
        if (m_pythonReady && m_enabled)
            rehighlight();
    });
}

SpellChecker::~SpellChecker() {}

bool SpellChecker::initPython()
{
    try {
        py::gil_scoped_acquire acquire;
        py::module_::import("spellchecker");
        return true;
    } catch (const py::error_already_set &e) {
        qWarning() << "pyspellchecker not found. Run: pip install pyspellchecker";
        qWarning() << e.what();
        return false;
    }
}

void SpellChecker::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_cache.clear();   // clear cache when toggled
    rehighlight();
}

// Called by QSyntaxHighlighter on every block (line) change
void SpellChecker::highlightBlock(const QString &text)
{
    if (!m_enabled || !m_pythonReady) return;

    static QRegularExpression wordRegex(R"(\b[a-zA-Z']+\b)");
    auto it = wordRegex.globalMatch(text);

    while (it.hasNext()) {
        auto    match = it.next();
        QString word  = match.captured();
        if (word.length() < 2) continue;

        // Use cached result — avoids calling Python for already-known words
        if (isMisspelled(word))
            setFormat(match.capturedStart(), match.capturedLength(), m_errorFormat);
    }

    // Restart debounce timer on every block change
    // This means we only fully rehighlight after user pauses typing
    m_debounceTimer->start();
}

void SpellChecker::onRehighlightTimer()
{
    // Only rehighlight if enabled — this is the deferred full-document check
    if (m_enabled && m_pythonReady)
        rehighlight();
}

bool SpellChecker::isMisspelled(const QString &word)
{
    if (!m_pythonReady || word.length() < 2) return false;

    QString lower = word.toLower();

    // Return cached result immediately — no Python call needed
    if (m_cache.contains(lower))
        return m_cache[lower];

    // Not cached yet — call Python and store result
    bool result = checkWordViaPython(lower);
    m_cache[lower] = result;
    return result;
}

bool SpellChecker::checkWordViaPython(const QString &word)
{
    try {
        py::gil_scoped_acquire acquire;

        // Reuse a single SpellChecker instance stored as a Python global
        // so we don't recreate it on every word check
        py::object builtins = py::module_::import("builtins");
        py::object globals  = py::globals();

        if (!globals.contains("_spell_instance")) {
            py::object SpellClass = py::module_::import("spellchecker").attr("SpellChecker");
            globals["_spell_instance"] = SpellClass();
        }

        py::object spell = globals["_spell_instance"];
        py::list   wlist;
        wlist.append(word.toStdString());
        return py::len(spell.attr("unknown")(wlist)) > 0;

    } catch (const std::exception &e) {
        qWarning() << "checkWordViaPython:" << e.what();
        return false;
    }
}

QStringList SpellChecker::suggestions(const QString &word, int maxCount)
{
    QStringList result;
    if (!m_pythonReady) return result;
    try {
        py::gil_scoped_acquire acquire;

        py::object globals = py::globals();
        if (!globals.contains("_spell_instance")) {
            py::object SpellClass = py::module_::import("spellchecker").attr("SpellChecker");
            globals["_spell_instance"] = SpellClass();
        }

        py::object spell      = globals["_spell_instance"];
        py::object candidates = spell.attr("candidates")(word.toLower().toStdString());
        if (candidates.is_none()) return result;

        int count = 0;
        for (auto &c : candidates) {
            if (count++ >= maxCount) break;
            result << QString::fromStdString(c.cast<std::string>());
        }
    } catch (const std::exception &e) {
        qWarning() << "suggestions:" << e.what();
    }
    return result;
}
