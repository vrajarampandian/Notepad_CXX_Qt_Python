#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

// NO pybind11 here — pybind only lives in spellchecker.cpp
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QStringList>
#include <QHash>
#include <QTimer>

class SpellChecker : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SpellChecker(QTextDocument *parent = nullptr);
    ~SpellChecker();

    void        setEnabled(bool enabled);
    bool        isEnabled()  const { return m_enabled; }
    bool        isMisspelled(const QString &word);
    QStringList suggestions(const QString &word, int maxCount = 5);

protected:
    void highlightBlock(const QString &text) override;

private slots:
    void onRehighlightTimer();

private:
    bool initPython();
    bool checkWordViaPython(const QString &word);

    bool             m_enabled;
    bool             m_pythonReady;
    QTextCharFormat  m_errorFormat;
    QHash<QString, bool> m_cache;
    QTimer          *m_debounceTimer;
};

#endif // SPELLCHECKER_H
