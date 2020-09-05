/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "screenplaysubsetreport.h"

ScreenplaySubsetReport::ScreenplaySubsetReport(QObject *parent)
    : AbstractScreenplaySubsetReport(parent)
{

}

ScreenplaySubsetReport::~ScreenplaySubsetReport()
{

}

void ScreenplaySubsetReport::setSceneNumbers(const QList<int> &val)
{
    if(m_sceneNumbers == val)
        return;

    m_sceneNumbers = val;
    std::sort(m_sceneNumbers.begin(), m_sceneNumbers.end());
    emit sceneNumbersChanged();
}

void ScreenplaySubsetReport::setGenerateSummary(bool val)
{
    if(m_generateSummary == val)
        return;

    m_generateSummary = val;
    emit generateSummaryChanged();
}

bool ScreenplaySubsetReport::includeScreenplayElement(const ScreenplayElement *element) const
{
    if(element->scene() == nullptr)
        return false;

    return m_sceneNumbers.isEmpty() || m_sceneNumbers.contains(element->sceneNumber());
}

QString ScreenplaySubsetReport::screenplaySubtitle() const
{
    if(m_sceneNumbers.isEmpty())
        return QStringLiteral("All scenes of the screenplay.");

    return QStringLiteral("Snapshot of ") + QString::number(m_sceneNumbers.size()) + QStringLiteral(" scene(s).");
}

void ScreenplaySubsetReport::configureScreenplayTextDocument(ScreenplayTextDocument &stDoc)
{
    Q_UNUSED(stDoc);
}

void ScreenplaySubsetReport::inject(QTextCursor &cursor, AbstractScreenplayTextDocumentInjectionInterface::InjectLocation location)
{
    AbstractScreenplaySubsetReport::inject(cursor, location);

    if(location != AfterTitlePage || !m_generateSummary)
        return;

    const QFont defaultFont = this->document()->printFormat()->defaultFont();

    QTextBlockFormat defaultBlockFormat;

    QTextCharFormat defaultCharFormat;
    defaultCharFormat.setFontFamily(defaultFont.family());
    defaultCharFormat.setFontPointSize(12);

    QTextBlockFormat blockFormat = defaultBlockFormat;
    blockFormat.setAlignment(Qt::AlignLeft);
    blockFormat.setTopMargin(20);

    QTextCharFormat charFormat = defaultCharFormat;
    charFormat.setFontPointSize(20);
    charFormat.setFontCapitalization(QFont::AllUppercase);
    charFormat.setFontWeight(QFont::Bold);
    charFormat.setFontItalic(true);

    cursor.insertBlock(blockFormat, charFormat);
    cursor.insertText("SUMMARY:");

    blockFormat = defaultBlockFormat;
    blockFormat.setIndent(1);

    charFormat = defaultCharFormat;

    cursor.insertBlock(blockFormat, charFormat);
    cursor.insertText(QStringLiteral("The following scenes are included in this screenplay:"));

    blockFormat.setIndent(2);

    const Screenplay *screenplay = this->document()->screenplay();
    for(int i=0; i<screenplay->elementCount(); i++)
    {
        const ScreenplayElement *element = screenplay->elementAt(i);
        if( !this->includeScreenplayElement(element) )
            continue;

        cursor.insertBlock(blockFormat, charFormat);
        cursor.insertText( QStringLiteral("[") + QString::number(element->sceneNumber()) + QStringLiteral("] - ") + element->scene()->heading()->text() );
    }

    blockFormat = defaultBlockFormat;
    blockFormat.setPageBreakPolicy(QTextBlockFormat::PageBreak_AlwaysAfter);
    charFormat = defaultCharFormat;
    cursor.insertBlock(blockFormat, charFormat);
    cursor.insertText(QStringLiteral("-- end of scene list --"));
}
