#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

#include <rgl_application.h>

#include "diagram_colors.h"
#include "person_details_widget.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonDetailsWidget::PersonDetailsWidget(const FTree *tree, QWidget *parent)
    : QWidget(parent)
    , tree(tree)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->htmlViewer = new RHtmlViewer;
    // Links lead to persons, not to documents.
    this->htmlViewer->setOpenLinks(false);
    mainLayout->addWidget(this->htmlViewer);

    this->populate();

    QObject::connect(this->htmlViewer,&QTextBrowser::anchorClicked,this,&PersonDetailsWidget::onAnchorClicked);

    QObject::connect(tree,&FTree::personChanged,this,&PersonDetailsWidget::onPersonChanged);
    // Colors of the generated page are chosen for the color scheme, so the page has to be generated again.
    QObject::connect(RApplication::instance(),&RApplication::styleApplied,this,&PersonDetailsWidget::onStyleApplied);
}

void PersonDetailsWidget::setPersons(const QList<QUuid> &personsIds)
{
    this->personsIds = personsIds;
    this->populate();
}

void PersonDetailsWidget::populate()
{
    this->htmlViewer->setHtml(this->generateHtml());
    // this->htmlViewer->setMarkdown(this->generateMarkdown());
}

QString PersonDetailsWidget::generateMarkdown() const
{
    QString content;

    bool isFirst = true;

    foreach (QUuid personId, this->personsIds)
    {
        if (!isFirst)
        {
            content += "\n---\n\n";
        }
        isFirst = false;

        FPerson person = this->tree->findPerson(personId);

        content += "## " + person.getName().toString() + '\n';
        content += '\n';
        if (!person.getPicture().isEmpty())
        {
            if (!person.getPicture().getTitle().isEmpty())
            {
                content += "**" + person.getPicture().getTitle() + "** " + '\n';
            }
            content += "![Personal photo](data:image/png;base64," + person.getPicture().getData().toBase64() + ")" + '\n';
            if (!person.getPicture().getDescription().isEmpty())
            {
                content += "*" + person.getPicture().getDescription() + "* " + '\n';
            }
        }
        content += '\n';
        content += "- " + tr("Sex") + ": " + _FP_TR(FPerson::sexKeyToDesc(person.getSex())) + '\n';

        QString bornDateString = person.getBirth().getDate().toLocaleString();
        QString bornPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString bornSeparator = !bornDateString.isEmpty() && !bornPlaceString.isEmpty() ? ", " : QString();

        content += "- " + tr("Born") + ": " + bornDateString + bornSeparator + bornPlaceString + '\n';

        QString deathDateString = person.getDeath().getDate().toLocaleString();
        QString deathPlaceString = person.getDeath().getPlace().getAddress().toString();
        QString deathSeparator = !deathDateString.isEmpty() && !deathPlaceString.isEmpty() ? ", " : QString();

        content += "- " + tr("Died") + ": " + deathDateString + deathSeparator + deathPlaceString + '\n';

        QList<QUuid> partnerRelations = this->tree->findPersonPartnerRelations(personId);
        QList<QUuid> childRelations = this->tree->findPersonChildRelations(personId);

        if (childRelations.size() > 0)
        {
            content += "### " + tr("Parents and siblings") + '\n';
            content += '\n';

            foreach (QUuid relationId, childRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "#### " + tr("Parents")  + " [" + relation.getBegin().getDate().toLocaleString() + " - " + relation.getEnd().getDate().toLocaleString() + "]" + '\n';
                content += '\n';
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners())) {
                    FPerson parent = this->tree->findPerson(parentId);
                    content += "- " + parent.getName().toString() + " [" + parent.getBirth().getDate().toLocaleString() + " - " + parent.getDeath().getDate().toLocaleString() + "]" + '\n';
                }
                content += '\n';

                content += "#### " + tr("Siblings") + '\n';
                content += '\n';
                foreach (QUuid childId, this->tree->sortPersons(relation.getChildren())) {
                    if (childId == personId)
                    {
                        continue;
                    }
                    FPerson child = this->tree->findPerson(childId);
                    content += "- " + child.getName().toString() + " [" + child.getBirth().getDate().toLocaleString() + " - " + child.getDeath().getDate().toLocaleString() + "]" + '\n';
                }
                content += '\n';
            }
        }

        if (partnerRelations.size() > 0)
        {
            content += "### " + tr("Partner and children") + '\n';
            content += '\n';
            foreach (QUuid relationId, partnerRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "#### " + tr("Partner") + " [" + relation.getBegin().getDate().toLocaleString() + " - " + relation.getEnd().getDate().toLocaleString() + "]" + '\n';
                content += '\n';
                foreach (QUuid partnerId, relation.getPartners()) {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    FPerson partner = this->tree->findPerson(partnerId);
                    content += "- " + partner.getName().toString() + " [" + partner.getBirth().getDate().toLocaleString() + " - " + partner.getDeath().getDate().toLocaleString() + "]" + '\n';
                }
                content += '\n';

                content += "#### " + tr("Children") + '\n';
                content += '\n';
                foreach (QUuid childId, relation.getChildren()) {
                    FPerson child = this->tree->findPerson(childId);
                    content += "- " + child.getName().toString() + " [" + child.getBirth().getDate().toLocaleString() + " - " + child.getDeath().getDate().toLocaleString() + "]" + '\n';
                }
                content += '\n';
            }
        }

        if (!person.getText().isEmpty())
        {
            content += "### " + tr("Additional information") + '\n';
            content += '\n';
            content += person.getText().toHtmlEscaped() + '\n';
        }
    }

    return content;
}

