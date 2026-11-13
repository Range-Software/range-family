#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

#include <rgl_application.h>

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

        QString bornDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString bornPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString bornSeparator = !bornDateString.isEmpty() && !bornPlaceString.isEmpty() ? ", " : QString();

        content += "- " + tr("Born") + ": " + bornDateString + bornSeparator + bornPlaceString + '\n';

        QString deathDateString = QLocale().toString(person.getDeath().getDate().toDate());
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

                content += "#### " + tr("Parents")  + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]" + '\n';
                content += '\n';
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners())) {
                    FPerson parent = this->tree->findPerson(parentId);
                    content += "- " + parent.getName().toString() + " [" + QLocale().toString(parent.getBirth().getDate().toDate()) + " - " + QLocale().toString(parent.getDeath().getDate().toDate()) + "]" + '\n';
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
                    content += "- " + child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]" + '\n';
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

                content += "#### " + tr("Partner") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]" + '\n';
                content += '\n';
                foreach (QUuid partnerId, relation.getPartners()) {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    FPerson partner = this->tree->findPerson(partnerId);
                    content += "- " + partner.getName().toString() + " [" + QLocale().toString(partner.getBirth().getDate().toDate()) + " - " + QLocale().toString(partner.getDeath().getDate().toDate()) + "]" + '\n';
                }
                content += '\n';

                content += "#### " + tr("Children") + '\n';
                content += '\n';
                foreach (QUuid childId, relation.getChildren()) {
                    FPerson child = this->tree->findPerson(childId);
                    content += "- " + child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]" + '\n';
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

QString PersonDetailsWidget::personLinkWithDates(const FPerson &person)
{
    return PersonDetailsWidget::personLink(person)
           + " [" + QLocale().toString(person.getBirth().getDate().toDate())
           + " - " + QLocale().toString(person.getDeath().getDate().toDate()) + "]";
}

QString PersonDetailsWidget::eventString(const FEvent &event)
{
    QString dateString = QLocale().toString(event.getDate().toDate());
    QString placeString = event.getPlace().getAddress().toString().toHtmlEscaped();
    QString separator = !dateString.isEmpty() && !placeString.isEmpty() ? ", " : QString();
    return dateString + separator + placeString;
}

QString PersonDetailsWidget::generateHtml() const
{
    const bool darkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    const QString titleBackground = darkMode ? "#161616" : "lightgrey";
    const QString sectionBackground = darkMode ? "#242424" : "#E8E8E8";

    QString content;

    content += "<!DOCTYPE html>";
    content += "<html lang=\"en\">";
    content += "<head>";
    content += "<title>" + tr("Person details") + "</title>";
    content += "<style type=\"text/css\">";
    content += "table, th, td {\
                    border: 1px solid " + QString(darkMode ? "gray" : "darkgrey") + "; \
                    border-collapse: collapse; \
                    background: " + QString(darkMode ? "#323232" : "whitesmoke") + "; \
                    text-align: left; \
                    color: " + QString(darkMode ? "white" : "black") + "; \
                }";
    content += "</style>";
    content += "</head>";
    content += "<body>";

    if (this->personsIds.isEmpty())
    {
        content += "<p align=\"center\" style=\"color: gray;\"><br/>" + tr("Select a person in the list of persons to see the details.") + "</p>";
    }
    else if (this->personsIds.count() > PersonDetailsWidget::maxDetailedPersons)
    {
        // Full details of many persons, with their pictures, would be long and slow to build.
        content += "<table width=\"100%\">";
        content += "<tr><th colspan=\"3\" align=\"center\" style=\"background: " + titleBackground + ";\">" + tr("Selected persons") + " (" + QLocale().toString(this->personsIds.count()) + ")</th></tr>";
        content += "<tr><th>" + tr("Name") + "</th><th>" + tr("Born") + "</th><th>" + tr("Died") + "</th></tr>";
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
        content += "<p style=\"color: gray;\">" + tr("Click a name to see the details of the person.") + "</p>";
    }
    else
    {
        foreach (QUuid personId, this->personsIds)
        {
            FPerson person = this->tree->findPerson(personId);
            content += "<table width=\"100%\">";
            content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + titleBackground + ";\">" + person.getName().toString().toHtmlEscaped() + "</th></tr>";
            if (!person.getPicture().isEmpty())
            {
                content += "<tr><td colspan=\"2\" align=\"left\">";
                content += "<table style=\"border: 0px\">";
                content += "<tr>";
                content += "<td style=\"border: 0px\">";
                content += "<img src=\"data:image/png;base64," + person.getPicture().getData().toBase64() + "\">";
                content += "</td>";
                content += "<td style=\"border: 0px\">";
                content += "<strong>" + person.getPicture().getTitle().toHtmlEscaped() + "</strong><br/>" + person.getPicture().getDescription().toHtmlEscaped();
                content += "</td>";
                content += "</tr>";
                content += "</table>";
                content += "</td></tr>";
            }
            content += "<tr><th>" + tr("Sex")  + "</th><td width=\"80%\">" + _FP_TR(FPerson::sexKeyToDesc(person.getSex())) + "</td></tr>";
            content += "<tr><th>" + tr("Born") + "</th><td>" + PersonDetailsWidget::eventString(person.getBirth()) + "</td></tr>";
            content += "<tr><th>" + tr("Died") + "</th><td>" + PersonDetailsWidget::eventString(person.getDeath()) + "</td></tr>";

            QList<QUuid> partnerRelations = this->tree->findPersonPartnerRelations(personId);
            QList<QUuid> childRelations = this->tree->findPersonChildRelations(personId);

            foreach (QUuid relationId, childRelations)
            {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + sectionBackground + ";\">" + tr("Parents and siblings") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th></tr>";
                content += "<tr><th>" + tr("Parents") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners()))
                {
                    content += "<li>" + PersonDetailsWidget::personLinkWithDates(this->tree->findPerson(parentId)) + "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";

                content += "<tr><th>" + tr("Siblings") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid childId, this->tree->sortPersons(relation.getChildren()))
                {
                    if (childId == personId)
                    {
                        continue;
                    }
                    content += "<li>" + PersonDetailsWidget::personLinkWithDates(this->tree->findPerson(childId)) + "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";
            }

            foreach (QUuid relationId, partnerRelations)
            {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + sectionBackground + ";\">" + tr("Partner and children") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th></tr>";
                content += "<tr><th>" + tr("Partner") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid partnerId, relation.getPartners())
                {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    content += "<li>" + PersonDetailsWidget::personLinkWithDates(this->tree->findPerson(partnerId)) + "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";

                content += "<tr><th>" + tr("Children") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid childId, relation.getChildren())
                {
                    content += "<li>" + PersonDetailsWidget::personLinkWithDates(this->tree->findPerson(childId)) + "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";
            }

            if (!person.getText().isEmpty())
            {
                content += "<tr><th colspan=\"2\" align=\"center\">" + tr("Additional information") + "</th></tr>";
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
