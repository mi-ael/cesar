#include "CodeTextEdit.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QMessageBox>
#include <QDebug>


using namespace std;


const char* TextEditWithClangCodeCompletion_DUMMY_FILENAME = "dummyfile.cpp";

CodeTextEdit::CodeTextEdit(QWidget* parent)
	: completerPopup(make_unique<QCompleter>(this))
{
	setText(tr("//Start typing some C/C++, Ctrl+Space activates code completion\n\n//Demo:\ntypedef int CodeCompleteMe0\ntypedef int CodeCompleteMe1;\nCodeCo\n//          ^Click here and press Ctrl+Space"));
	completerPopup->setCaseSensitivity(Qt::CaseInsensitive);
	completerPopup->setWidget(this);
	completerPopup->setCompletionMode(QCompleter::PopupCompletion);
	connect(completerPopup.get(), SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

	clangIndex = clang_createIndex(1, 0);
	if(!clangIndex)
	{
		QMessageBox::critical(this, tr("Error"), tr("clang_createIndex failed"));
		close();
	}

	resize(500, 300);
	setWindowTitle(tr("Text Edit with Clang Code Completion"));
}


void CodeTextEdit::keyPressEvent(QKeyEvent *e) {
	if(completerPopup->popup()->isVisible())
	{
		switch (e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return;
			default:
				break;
		}
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
	if(!isShortcut)
		QTextEdit::keyPressEvent(e);

	static QString endOfWordCharacters("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
	bool hasModifier = (e->modifiers() != Qt::NoModifier);

	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	QString completionPrefix = tc.selectedText();

	if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || endOfWordCharacters.contains(e->text().right(1))))
	{
		completerPopup->popup()->hide();
		return;
	}

	populateCompleterPopupViaClangCodeComplete(completionPrefix);

	if(completionPrefix != completerPopup->completionPrefix())
	{
		completerPopup->setCompletionPrefix(completionPrefix);
		completerPopup->popup()->setCurrentIndex(completerPopup->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(completerPopup->popup()->sizeHintForColumn(0) + completerPopup->popup()->verticalScrollBar()->sizeHint().width());
	completerPopup->complete(cr);
}

void CodeTextEdit::focusInEvent(QFocusEvent *e) {
	completerPopup->setWidget(this);
	QTextEdit::focusInEvent(e);
}

void CodeTextEdit::populateCompleterPopupViaClangCodeComplete(const QString &token) {
	CXUnsavedFile unsavedFiles[1];
	unsavedFiles[0].Filename = TextEditWithClangCodeCompletion_DUMMY_FILENAME;
	QString docString = document()->toPlainText();
	std::string docStdString = docString.toStdString();
	unsavedFiles[0].Contents = docStdString.c_str();
	unsavedFiles[0].Length = docStdString.length();

	CXTranslationUnit translationUnit = clang_parseTranslationUnit(clangIndex, TextEditWithClangCodeCompletion_DUMMY_FILENAME, 0, 0, unsavedFiles, 1, CXTranslationUnit_PrecompiledPreamble);
	if(!translationUnit)
	{
		QMessageBox::critical(this, tr("Error"), tr("parseTranslationUnit failed"));
		completerPopup->setModel(new QStringListModel(completerPopup.get()));
		return;
	}

	//clang_reparseTranslationUnit(u, 0, 0, 0);

	int line = textCursor().blockNumber()+1;
	qDebug() << "Line: " << line;
	int column = (textCursor().positionInBlock()+1)-token.length(); //columNumber instead?
	qDebug() << "Column: " << column;

	CXCodeCompleteResults* codeCompleteResults = clang_codeCompleteAt(translationUnit, TextEditWithClangCodeCompletion_DUMMY_FILENAME,
																	  (unsigned int) line, (unsigned int) column, unsavedFiles, 1, 0);
	if(!codeCompleteResults)
	{
		QMessageBox::critical(this, tr("Error"), tr("Could not complete"));
		completerPopup->setModel(new QStringListModel(completerPopup.get()));
		return;
	}

	QStringList codeCompletionEntries;
	for(unsigned i = 0; i < codeCompleteResults->NumResults; ++i)
	{
		const CXCompletionString& str = codeCompleteResults->Results[i].CompletionString;
		for(unsigned j = 0; j < clang_getNumCompletionChunks(str); ++j)
		{
			if(clang_getCompletionChunkKind(str, j) != CXCompletionChunk_TypedText)
				continue;

			const CXString& out = clang_getCompletionChunkText(str, j);
			const char* codeCompletionEntry = clang_getCString(out);
			QString codeCompletionEntryQString = QString::fromLatin1(codeCompletionEntry,
																	 (int) strlen(codeCompletionEntry));
			if(!codeCompletionEntryQString.trimmed().isEmpty())
				codeCompletionEntries.append(codeCompletionEntry);
		}
	}

	completerPopup->setModel(new QStringListModel(codeCompletionEntries, completerPopup.get()));
	clang_disposeCodeCompleteResults(codeCompleteResults);
}

void CodeTextEdit::insertCompletion(const QString &completion) {
	if(completerPopup->widget() != this)
		return;
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	tc.removeSelectedText(); //capitalization may have been wrong, since we're doing case insensitive completing
	tc.insertText(completion);
	setTextCursor(tc);
}