QString PersonDetailsWidget::personLink(const FPerson &person)
{
    return "<a href=\"person:" + person.getId().toString(QUuid::WithoutBraces) + "\">" + person.getName().toString().toHtmlEscaped() + "</a>";
}

QString PersonDetailsWidget::periodString(const FDate &begin, const FDate &end)
{
    if (begin.isEmpty() && end.isEmpty())
    {
        return QString();
    }
    return "<span style=\"color: " + DiagramColors::textColorSecondary().name() + "; font-weight: normal;\">"
           + begin.toLocaleString(QLocale::ShortFormat) + " – " + end.toLocaleString(QLocale::ShortFormat)
           + "</span>";
}

QString PersonDetailsWidget::personLinkWithDates(const FPerson &person)
{
    return PersonDetailsWidget::personLink(person)
           + "&nbsp;&nbsp;" + PersonDetailsWidget::periodString(person.getBirth().getDate(),person.getDeath().getDate());
}

QString PersonDetailsWidget::eventString(const FEvent &event)
{
    QString dateString = event.getDate().toLocaleString();
    QString placeString = event.getPlace().getAddress().toString().toHtmlEscaped();
    QString separator = !dateString.isEmpty() && !placeString.isEmpty() ? ", " : QString();
    return dateString + separator + placeString;
}

QString PersonDetailsWidget::rowHtml(const QString &label, const QString &value)
{
    return "<tr><td class=\"label\" width=\"25%\">" + label + "</td><td>" + value + "</td></tr>";
}

QString PersonDetailsWidget::personsRowHtml(const QString &label, const QList<QUuid> &personsIds, const QUuid &skippedId) const
{
    QStringList links;
    foreach (QUuid id, personsIds)
    {
        if (id != skippedId)
        {
            links.append(PersonDetailsWidget::personLinkWithDates(this->tree->findPerson(id)));
        }
    }
    if (links.isEmpty())
    {
        return QString();
    }
    return PersonDetailsWidget::rowHtml(label,links.join("<br/>"));
}

