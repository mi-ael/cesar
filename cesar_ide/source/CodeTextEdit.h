#ifndef CODETEXTEDIT_H
#define CODETEXTEDIT_H

#include <memory>

#include <QTextEdit>
#include <QCompleter>

#include "clang-c/Index.h"
//#include "QtQuick/QQuickPaintedItem"
//#include <QColor>

class CodeTextEdit : public QTextEdit
{
	Q_OBJECT
public:
	CodeTextEdit(QWidget* parent = nullptr);

protected:
	void keyPressEvent(QKeyEvent *e) override;
	void focusInEvent(QFocusEvent *e) override;

private:
	void populateCompleterPopupViaClangCodeComplete(const QString &token);
private:
	void insertCompletion(const QString &completion);

private:
	CXIndex clangIndex{};
	std::unique_ptr<QCompleter> completerPopup{};
};

#endif // CODETEXTEDIT_H
