#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QTextEdit *editor, QWidget *parent = nullptr);

private slots:
    void findNext();
    void replace();
    void replaceAll();

private:
    QTextEdit   *textEdit;
    QLineEdit   *findEdit;
    QLineEdit   *replaceEdit;
    QCheckBox   *caseCheck;
    QCheckBox   *wholeWordCheck;
};

#endif // FINDDIALOG_H