QString PersonDetailsWidget::generateHtml() const
{
    // Same colors as the cards of the family tree diagram.
    const QString border = DiagramColors::borderColor().name();
    const QString background = DiagramColors::boxBackground().name();
    const QString text = DiagramColors::textColorPrimary().name();
    const QString secondaryText = DiagramColors::textColorSecondary().name();
    const QString link = DiagramColors::borderColorSelected().name();
    const QString sectionBackground = DiagramColors::relationHeaderTop().name();
    const QString sectionText = DiagramColors::relationTextPrimary().name();

    const QString tableStart = "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"6\">";

    QString content;

    content += "<!DOCTYPE html>";
    content += "<html>";
    content += "<head>";
    content += "<title>" + tr("Person details") + "</title>";
    content += "<style type=\"text/css\">";
    content += "table { border: 1px solid " + border + "; border-collapse: collapse; background: " + background + "; color: " + text + "; }";
    content += "td { border-bottom: 1px solid " + border + "; }";
    content += "td.title { font-size: large; font-weight: bold; }";
    content += "td.section { background: " + sectionBackground + "; color: " + sectionText + "; font-weight: bold; }";
    content += "td.label { color: " + secondaryText + "; }";
    content += "td.plain { border: 0px; }";
    content += "a { color: " + link + "; text-decoration: none; }";
    content += "p.hint { color: " + secondaryText + "; }";
    content += "</style>";
    content += "</head>";
    content += "<body>";

    if (this->personsIds.isEmpty())
    {
        content += "<p class=\"hint\" align=\"center\"><br/>" + tr("Select a person in the list of persons to see the details.") + "</p>";
    }
    else if (this->personsIds.count() > PersonDetailsWidget::maxDetailedPersons)
    {
        // Full details of many persons, with their pictures, would be long and slow to build.
        content += tableStart;
        content += "<tr><td colspan=\"3\" class=\"title\" style=\"background: " + DiagramColors::headerNeutral().name() + ";\">" + tr("Selected persons") + " (" + QLocale().toString(this->personsIds.count()) + ")</td></tr>";
        content += "<tr><td class=\"label\">" + tr("Name") + "</td><td class=\"label\">" + tr("Born") + "</td><td class=\"label\">" + tr("Died") + "</td></tr>";
        foreach (QUuid personId, this->tree->sortPersons(this->personsIds))
        {
            FPerson person = this->tree->findPerson(personId);
            content += "<tr>";
            content += "<td>" + PersonDetailsWidget::personLink(person) + "</td>";
            content += "<td>" + PersonDetailsWidget::eventString(person.getBirth()) + "</td>";
            content += "<td>" + PersonDetailsWidget::eventString(person.getDeath()) + "</td>";
            content += "</tr>";
        }
        content += "</table>";
        content += "<p class=\"hint\">" + tr("Click a name to see the details of the person.") + "</p>";
    }
    else
    {
        foreach (QUuid personId, this->personsIds)
        {
            FPerson person = this->tree->findPerson(personId);

            QColor titleBackground = DiagramColors::headerNeutral();
            switch (FPerson::sexDescToIndex(person.getSex()))
            {
                case FPerson::SexIndex::male:   titleBackground = DiagramColors::headerMale(); break;
                case FPerson::SexIndex::female: titleBackground = DiagramColors::headerFemale(); break;
                case FPerson::SexIndex::other:  titleBackground = DiagramColors::headerOther(); break;
                default: break;
            }

            content += tableStart;
            content += "<tr><td colspan=\"2\" class=\"title\" style=\"background: " + titleBackground.name() + ";\">" + person.getName().toString().toHtmlEscaped() + "</td></tr>";
            if (!person.getPicture().isEmpty())
            {
                content += "<tr><td colspan=\"2\">";
                content += "<table style=\"border: 0px;\" cellspacing=\"0\" cellpadding=\"0\">";
                content += "<tr>";
                content += "<td class=\"plain\">";
                content += "<img src=\"data:image/png;base64," + person.getPicture().getData().toBase64() + "\">";
                content += "</td>";
                content += "<td class=\"plain\" style=\"padding-left: 10px;\">";
                content += "<strong>" + person.getPicture().getTitle().toHtmlEscaped() + "</strong><br/>" + person.getPicture().getDescription().toHtmlEscaped();
                content += "</td>";
                content += "</tr>";
                content += "</table>";
                content += "</td></tr>";
            }
            content += PersonDetailsWidget::rowHtml(tr("Sex"),_FP_TR(FPerson::sexKeyToDesc(person.getSex())));
            content += PersonDetailsWidget::rowHtml(tr("Born"),PersonDetailsWidget::eventString(person.getBirth()));
            content += PersonDetailsWidget::rowHtml(tr("Died"),PersonDetailsWidget::eventString(person.getDeath()));

            foreach (QUuid relationId, this->tree->findPersonChildRelations(personId))
            {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><td colspan=\"2\" class=\"section\">" + tr("Parents and siblings") + "&nbsp;&nbsp;" + PersonDetailsWidget::periodString(relation.getBegin().getDate(),relation.getEnd().getDate()) + "</td></tr>";
                content += this->personsRowHtml(tr("Parents"),this->tree->sortPersons(relation.getPartners()));
                content += this->personsRowHtml(tr("Siblings"),this->tree->sortPersons(relation.getChildren()),personId);
            }

            foreach (QUuid relationId, this->tree->findPersonPartnerRelations(personId))
            {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><td colspan=\"2\" class=\"section\">" + tr("Partner and children") + "&nbsp;&nbsp;" + PersonDetailsWidget::periodString(relation.getBegin().getDate(),relation.getEnd().getDate()) + "</td></tr>";
                content += this->personsRowHtml(tr("Partner"),relation.getPartners(),personId);
                content += this->personsRowHtml(tr("Children"),relation.getChildren());
            }

            if (!person.getText().isEmpty())
            {
                content += "<tr><td colspan=\"2\" class=\"section\">" + tr("Additional information") + "</td></tr>";
                content += "<tr><td colspan=\"2\"><p style=\"white-space: pre-wrap;\">" + person.getText().toHtmlEscaped() + "</p></td></tr>";
            }

            content += "</table>";
            content += "<br/>";
        }
    }
    content += "</body>";
    content += "</html>";

    return content;
}

void PersonDetailsWidget::onAnchorClicked(const QUrl &url)
{
    if (url.scheme() == "person")
    {
        emit this->personActivated(QUuid::fromString(url.path()));
    }
}

void PersonDetailsWidget::onPersonChanged(const FPerson &person)
{
    if (personsIds.contains(person.getId()))
    {
        this->populate();
    }
}

void PersonDetailsWidget::onStyleApplied()
{
    this->populate();
}
