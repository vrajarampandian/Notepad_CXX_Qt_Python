#include "finddialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextCursor>

FindDialog::FindDialog(QTextEdit *editor, QWidget *parent)
    : QDialog(parent), textEdit(editor)
{
    setWindowTitle("Find & Replace");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    findEdit    = new QLineEdit(this);
    replaceEdit = new QLineEdit(this);
    caseCheck      = new QCheckBox("Match case",       this);
    wholeWordCheck = new QCheckBox("Whole words only", this);

    QFormLayout *form = new QFormLayout;
    form->addRow("Find:",    findEdit);
    form->addRow("Replace:", replaceEdit);

    QVBoxLayout *optL = new QVBoxLayout;
    optL->addWidget(caseCheck);
    optL->addWidget(wholeWordCheck);
    QGroupBox *optBox = new QGroupBox("Options");
    optBox->setLayout(optL);

    QPushButton *findBtn       = new QPushButton("Find Next",   this);
    QPushButton *replaceBtn    = new QPushButton("Replace",     this);
    QPushButton *replaceAllBtn = new QPushButton("Replace All", this);
    QPushButton *closeBtn      = new QPushButton("Close",       this);
    findBtn->setDefault(true);

    QVBoxLayout *btnL = new QVBoxLayout;
    btnL->addWidget(findBtn);
    btnL->addWidget(replaceBtn);
    btnL->addWidget(replaceAllBtn);
    btnL->addStretch();
    btnL->addWidget(closeBtn);

    QVBoxLayout *leftL = new QVBoxLayout;
    leftL->addLayout(form);
    leftL->addWidget(optBox);

    QHBoxLayout *main = new QHBoxLayout(this);
    main->addLayout(leftL);
    main->addLayout(btnL);

    connect(findBtn,       &QPushButton::clicked, this, &FindDialog::findNext);
    connect(replaceBtn,    &QPushButton::clicked, this, &FindDialog::replace);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindDialog::replaceAll);
    connect(closeBtn,      &QPushButton::clicked, this, &QDialog::close);
}

void FindDialog::findNext()
{
    QString s = findEdit->text();
    if (s.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (caseCheck->isChecked())      flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheck->isChecked()) flags |= QTextDocument::FindWholeWords;

    if (!textEdit->find(s, flags)) {
        QTextCursor c = textEdit->textCursor();
        c.movePosition(QTextCursor::Start);
        textEdit->setTextCursor(c);
        if (!textEdit->find(s, flags))
            QMessageBox::information(this, "Find", "Text not found.");
    }
}

void FindDialog::replace()
{
    QTextCursor c = textEdit->textCursor();
    if (c.hasSelection() &&
        c.selectedText().compare(findEdit->text(),
            caseCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0)
        c.insertText(replaceEdit->text());
    findNext();
}

void FindDialog::replaceAll()
{
    QString s = findEdit->text();
    if (s.isEmpty()) return;
    Qt::CaseSensitivity cs = caseCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QString content = textEdit->toPlainText();
    int count = content.count(s, cs);
    if (count == 0) { QMessageBox::information(this, "Replace All", "Text not found."); return; }
    content.replace(s, replaceEdit->text(), cs);
    textEdit->setPlainText(content);
    QMessageBox::information(this, "Replace All",
                             QString("Replaced %1 occurrence(s).").arg(count));
}
